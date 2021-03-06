/* wrnote.c - Wind River ELF note header */

/*
 * Copyright (c) 2007 Wind River Systems, Inc.
 *
 */

/*
modification history
--------------------
01a,04jul07,fle  written
*/

/* -------------------------------- includes -------------------------------- */

#include <linux/utsname.h>
#include <linux/elfcore.h>	/* for elf_prpsinfo structure at least */
#include <linux/fs.h>		/* for  SEEK_SET end other fs defines ... */
#include <linux/list.h>		/* for list related calls (list_add() ...) */
#include <linux/string.h>	/* for memcpy() at least */
#include <linux/unistd.h>	/* for open()/lseek()/close() ... */
#include <linux/slab.h>		/* for kfree() ... */
#include <linux/wrnote.h>	/* for WR note types */
#include <linux/wrs_sys_info.h> /* for SYS_INFO macros */

#if defined(CONFIG_64BIT) && (defined(CONFIG_MIPS32_N32) || defined(CONFIG_MIPS32_O32))
#include <linux/compat.h>

/* following should consist with  arch/mips/kernel/binfmt_elfo32.c
 * and arch/mips/kernel/binfmt_elfn32.c
 */

#define elf_prstatus elf_prstatus32
struct elf_prstatus32
{
	struct elf_siginfo pr_info;	/* Info associated with signal */
	short	pr_cursig;		/* Current signal */
	unsigned int pr_sigpend;	/* Set of pending signals */
	unsigned int pr_sighold;	/* Set of held signals */
	pid_t	pr_pid;
	pid_t	pr_ppid;
	pid_t	pr_pgrp;
	pid_t	pr_sid;
	struct compat_timeval pr_utime;	/* User time */
	struct compat_timeval pr_stime;	/* System time */
	struct compat_timeval pr_cutime;/* Cumulative user time */
	struct compat_timeval pr_cstime;/* Cumulative system time */
	elf_gregset_t pr_reg;	/* GP registers */
	int pr_fpvalid;		/* True if math co-processor being used.  */
};

#define elf_prpsinfo elf_prpsinfo32
struct elf_prpsinfo32
{
	char	pr_state;	/* numeric process state */
	char	pr_sname;	/* char for pr_state */
	char	pr_zomb;	/* zombie */
	char	pr_nice;	/* nice val */
	unsigned int pr_flag;	/* flags */
	__kernel_uid_t	pr_uid;
	__kernel_gid_t	pr_gid;
	pid_t	pr_pid, pr_ppid, pr_pgrp, pr_sid;
	/* Lots missing */
	char	pr_fname[16];	/* filename of executable */
	char	pr_psargs[ELF_PRARGSZ];	/* initial part of arg list */
};
#endif

/* -------------------------------- defines --------------------------------- */

#define WR_GET_SIZE	1	/* get note size info */
#define WR_GET_NOTE	2	/* get note itself */

/* -------------------------------- typedefs -------------------------------- */

struct wr_note_node {
	struct list_head list;	/* list pointer */
	void *ptr;		/* pointer to WR note data */
};

/* -------------------------- forward declarations -------------------------- */

static void add_wr_note_data(void *ptr);

static int get_wr_kernel_info(void *in, void *out, int info_type);

static int get_wr_note_info(int type,	/* type of note to get info for */
			    void *in,	/* input note info */
			    void *out,	/* output note info */
			    int info_type /* type of info to get */);

static int get_wr_prstatus_info(void *in, void *out, int info_type);

static int get_wr_ps_info(void *in, void *out, int info_type);

/* ---------------------------- local variables ----------------------------- */

static struct list_head ptrList;	/* WR notes data ptr list */
static int listInitialised;		/* is ptr list initialised */
static DEFINE_SPINLOCK(ptrList_lock); /* spinlock govering access to ptrList */

/* --------------------------- exported routines ---------------------------- */

/**
 * free_wr_note_data - free all wr note data pointer from list.
 *
 * All pointers allocated while callinf \e get_wr_note() are freed by
 * calling this routine.
 *
 * \see	get_wr_note()
 */

void free_wr_note_data(void)
{
	struct list_head *pFirst = NULL;	/* first node in list */
	struct wr_note_node *pNode = NULL;	/* ptr node in list */

	/* initialise the list if needed */
	spin_lock(&ptrList_lock);
	if (listInitialised == 0) {
		INIT_LIST_HEAD(&ptrList);
		listInitialised = 1;
	}

	/* free every node and its associated pointer */

	while (!list_empty(&ptrList)) {
		pFirst = ptrList.next;
		list_del(pFirst);

		pNode =
		    (struct wr_note_node *)list_entry(pFirst,
						      struct wr_note_node,
						      list);

		/* free pointer and node */

		kfree(pNode->ptr);
		kfree(pNode);
	}
	spin_unlock(&ptrList_lock);
}

/**
 * get_wr_note_size - get a Wind River note size.
 *
 * Get the note size for a WR note of type \e type. This should help to
 * allocate memory when getting the note itself.
 *
 * \param	type	Type of the input data. Might be any of
 *			\b NT_WR_KERNEL_INFO, \b NT_WR_PRPSINFO,
 *			\b NT_WR_PRSTATUS
 * \param	in	Input data to read and get size of
 *
 * \return	The size of a WR note data buffer to save \e in data.
 *
 * \see	get_wr_note
 */

int get_wr_note_size(int type,	/* type of input data */
		     void *in	/* input data */
    )
{
	return get_wr_note_info(type, in, NULL, WR_GET_SIZE);
}

/**
 * get_wr_note - fill a Wind River note data.
 *
 * The data pointed at by \e in is parsed as a \e type data, and saved in
 * a Wind River note buffer allocated depending on the type and content of
 * \e in.
 *
 * This buffer is returned by this routine, and should be freed once useless,
 * using the \e free_wr_note_data() routine.
 *
 * \param	type	Type of the input data. Might be any of
 *			\b NT_WR_KERNEL_INFO, \b NT_WR_PRPSINFO,
 *			\b NT_WR_PRSTATUS
 * \param	in	Input data to read and save in the WR note data
 *
 * \return	A pointer to an allocated buffer containing saved data for \e in
 *
 * \see	free_wr_note_data()
 * \see	get_wr_note_size()
 */

void *get_wr_note(int type,	/* type of the input data */
		  void *in	/* input data to save in dest */
    )
{
	void *pNoteData = NULL;	/* note data */
	int ntSz = 0;		/* note data size */

	/* allocate room for the note data */

	ntSz = get_wr_note_size(type, in);

	pNoteData = kmalloc(ntSz, GFP_KERNEL);
	if (pNoteData != NULL) {
		get_wr_note_info(type, in, pNoteData, WR_GET_NOTE);
		add_wr_note_data(pNoteData);
	}

	return pNoteData;
}


/* ----------------------------- local routines ----------------------------- */

/**
 * get_wr_note_info - get a Wind River note specific info.
 *
 * Get Wind River note data for a given note type.
 *
 * If \e info_type is set to \b WR_GET_SIZE, this routine will just parse
 * input data of given \e type, and return the size needed to store data
 * in a Wind River note.
 *
 * If \e info_type is set to \b WR_GET_NOTE, the output \e out parameter is
 * considered big enough to store the appropriate data from \e in, and is
 * filled with this info.
 *
 * \param	type		Type of the input data. Might be any of
 *				\b NT_WR_KERNEL_INFO, \b NT_WR_PRPSINFO,
 *				\b NT_WR_PRSTATUS
 * \param	in		Input data to read
 * \param	out		Output buffer to write data to if \e info_type
 *				is set to \b WR_GET_NOTE
 * \param	info_type	Type of information to get. Can be either
 *				\b WR_GET_SIZE to only get the note data size,
 *				or \b WR_GET_NOTE to actually write note data
 *				to \e out
 *
 * \return	The size of the Wind River note data.
 */

static int get_wr_note_info(int type,	/* type of note to get info for */
			    void *in,	/* input note info */
			    void *out,	/* output note info */
			    int info_type	/* type of info to get */
    )
{
	int res = 0;		/* returned value */

	switch (type) {
	case NT_WR_KERNEL_INFO:

		res = get_wr_kernel_info(in, out, info_type);
		break;

	case NT_WR_PRPSINFO:

		res = get_wr_ps_info(in, out, info_type);
		break;

	case NT_WR_PRSTATUS:

		res = get_wr_prstatus_info(in, out, info_type);
		break;
	}

	return res;
}

/**
 * get_wr_kernel_info - get WR kernel info.
 *
 * \param	in		input data (should be \b NULL here)
 * \param	out		data to save kernel info to
 * \param	info_type	Type of info to get. Can be either
 *				\b WR_GET_SIZE or \b WR_GET_NOTE
 *
 * \return	The kernel info data size
 */

static int get_wr_kernel_info(void *in, void *out, int info_type)
{
	struct wr_elf_note_data note;	/* note data to write */
	unsigned int vuint32 = 0;	/* unsigned int value */
	void *cursor = out;	/* dest memory ptr */
	char *archName = NULL;	/* Architecture name */
	char *osVersion = NULL;	/* OS version */
	char *bspName = NULL;	/* BSP Name */
	char *bspShortName = NULL;	/* BSP Short Name */
	char *compilerVersion = NULL;	/* compiler version */
	int dataSize = 0;	/* written data size */
	int ix = 0;		/* loop counter */
	int res = 0;		/* returned size */

	archName = init_utsname()->machine;
	osVersion = init_utsname()->release;
	bspName = WRS_SYS_INFO_BSP;
	bspShortName = WRS_SYS_INFO_SHORTNAME;
	compilerVersion = (char *)linux_compiler;

	for (ix = 0; ix < 6; ix++) {
		if (ix == 0) {
			/* note version */

			note.type = WR_ELF_UINT32;
			note.count = 1;

			vuint32 = 1;
			note.data = &vuint32;

			dataSize = 4;
		} else if (ix == 1) {
			/* OS version */

			note.type = WR_ELF_CHAR;
			note.count = strlen(osVersion) + 1;
			note.data = osVersion;
			dataSize = note.count;
		} else if (ix == 2) {
			/* BSP name */

			note.type = WR_ELF_CHAR;
			note.count = strlen(bspName) + 1;
			note.data = bspName;
			dataSize = note.count;
		} else if (ix == 3) {
			/* BSP short name */

			note.type = WR_ELF_CHAR;
			note.count = strlen(bspShortName) + 1;
			note.data = bspShortName;
			dataSize = note.count;
		} else if (ix == 4) {
			/* architecture name */

			note.type = WR_ELF_CHAR;
			note.count = strlen(archName) + 1;
			note.data = archName;
			dataSize = note.count;
		} else if (ix == 5) {
			/* compiler version and name */

			note.type = WR_ELF_CHAR;
			note.count = strlen(compilerVersion) + 1;
			note.data = compilerVersion;
			dataSize = note.count;
		}

		/* now copy information in data out if needed */

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, &note.type, sizeof(note.type));

		cursor += sizeof(note.type);
		res += sizeof(note.type);

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, &note.count, sizeof(note.count));

		cursor += sizeof(note.count);
		res += sizeof(note.count);

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, note.data, dataSize);

		cursor += dataSize;
		res += dataSize;
	}

	return res;
}

/**
 * get_wr_ps_info - get WR process info.
 *
 * \param	in		input data pointer (elf_prpsinfo structure)
 * \param	out		pointer to save process info info to
 * \param	info_type	Type of info to get. Can be either
 *				\b WR_GET_SIZE or \b WR_GET_NOTE
 *
 * \return	The process info data size
 */

static int get_wr_ps_info(void *in, void *out, int info_type)
{
	struct elf_prpsinfo *pPsinfo = NULL;	/* process info */
	unsigned long long vuint64 = 0;	/* unsigned 64 value */
	struct wr_elf_note_data note;	/* note data to write */
	void *cursor = out;	/* dest memory ptr */
	int dataSize = 0;	/* written data size */
	int ix = 0;		/* loop counter */
	int res = 0;		/* returned size */

	/* pr_gid, pr_pid, pr_ppid, pr_fname, pr_psargs */

	pPsinfo = (struct elf_prpsinfo *)in;

	for (ix = 0; ix < 5; ix++) {
		note.type = WR_ELF_UINT64;
		note.count = 1;

		if (ix == 0) {
			/* program group */

			vuint64 = (unsigned long long)pPsinfo->pr_gid;
			note.data = &vuint64;

			dataSize = 8;
		} else if (ix == 1) {
			/* program ID */

			vuint64 = (unsigned long long)pPsinfo->pr_pid;
			note.data = &vuint64;

			dataSize = 8;
		} else if (ix == 2) {
			/* program parent ID */

			vuint64 = (unsigned long long)pPsinfo->pr_ppid;
			note.data = &vuint64;

			dataSize = 8;
		} else if (ix == 3) {
			/* program name */

			note.type = WR_ELF_CHAR;
			note.count = strlen(pPsinfo->pr_fname) + 1;
			note.data = pPsinfo->pr_fname;
			dataSize = note.count;
		} else if (ix == 4) {
			/* program arguments */

			note.type = WR_ELF_CHAR;
			note.count = strlen(pPsinfo->pr_psargs) + 1;
			note.data = pPsinfo->pr_psargs;
			dataSize = note.count;
		}

		/* now copy information in data out if needed */

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, &note.type, sizeof(note.type));

		cursor += sizeof(note.type);
		res += sizeof(note.type);

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, &note.count, sizeof(note.count));

		cursor += sizeof(note.count);
		res += sizeof(note.count);

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, note.data, dataSize);

		cursor += dataSize;
		res += dataSize;
	}

	return res;
}

/**
 * get_wr_prstatus_info - get WR process status info.
 *
 * \param	in		input data pointer (elf_prstatus structure)
 * \param	out		pointer to save process status info to
 * \param	info_type	Type of info to get. Can be either
 *				\b WR_GET_SIZE or \b WR_GET_NOTE
 *
 * \return	The process status data size
 */

static int get_wr_prstatus_info(void *in, void *out, int info_type)
{
	struct elf_prstatus *pPrstatus = NULL;	/* process status */
	unsigned long long vuint64 = 0;	/* unsigned 64 value */
	struct wr_elf_note_data note;	/* note data to write */
	short int vshort = 0;	/* short value */
	void *cursor = out;	/* dest memory ptr */
	int dataSize = 0;	/* written data size */
	int ix = 0;		/* loop counter */
	int res = 0;		/* returned size */

	/* pr_cursig, pr_pid, pr_ppid, pr_reg, pr_fpreg */

	pPrstatus = (struct elf_prstatus *)in;

	for (ix = 0; ix < 4; ix++) {
		note.type = WR_ELF_UINT64;
		note.count = 1;

		if (ix == 0) {
			/* program current signal */

			note.type = WR_ELF_INT16;
			vshort = pPrstatus->pr_cursig;
			note.data = &vshort;

			dataSize = sizeof(vshort);
		} else if (ix == 1) {
			/* program id */

			vuint64 = (unsigned long long)pPrstatus->pr_pid;
			note.data = &vuint64;

			dataSize = 8;
		} else if (ix == 2) {
			/* program parent id */

			vuint64 = (unsigned long long)pPrstatus->pr_ppid;
			note.data = &vuint64;

			dataSize = 8;
		} else if (ix == 3) {
			/* general purpose registers */

			if (sizeof(elf_greg_t) == 4) {
				note.type = WR_ELF_UINT32;
				dataSize = 4 * ELF_NGREG;
			} else {
				note.type = WR_ELF_UINT64;
				dataSize = 8 * ELF_NGREG;
			}

			note.count = ELF_NGREG;
			note.data = pPrstatus->pr_reg;
		}

		/* now copy information in data out if needed */

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, &note.type, sizeof(note.type));

		cursor += sizeof(note.type);
		res += sizeof(note.type);

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, &note.count, sizeof(note.count));

		cursor += sizeof(note.count);
		res += sizeof(note.count);

		if (info_type == WR_GET_NOTE)
			memcpy(cursor, note.data, dataSize);

		cursor += dataSize;
		res += dataSize;
	}

	return res;
}

/**
 * add_wr_note_data - add a note data pointer to list of pointers.
 *
 * The pointers list is initialised if needed.
 *
 * \param	ptr	Pointer to add to pointer list.
 *
 * \see	free_wr_note_data()
 */

static void add_wr_note_data(void *ptr)
{
	struct wr_note_node *pNode = NULL;	/* node to add to list */

	/* initialise the list if needed */
	spin_lock(&ptrList_lock);
	if (listInitialised == 0) {
		INIT_LIST_HEAD(&ptrList);
		listInitialised = 1;
	}

	/* allocate memeory for the node to add to the list */
	pNode = kzalloc(sizeof(*pNode), GFP_ATOMIC);
	if (pNode != NULL) {
		pNode->ptr = ptr;
		list_add(&pNode->list, &ptrList);
	}
	spin_unlock(&ptrList_lock);
}
