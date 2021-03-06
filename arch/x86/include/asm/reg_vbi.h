/*
 * x86 regs_vbi.h - x86 cpu registers
 *
 * Copyright (c) 2007-2011 Wind River Systems, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */

#ifndef __ASM_REG_VBI_H
#define __ASM_REG_VBI_H

#ifdef CONFIG_64BIT
#ifndef LP64
#define LP64
#endif
#endif

#define IO_BMAP_SIZE	32	/* IO bitmap for port 0x000 - 0x3ff */

#ifndef _ASMLANGUAGE

#ifdef LP64
typedef struct			/* REG_SET - x86 register set	*/
    {
    _RType   rax;		/* 00: general register		*/
    _RType   rbx;		/* 08: general register		*/
    _RType   rcx;		/* 10: general register		*/
    _RType   rdx;		/* 18: general register		*/
    _RType   rsp;		/* 20: stack pointer register	*/
    _RType   rbp;		/* 28: frame pointer register	*/
    _RType   rsi;		/* 30: general register		*/
    _RType   rdi;		/* 38: general register		*/
    _RType   r8;		/* 40: general register		*/
    _RType   r9;		/* 48: general register		*/
    _RType   r10;		/* 50: general register		*/
    _RType   r11;		/* 58: general register		*/
    _RType   r12;		/* 60: general register		*/
    _RType   r13;		/* 68: general register		*/
    _RType   r14;		/* 70: general register		*/
    _RType   r15;		/* 78: general register		*/
    INSTR   *pc;		/* 80: program counter		*/
    _RType   flags;		/* 88: status register		*/
    _RType   cr0;		/* 90: control register 0	*/
    _RType   cr2;		/* 98: control register 2	*/
    _RType   cr3;		/* 100: control register 3	*/
    _RType   cr4;		/* 108: control register 4	*/
    uint64_t tsp;		/* 110: transition stack pointer*/
    uint16_t cs;		/* 118: code segment		*/
    uint16_t ds;		/* 11A: data segment		*/
    uint16_t ss;		/* 11C: stack segment		*/
    uint16_t es;		/* 11E: E segment		*/
    uint16_t fs;		/* 120: F segment		*/
    uint16_t gs;		/* 122: G segment		*/

    /* xxx(gws): excluding FP support */
    } HREG_SET;
#else
typedef struct			/* REG_SET - x86 register set	*/
    {
    uint32_t  edi;		/* 00: general register		*/
    uint32_t  esi;		/* 04: general register		*/
    uint32_t  ebp;		/* 08: frame pointer register	*/
    uint32_t  esp;		/* 0C: stack pointer register	*/
    uint32_t  ebx;		/* 10: general register		*/
    uint32_t  edx;		/* 14: general register		*/
    uint32_t  ecx;		/* 18: general register		*/
    uint32_t  eax;		/* 1C: general register		*/
    uint32_t  flags;		/* 20: status register		*/
    INSTR    *pc;		/* 24: program counter		*/
    uint32_t  cr0;		/* 28: control register 0	*/
    uint32_t  cr2;		/* 2C: control register 2	*/
    uint32_t  cr3;		/* 30: control register 3	*/
    uint32_t  cr4;		/* 34: control register 4	*/
    uint32_t  cs;		/* 28: code segment		*/
    uint32_t  ds;		/* 3C: data segment		*/
    uint32_t  ss;		/* 40: stack segment		*/
    uint32_t  es;		/* 44: E segment		*/
    uint32_t  fs;		/* 48: F segment		*/
    uint32_t  gs;		/* 4C: G segment		*/
    uint32_t  tsp;		/* 50: transition stack pointer	*/
    } HREG_SET;
#endif

typedef struct cpuid		/* CPUID - 80x86 cpuid version/feature */
    {
    uint32_t highestValue;	/* EAX=0: highest integer value */
    uint32_t vendorId[3];	/* EAX=0: vendor identification string */
    uint32_t signature;		/* EAX=1: processor signature */
    uint32_t featuresEbx;	/* EAX=1: feature flags EBX */
    uint32_t featuresEcx;	/* EAX=1: feature flags ECX */
    uint32_t featuresEdx;	/* EAX=1: feature flags EDX */
    uint32_t cacheEax;		/* EAX=2: config parameters EAX */
    uint32_t cacheEbx;		/* EAX=2: config parameters EBX */
    uint32_t cacheEcx;		/* EAX=2: config parameters ECX */
    uint32_t cacheEdx;		/* EAX=2: config parameters EDX */
    uint32_t serialNo64[2];	/* EAX=3: lower 64 of 96 bit serial no */
    uint32_t brandString[12];	/* EAX=0x8000000[234]: brand strings */
    } CPUID;


/* CPUID fields in the EAX register when EAX=1 */

typedef union
    {
    struct
	{
	uint32_t stepid:4;	/* processor stepping id mask	*/
	uint32_t model:4;	/* processor model mask		*/
	uint32_t family:4;	/* processor family mask	*/
	uint32_t type:2;	/* processor type mask		*/
	uint32_t reserved1:2;
	uint32_t modelExt:4;	/* processor extended model mask */
	uint32_t familyExt:8;	/* processor extended family mask */
	uint32_t reserved2:4;
	} field;
    uint32_t value;
    } CPUID_VERSION;

#define CPUID_TYPE_ORIG		 0	/* type: original OEM		*/
#define CPUID_TYPE_OVERD	 1	/* type: overdrive		*/
#define CPUID_TYPE_DUAL		 2	/* type: dual			*/

#define CPUID_FAMILY_486	 4	/* family: 486			*/
#define CPUID_FAMILY_PENTIUM	 5	/* family: Pentium		*/
#define CPUID_FAMILY_PENTIUMPRO  6	/* family: Pentium PRO		*/
#define CPUID_FAMILY_EXTENDED	15	/* family: Extended		*/
#define CPUID_FAMILY_PENTIUM4	 0	/* extended family: PENTIUM4	*/

/* Intel family models (Table B-1 in System Programming Volume 3B) */

#define CPUID_MODEL_PPRO	 1	/* model: Pentium Pro		*/
#define CPUID_MODEL_P2_OVER	 2	/* model: Pentium II Overdrive	*/
#define CPUID_MODEL_P2_03	 3	/* model: Pentium II, model 03	*/
#define CPUID_MODEL_P2_05	 5	/* model: Pentium II, model 05	*/
#define CPUID_MODEL_CELERON_06	 6	/* model: Celeron, model 06	*/
#define CPUID_MODEL_P3_07	 7	/* model: Pentium III, model 07	*/
#define CPUID_MODEL_P3_08	 8	/* model: Pentium III, model 08	*/
#define CPUID_MODEL_PENTIUM_M	 9	/* model: Pentium M		*/
#define CPUID_MODEL_P3_XEON	10	/* model: Pentium III Xeon	*/
#define CPUID_MODEL_P3_0B	11	/* model: Pentium III, model 0B	*/
#define CPUID_MODEL_PENTIUMM_0D	13	/* model: Pentium M, model 0D	*/
#define CPUID_MODEL_CORE	14	/* model: Core Duo/Solo		*/
#define CPUID_MODEL_CORE2	15	/* model: Core2 Duo/Quad/Xeon	*/
#define CPUID_MODEL_CORE2_E	23	/* model: Core2 Extreme		*/
#define CPUID_MODEL_I7		26	/* model: Core i7		*/
#define CPUID_MODEL_ATOM	28	/* model: Atom			*/
#define CPUID_MODEL_NEHALEM1	30	/* model: Core Architecture	*/
#define CPUID_MODEL_NEHALEM2	31	/* model: Core Architecture	*/
#define CPUID_MODEL_NEHALEM3	46	/* model: Core Architecture	*/
#define CPUID_MODEL_WESTMERE1	37	/* model: next generation	*/
#define CPUID_MODEL_WESTMERE2	44	/* model: next generation	*/
#define CPUID_MODEL_SANDYBRIDGE_XEON 45 /* model: Sandy Bridge, Xeon	*/


/* CPUID fields in the EBX register when EAX=1 */

typedef union
    {
    struct
	{
	uint32_t brand:8;	/* Brand index			*/
	uint32_t flushSize:8;	/* CLFLUSH line size		*/
	uint32_t nproc:8;	/* number of local processors	*/
	uint32_t apicId:8;	/* local APIC id		*/
	} field;
    uint32_t value;
    } CPUID_INFO;

/* CPUID fields in the EDX register when EAX=1 */

/* CPUID: feature bit definitions */

typedef union
    {
    struct
	{
	uint32_t fpu:1;		/* FPU on chip			*/
	uint32_t vme:1;		/* virtual 8086 mode enhancement*/
	uint32_t de:1;		/* debugging extensions		*/
	uint32_t pse:1;		/* page size extension		*/
	uint32_t tsc:1;		/* time stamp counter		*/
	uint32_t msr:1;		/* RDMSR and WRMSR support	*/
	uint32_t pae:1;		/* physical address extensions	*/
	uint32_t mce:1;		/* machine check exception	*/
	uint32_t cx8:1;		/* CMPXCHG8 inst		*/
	uint32_t apic:1;	/* APIC on chip			*/
	uint32_t reserved1:1;
	uint32_t sep:1;		/* SEP, Fast System Call	*/
	uint32_t mtrr:1;	/* MTRR				*/
	uint32_t pge:1;		/* PTE global bit		*/
	uint32_t mca:1;		/* machine check arch.		*/
	uint32_t cmov:1;	/* cond. move/cmp. inst		*/
	uint32_t pat:1;		/* page attribute table		*/
	uint32_t pse36:1;	/* 36 bit page size extension	*/
	uint32_t psnum:1;	/* processor serial number	*/
	uint32_t clflush:1;	/* CLFLUSH inst supported	*/
	uint32_t reserved2:1;
	uint32_t dts:1;		/* Debug Store			*/
	uint32_t acpi:1;	/* TM and SCC supported		*/
	uint32_t mmx:1;		/* MMX technology supported	*/
	uint32_t fxsr:1;	/* fast FP save and restore	*/
	uint32_t sse:1;		/* SSE supported		*/
	uint32_t sse2:1;	/* SSE2 supported		*/
	uint32_t ss:1;		/* Self Snoop supported		*/
	uint32_t htt:1;		/* Hyper Threading Technology   */
	uint32_t tm:1;		/* Thermal Monitor supported	*/
	uint32_t ia64:1;	/* IA64 Capabilities		*/
	uint32_t pbe:1;		/* Pend break enable		*/
	} field;
    uint32_t value;
    } CPUID_FEATURES;


/* fields in the ECX register when EAX=1 */

/* CPUID: extended feature bit definitions */

typedef union
    {
    struct
	{
	uint32_t sse3:1;	/* SSE3 Extensions		*/
	uint32_t reserved1:1;
	uint32_t dtes64:1;	/* 64-bit Debug Store		*/
	uint32_t mon:1;		/* Monitor/wait			*/
	uint32_t ds_cpl:1;	/* CPL qualified Debug Store	*/
	uint32_t vmx:1;		/* Virtual Machine Technology	*/
	uint32_t smx:1;		/* Safer Mode Extensions	*/
	uint32_t est:1;		/* Enhanced Speedstep Technology*/
	uint32_t tm2:1;		/* Thermal Monitor 2 supported	*/
	uint32_t ssse3:1;	/* SSSE3 Extensions		*/
	uint32_t cid:1;		/* L1 context ID		*/
	uint32_t reserved2:2;
	uint32_t cx16:1;	/* CMPXCHG16B			*/
	uint32_t xtpr:1;	/* Update control		*/
	uint32_t pdcm:1;	/* Performance/Debug capability	*/
	uint32_t reserved3:2;
	uint32_t dca:1;		/* Direct Cache Access		*/
	uint32_t sse41:1;	/* SIMD Extensions 4.1		*/
	uint32_t sse42:1;	/* SIMD Extensions 4.2		*/
	uint32_t x2apic:1;	/* x2APIC supported		*/
	uint32_t movbe:1;	/* MOVBE instruction supported	*/
	uint32_t popcnt:1;	/* POPCNT instruction supported	*/
	uint32_t reserved4:1;
	uint32_t aes:1;		/* AES instruction supported	*/
	uint32_t xsave:1;	/* XSAVE states supported	*/
	uint32_t osxsave:1;	/* extended state management	*/
	uint32_t reserved5:4;
	} field;
    uint32_t value;
    } CPUID_FEATURES_EXT;

/* fields in the EDX register when EAX=0x80000001 */
typedef union
    {
    struct
	{
	uint32_t reserved1:11;
	uint32_t syscall:1;	/* SYSCALL/SYSRET instructions */
	uint32_t reserved2:7;
	uint32_t mp:1;		/* Multi-processor capable */
	uint32_t nx:1;		/* Execute disable */
	uint32_t reserved3:1;
	uint32_t amdmmx:1;	/* AMD MMX extensions */
	uint32_t reserved4:2;
	uint32_t fxsave_opt:1;	/* FXSAVE/FXRSTOR optimizations */
	uint32_t gbpage:1;	/* Gigabyte page support */
	uint32_t rdtscp:1;	/* RDTSCP support */
	uint32_t reserved5:1;
	uint32_t lm:1;		/* Long mode em64t */
	uint32_t amd3dnow2:1;	/* AMD 3DNow! extensions */
	uint32_t amd3dnow:1;	/* AMD 3DNow! extensions */
	} field;
    uint32_t value;
    } CPUID_80000001_EDX;

/* fields in the EAX/EBX/ECX/EDX register when EAX=4 */

/* CPUID: deterministic cache parameters definitions */

typedef union
    {
    struct
	{
	/* EAX */

	uint32_t type:5;	/* Cache type			*/
	uint32_t level:3;	/* Cache level			*/
	uint32_t self_init:1;	/* Self initialising cache	*/
	uint32_t associative:1;	/* Fully associate cache	*/
	uint32_t reserved1:4;
	uint32_t threads:12;	/* Max threads sharing cache	*/
	uint32_t proc:6;	/* Max processor cores per pkg	*/

	/* EBX */

	uint32_t cline_size:12;	/* Coherency line size		*/
	uint32_t pline_size:10;	/* Physical line size		*/
	uint32_t way:10;	/* Ways of associativity	*/

	/* ECX */

	uint32_t sets:32;	/* Number of sets		*/

	/* EDX */

	uint32_t stride:10;	/* Prefetch stride		*/
	uint32_t reserved2:22;

	} field;
    struct
	{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	} value;
    } CPUID_CACHE;

/* CPUID cache types */

#define CPUID_CACHE_TYPE_NULL		0
#define CPUID_CACHE_TYPE_DATA		1
#define CPUID_CACHE_TYPE_INSTR		2
#define CPUID_CACHE_TYPE_UNIFIED	3

/* fields in the EAX/EBX/ECX/EDX register when EAX=11 */

/* CPUID: x2APIC features / processor topology parameter definitions */

typedef union
    {
    struct
	{
	/* EAX */

	uint32_t apic_r_shift:5; /* number of bits to shift right */
	uint32_t reserved1:27;

	/* EBX */

	uint32_t logical_proc:16; /* number of logical processor  */
	uint32_t reserved2:16;

	/* ECX */

	uint32_t level:8;	 /* level number		   */
	uint32_t type:8;	 /* level type			   */
	uint32_t reserved3:16;

	/* EDX */

	uint32_t ext_apic_id;	/* extended APIC id		    */

	} field;
    struct
	{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	} value;
    } CPUID_X2APIC;

/* MSR_VMX_EPT_VPID_CAP: EPT and VPID Capabilities MSR */

typedef union
    {
    struct
	{
	uint64_t ept_x_only:1;	   /* 0:     EPT execute only supported      */
	uint64_t reserved1:5;	   /* 1-5:                                   */
	uint64_t ept_pw4:1;	   /* 6:     EPT page walk length of 4       */
	uint64_t reserved2:1;	   /* 7:                                     */
	uint64_t ept_uc:1;	   /* 8:     EPT uncacheable supported       */
	uint64_t reserved3:5;	   /* 9-13:                                  */
	uint64_t ept_wb:1;	   /* 14:    EPT write-back supported        */
	uint64_t reserved4:1;	   /* 15:                                    */
	uint64_t ept_2mb:1;	   /* 16:    EPT 2MB pages supported         */
	uint64_t ept_1gb:1;	   /* 17:    EPT 1GB pages supported         */
	uint64_t reserved5:2;	   /* 18-19:                                 */
	uint64_t invept:1;	   /* 20:    INVEPT supported                */
	uint64_t reserved6:4;	   /* 21-24:                                 */
	uint64_t invept_single:1;  /* 25:    single-context INVEPT supported */
	uint64_t invept_all:1;	   /* 26:    all-context INVEPT supported    */
	uint64_t reserved7:5;	   /* 27-31:                                 */
	uint64_t invvpid:1;	   /* 32:    INVVPID supported               */
	uint64_t reserved8:7;	   /* 33-39:                                 */
	uint64_t invvpid_ind:1;	   /* 40:    individual-context INVVPID supp */
	uint64_t invvpid_single:1; /* 41:    single-context INVVPID supported*/
	uint64_t invvpid_all:1;	   /* 42:    all-context INVVPID supported   */
	uint64_t invvpid_global:1; /* 43:    keep-global INVVPID supported   */
	uint64_t reserved9:20;	   /* 44-63				     */
	} field;
    uint64_t value;
    } EPT_VPID_CAP_MSR;

typedef struct mtrr_fix		/* MTRR - fixed range register */
    {
    char type[8];
    } MTRR_FIX;

typedef struct mtrr_var		/* MTRR - variable range register */
    {
    long long int base;
    long long int mask;
    } MTRR_VAR;

typedef struct mtrr		/* MTRR */
    {
    uint32_t cap[2];		/* MTRR cap register */
    uint32_t deftype[2];	/* MTRR defType register */
    MTRR_FIX fix[11];		/* MTRR fixed range registers */
    MTRR_VAR var[8];		/* MTRR variable range registers */
    } MTRR;

typedef MTRR *		MTRR_ID;

/* PLATFORM_INFO MSR fields */

typedef union
    {
    struct
	{
	uint64_t reserved1:8;
	uint64_t ratio_max:8;	/* maximum non-turbo TSC ratio	*/
	uint64_t reserved2:24;
	uint64_t ratio_min:8;	/* minimum non-turbo TSC ratio	*/
	uint64_t reserved3:16;
	} field;
    uint64_t value;
    } PLATFORM_INFO;

/* MSR_FLEX_RATIO MSR fields */

typedef union
    {
    struct
	{
	uint64_t reserved1:8;
	uint64_t ratio:8;	/* current non-turbo TSC ratio	*/
	uint64_t enabled:1;	/* flex ratio enabled		*/
	uint64_t reserved2:47;
	} field;
    uint64_t value;
    } FLEX_RATIO;

/*
 * Structure of a global descriptor table (GDT) entry
 * (common to 32 & 64 bits)
 */

typedef struct gdt_entry
    {
    uint16_t	limit00;	/* limit: xffff			*/
    uint16_t	base00;		/* base : xxxx0000		*/
    uint8_t	base01;		/* base : xx00xxxx		*/
    uint8_t	type:4;		/* Code e/r,			*/
    uint8_t	sys:1;		/* 0 = system segment		*/
    uint8_t	dpl:2;		/* descriptor priv level	*/
    uint8_t	p:1;		/* present			*/
    uint8_t	limit01:4;	/* limit: fxxxx			*/
    uint8_t	free:1;		/* free bit			*/
    uint8_t	l:1;		/* 64-bit code segment		*/
    uint8_t	d_b:1;		/* access 0=16-bit 1=32-bit	*/
    uint8_t	g:1;		/* page granularity		*/
    uint8_t	base02;		/* base : 00xxxxxx		*/
    } GDT_ENTRY;

/* Structure of global descriptor table call entry in 32-bits mode */

typedef struct gdt32_call_entry
    {
    uint16_t	offset00;	/* offset: xxxxffff		*/
    uint16_t	segment;	/* segment selector		*/
    uint8_t	nparam:5;	/* number of parameters		*/
    uint8_t	res1:3;		/* reserved (zeroes)		*/
    uint8_t	type:4;		/* Code e/r,			*/
    uint8_t	sys:1;		/* 0 = system segment		*/
    uint8_t	dpl:2;		/* descriptor priv level	*/
    uint8_t	p:1;		/* present			*/
    uint16_t	offset01;	/* offset: ffffxxxx		*/
    } GDT32_CALL_ENTRY;

/* Structure of interrupt descriptor table (IDT) entry in 32-bits mode */

typedef struct idt32_entry
    {
    uint16_t	offset00;	/* offset : xxxxffff		*/
    uint16_t	segment;	/* segment selector		*/
    uint8_t	res1;		/* reserved (zeroes) */
    uint8_t	type:5;		/* Code e/r,			*/
    uint8_t	dpl:2;		/* descriptor priv level	*/
    uint8_t	p:1;		/* present			*/
    uint16_t	offset01;	/* offset: ffffxxxx		*/
    } IDT32_ENTRY;

#ifdef	LP64
/* Structure of global descriptor table call entry in 64-bits mode */

typedef struct gdt64_call_entry
    {
    uint16_t	offset00;	/* offset: xxxxxxxxxxxxffff	*/
    uint16_t	segment;	/* segment selector		*/
    uint8_t	res1;		/* reserved (zeroes)		*/
    uint8_t	type:4;		/* Code e/r,			*/
    uint8_t	sys:1;		/* 0 = system segment		*/
    uint8_t	dpl:2;		/* descriptor priv level	*/
    uint8_t	p:1;		/* present			*/
    uint16_t	offset01;	/* offset: xxxxxxxxffffxxxx	*/
    uint32_t	offset02;	/* offset: ffffffffxxxxxxxx	*/
    uint32_t	res3;		/* reserved */
    } GDT64_CALL_ENTRY;

/* Structure of interrupt descriptor table (IDT) entry in 64-bits mode */

typedef struct idt64_entry
    {
    uint16_t	offset00;	/* offset: xxxxxxxxxxxxffff	*/
    uint16_t	segment;	/* segment selector		*/
    uint8_t	ist:3;		/* interrupt stack table	*/
    uint8_t	res1:5;		/* reserved (zeroes)		*/
    uint8_t	type:4;		/* Code e/r,			*/
    uint8_t	res2:1;		/* reserved (zeroes)		*/
    uint8_t	dpl:2;		/* descriptor priv level	*/
    uint8_t	p:1;		/* present			*/
    uint16_t	offset01;	/* offset: xxxxxxxxffffxxxx	*/
    uint32_t	offset02;	/* offset: ffffffffxxxxxxxx	*/
    uint32_t	res3;		/* reserved */
    } IDT64_ENTRY;

#define GDT_CALL_ENTRY		GDT64_CALL_ENTRY
#define IDT_ENTRY		IDT64_ENTRY
#else
#define GDT_CALL_ENTRY		GDT32_CALL_ENTRY
#define IDT_ENTRY		IDT32_ENTRY
#endif	/* LP64 */


#define GDT_BASE00_SHIFT	00
#define GDT_BASE01_SHIFT	16
#define GDT_BASE02_SHIFT	24
#define GDT_BASE00_MASK		0x0000ffff
#define GDT_BASE01_MASK		0x00ff0000
#define GDT_BASE02_MASK		0xff000000

#define GDT_LIMIT00_SHIFT	00
#define GDT_LIMIT01_SHIFT	16
#define GDT_LIMIT00_MASK	0x0ffff
#define GDT_LIMIT01_MASK	0xf0000

#define GDT_BASE(p)	((p->base02 << GDT_BASE02_SHIFT) | \
			 (p->base01 << GDT_BASE01_SHIFT) | p->base00)
#define GDT_LIMIT(p)	((p->limit01 << GDT_LIMIT01_SHIFT) | p->limit00)

#define GDT_LIMIT00(v)	(((v) & GDT_LIMIT00_MASK) >> GDT_LIMIT00_SHIFT)
#define GDT_LIMIT01(v)	(((v) & GDT_LIMIT01_MASK) >> GDT_LIMIT01_SHIFT)

#define GDT_BASE00(v)	(((v) & GDT_BASE00_MASK) >> GDT_BASE00_SHIFT)
#define GDT_BASE01(v)	(((v) & GDT_BASE01_MASK) >> GDT_BASE01_SHIFT)
#define GDT_BASE02(v)	(((v) & GDT_BASE02_MASK) >> GDT_BASE02_SHIFT)

/* GDT call entry macros */

#define GDT32_OFFSET00_SHIFT	00
#define GDT32_OFFSET01_SHIFT	16
#define GDT32_OFFSET00_MASK	0x0000ffff
#define GDT32_OFFSET01_MASK	0xffff0000

#define GDT32_OFFSET(p)		((p->offset01 << GDT32_OFFSET01_SHIFT) | \
				 p->offset00)
#define GDT32_OFFSET00(v)	(((v) & GDT32_OFFSET00_MASK) >> \
				 GDT32_OFFSET00_SHIFT)
#define GDT32_OFFSET01(v)	(((v) & GDT32_OFFSET01_MASK) >> \
				 GDT32_OFFSET01_SHIFT)

#ifdef	LP64
#define GDT64_OFFSET00_SHIFT	00
#define GDT64_OFFSET01_SHIFT	16
#define GDT64_OFFSET02_SHIFT	32
#define GDT64_OFFSET00_MASK	0x000000000000ffff
#define GDT64_OFFSET01_MASK	0x00000000ffff0000

#define GDT64_OFFSET(p)		((((uint64_t) p->offset02) << \
					GDT64_OFFSET02_SHIFT) | \
				 (p->offset01 << GDT64_OFFSET01_SHIFT) | \
				 p->offset00)
#define GDT64_OFFSET00(v)	((uint16_t)((((v) & GDT64_OFFSET00_MASK) >> \
					    GDT64_OFFSET00_SHIFT)))
#define GDT64_OFFSET01(v)	((uint16_t)((((v) & GDT64_OFFSET01_MASK) >> \
					    GDT64_OFFSET01_SHIFT)))
#define GDT64_OFFSET02(v)	((uint32_t)(((v) >> GDT64_OFFSET02_SHIFT)))

#define GDT_OFFSET		GDT64_OFFSET
#define GDT_OFFSET00		GDT64_OFFSET00
#define GDT_OFFSET01		GDT64_OFFSET01
#define GDT_OFFSET02		GDT64_OFFSET02
#else	/* LP64 */
#define GDT_OFFSET		GDT32_OFFSET
#define GDT_OFFSET00		GDT32_OFFSET00
#define GDT_OFFSET01		GDT32_OFFSET01
#endif

/* IDT aliases */

#define IDT32_OFFSET(x)		GDT32_OFFSET(x)
#define IDT32_OFFSET00(x)	GDT32_OFFSET00(x)
#define IDT32_OFFSET01(x)	GDT32_OFFSET01(x)

#ifdef	LP64
#define IDT64_OFFSET(x)		GDT64_OFFSET(x)
#define IDT64_OFFSET00(x)	GDT64_OFFSET00(x)
#define IDT64_OFFSET01(x)	GDT64_OFFSET01(x)
#define IDT64_OFFSET02(x)	GDT64_OFFSET02(x)

#define IDT_OFFSET		IDT64_OFFSET
#define IDT_OFFSET00		IDT64_OFFSET00
#define IDT_OFFSET01		IDT64_OFFSET01
#define IDT_OFFSET02		IDT64_OFFSET02
#else	/* LP64 */
#define IDT_OFFSET		IDT32_OFFSET
#define IDT_OFFSET00		IDT32_OFFSET00
#define IDT_OFFSET01		IDT32_OFFSET01
#define IDT_OFFSET02		IDT32_OFFSET02
#endif	/* LP64 */

/* GDT data segment types */

#define GDT_TYPE_RO		0	/* read-only			   */
#define GDT_TYPE_RO_A		1	/* read-only accessed		   */
#define GDT_TYPE_RW		2	/* read-write			   */
#define GDT_TYPE_RW_A		3	/* read-write accessed		   */
#define GDT_TYPE_RO_DOWN	4	/* read-only expand-down	   */
#define GDT_TYPE_RO_DOWN_A	5	/* read-only expand-down accessed  */
#define GDT_TYPE_RW_DOWN	6	/* read-write expand-down	   */
#define GDT_TYPE_RW_DOWN_A	7	/* read-write expand-down accessed */
#define GDT_TYPE_EX		8	/* execute			   */
#define GDT_TYPE_EX_A		9	/* execute accessed		   */
#define GDT_TYPE_EX_RO		10	/* execute read-only		   */
#define GDT_TYPE_EX_RO_A	11	/* execute read-only accessed	   */
#define GDT_TYPE_EX_CF		12	/* execute conforming		   */
#define GDT_TYPE_EX_CF_A	13	/* execute conforming accessed	   */
#define GDT_TYPE_EX_RO_CF	14	/* execute read-only conforming	   */
#define GDT_TYPE_EX_RO_CF_A	15	/* execute r/o conforming accessed */

/* GDT system segment types */

#ifndef LP64
#define GDT_TYPE_SYS_RES0	0	/* reserved			   */
#define GDT_TYPE_SYS_TSS16_A	1	/* 16-bit TSS (available)	   */
#define GDT_TYPE_SYS_LDT	2	/* local descriptor table	   */
#define GDT_TYPE_SYS_TSS16_B	3	/* 16-bit TSS (busy)		   */
#define GDT_TYPE_SYS_CALL16	4	/* 16-bit call gate		   */
#define GDT_TYPE_SYS_TGATE	5	/* task gate			   */
#define GDT_TYPE_SYS_IGATE16	6	/* 16-bit interrupt gate	   */
#define GDT_TYPE_SYS_TGATE16	7	/* 16-bit trap gate		   */
#define GDT_TYPE_SYS_RES1	8	/* reserved			   */
#define GDT_TYPE_SYS_TSS32_A	9	/* 32-bit TSS (available)	   */
#define GDT_TYPE_SYS_RES2	10	/* reserved			   */
#define GDT_TYPE_SYS_TSS32_B	11	/* 32-bit TSS (busy)		   */
#define GDT_TYPE_SYS_CALL32	12	/* 32-bit call gate		   */
#define GDT_TYPE_SYS_RES3	13	/* reserved			   */
#define GDT_TYPE_SYS_IGATE32	14	/* 32-bit interrupt gate	   */
#define GDT_TYPE_SYS_TGATE32	15	/* 32-bit trap gate		   */
#else	/* !LP64 */
#define GDT32_TYPE_SYS_TSS16_A	1	/* 16-bit TSS (available)	   */
#define GDT32_TYPE_SYS_LDT	2	/* local descriptor table	   */
#define GDT32_TYPE_SYS_TSS16_B	3	/* 16-bit TSS (busy)		   */
#define GDT32_TYPE_SYS_CALL16	4	/* 16-bit call gate		   */
#define GDT32_TYPE_SYS_TGATE	5	/* task gate			   */
#define GDT32_TYPE_SYS_IGATE16	6	/* 16-bit interrupt gate	   */
#define GDT32_TYPE_SYS_TGATE16	7	/* 16-bit trap gate		   */
#define GDT32_TYPE_SYS_TSS32_A	9	/* 32-bit TSS (available)	   */
#define GDT32_TYPE_SYS_TSS32_B	11	/* 32-bit TSS (busy)		   */
#define GDT32_TYPE_SYS_CALL32	12	/* 32-bit call gate		   */
#define GDT32_TYPE_SYS_IGATE32	14	/* 32-bit interrupt gate	   */
#define GDT32_TYPE_SYS_TGATE32	15	/* 32-bit trap gate		   */

#define GDT_TYPE_SYS_LDT	2	/* local descriptor table	   */
#define GDT_TYPE_SYS_TSS64_A	9	/* 64 bit TSS (available)	   */
#define GDT_TYPE_SYS_TSS64_B	11	/* 64 bit TSS (busy)	           */
#define GDT_TYPE_SYS_CALL64	12	/* 64-bit call gate		   */
#define GDT_TYPE_SYS_IGATE64	14	/* 64-bit interrupt gate	   */
#define GDT_TYPE_SYS_TGATE63	15	/* 64-bit trap gate		   */
#endif	/* !LP64 */
/* GDT segment status */

#define GDT_SEG_INVALID		0	/* segment is not present	   */
#define GDT_SEG_VALID		1	/* segment is present		   */

/* GDT segment type */

#define GDT_SEG_SYSTEM		0	/* system type segment		   */
#define GDT_SEG_CODE_DATA	1	/* code/data type segment	   */

/* GDT access types */

#define GDT_ACCESS_16		0	/* 16-bit access to memory	   */
#define GDT_ACCESS_32		1	/* 32-bit access to memory	   */

/* GDT access types */

#define GDT_GRAN_BYTE		0	/* byte size page granularity	   */
#define GDT_GRAN_4KB_PAGE	1	/* 4KB size page granularity	   */

/* GDT access types */

#define GDT_64_CODE_SEGMENT	1	/* 64-bit code segment		   */
#define GDT_COMP_CODE_SEGMENT	0	/* compatible mode code segment	   */

/* GDT privilidge level */

#define GDT_PRIV_RING_0		0	/* ring 0 priv level		   */
#define GDT_PRIV_RING_1		1	/* ring 1 priv level		   */
#define GDT_PRIV_RING_2		2	/* ring 2 priv level		   */
#define GDT_PRIV_RING_3		3	/* ring 3 priv level		   */

/* IDT segment types */

#define IDT_TYPE_RES0		0	/* reserved			   */
#define IDT_TYPE_RES1		1	/* reserved			   */
#define IDT_TYPE_RES2		2	/* reserved			   */
#define IDT_TYPE_RES3		3	/* reserved			   */
#define IDT_TYPE_RES4		4	/* reserved			   */
#define IDT_TYPE_TGATE		5	/* task gate			   */
#define IDT_TYPE_IGATE_16	6	/* 16-bit interrupt gate	   */
#define IDT_TYPE_TRGATE_16	7	/* 16-bit trap gate		   */
#define IDT_TYPE_RES8		8	/* reserved			   */
#define IDT_TYPE_RES9		9	/* reserved			   */
#define IDT_TYPE_RES10		10	/* reserved			   */
#define IDT_TYPE_RES11		11	/* reserved			   */
#define IDT_TYPE_RES12		12	/* reserved			   */
#define IDT_TYPE_RES13		13	/* reserved			   */
#define IDT_TYPE_IGATE		14	/* 32-bit interrupt gate	   */
#define IDT_TYPE_TRGATE		15	/* 32-bit trap gate		   */

/* IDT segment status */

#define IDT_SEG_INVALID		0	/* segment is not present	   */
#define IDT_SEG_VALID		1	/* segment is present		   */

/* IDT segment type */

#define IDT_SEG_SYSTEM		0	/* system type segment		   */
#define IDT_SEG_CODE_DATA	1	/* code/data type segment	   */

/* IDT access types */

#define IDT_ACCESS_16		0	/* 16-bit access to memory	   */
#define IDT_ACCESS_32		1	/* 32-bit access to memory	   */

/* IDT access types */

#define IDT_GRAN_BYTE		0	/* byte size page granularity	   */
#define IDT_GRAN_4KB_PAGE	1	/* 4KB size page granularity	   */

/* IDT privilidge level */

#define IDT_PRIV_RING_0		0	/* ring 0 priv level		   */
#define IDT_PRIV_RING_1		1	/* ring 2 priv level		   */
#define IDT_PRIV_RING_2		2	/* ring 2 priv level		   */
#define IDT_PRIV_RING_3		3	/* ring 3 priv level		   */

/* structure of the system descriptor table registers (GDTR, IDTR, LDTR) */

#if(TOOL == msc)
#pragma pack(push,1)
#endif

struct XDTR
{
	uint16_t limit;		/* maximum size of the DT */
	size_t   base;		/* address of DT */
	uint16_t pad;
}
#ifndef _MSC_TOOL
__attribute__((packed));
#endif
;



struct XDTR32
{
	uint16_t limit;
	uint32_t base;
	uint16_t pad;
} 
#ifndef _MSC_TOOL
__attribute__((packed))
#endif
;
#if(TOOL == msc)
#pragma pack(pop)
#endif

typedef struct XDTR GDTR;
typedef struct XDTR IDTR;
typedef struct XDTR LDTR;

typedef struct XDTR32 GDTR32;
typedef struct XDTR32 IDTR32;
typedef struct XDTR32 LDTR32;

#define	XDTR_ADDR(reg)	(reg.base)
#define	XDTR_LIMIT(reg)	(reg.limit)

#define GDTR_ADDR(reg)	XDTR_ADDR(reg)
#define GDTR_LIMIT(reg)	XDTR_LIMIT(reg)
#define IDTR_ADDR(reg)	XDTR_ADDR(reg)
#define IDTR_LIMIT(reg)	XDTR_LIMIT(reg)
#define LDTR_ADDR(reg)	XDTR_ADDR(reg)
#define LDTR_LIMIT(reg)	XDTR_LIMIT(reg)

/* x86 Task State Segment (TSS) */

typedef struct tss
    {
#ifdef LP64
    uint32_t  reserved0;	/* reserved */
    uint64_t  rsp0;		/* privilege level 0 SP */
    uint64_t  rsp1;		/* privilege level 1 SP */
    uint64_t  rsp2;		/* privilege level 2 SP */
    uint64_t  reserved1;	/* reserved */
    uint64_t  ist1;		/* interrupt stack table ptr */
    uint64_t  ist2;		/* interrupt stack table ptr */
    uint64_t  ist3;		/* interrupt stack table ptr */
    uint64_t  ist4;		/* interrupt stack table ptr */
    uint64_t  ist5;		/* interrupt stack table ptr */
    uint64_t  ist6;		/* interrupt stack table ptr */
    uint64_t  ist7;		/* interrupt stack table ptr */
    uint64_t  reserved2;	/* reserved */
    uint16_t  reserved3;	/* reserved */
    uint16_t  iomapb;		/* IO map base offset */
    uint32_t  iobmap[IO_BMAP_SIZE + 1];
#else
    uint16_t  link;		/* link to previous task */
    uint16_t  reserved0;	/* reserved */
    uint32_t  esp0;		/* privilege level 0 SP */
    uint16_t  ss0;		/*   ''              SS */
    uint16_t  reserved1;	/* reserved */
    uint32_t  esp1;		/* privilege level 1 SP */
    uint16_t  ss1;		/*   ''              SS */
    uint16_t  reserved3;	/* reserved */
    uint32_t  esp2;		/* privilege level 2 SP */
    uint16_t  ss2;		/*   ''              SS */
    uint16_t  reserved4;	/* reserved */
    uint32_t  cr3;		/* control register CR3 */
    INSTR    *eip;		/* program counter  EIP */
    uint32_t  eflags;		/* status register  EFLAGS */
    uint32_t  eax;		/* general register EAX */
    uint32_t  ecx;		/* general register ECX */
    uint32_t  edx;		/* general register EDX */
    uint32_t  ebx;		/* general register EBX */
    uint32_t  esp;		/* stack pointer register ESP */
    uint32_t  ebp;		/* frame pointer register EBP */
    uint32_t  esi;		/* general register ESI */
    uint32_t  edi;		/* general register EDI */
    uint16_t  es;		/* segment selector ES */
    uint16_t  reserved5;	/* reserved */
    uint16_t  cs;		/* segment selector CS */
    uint16_t  reserved6;	/* reserved */
    uint16_t  ss;		/* segment selector SS */
    uint16_t  reserved7;	/* reserved */
    uint16_t  ds;		/* segment selector DS */
    uint16_t  reserved8;	/* reserved */
    uint16_t  fs;		/* segment selector FS */
    uint16_t  reserved9;	/* reserved */
    uint16_t  gs;		/* segment selector GS */
    uint16_t  reserved10;	/* reserved */
    uint16_t  ldt;		/* segment selector LDT */
    uint16_t  reserved11;	/* reserved */
    uint16_t  tflag;		/* debug trap flag T */
    uint16_t  iomapb;		/* IO map base address */
    uint32_t  iobmap[IO_BMAP_SIZE + 1];
    uint32_t  reserved12;	/* TSS selector */
    uint32_t  reserved13;
    uint32_t  reserved14;
    uint32_t  reserved15;
    uint32_t  reserved16;
    uint32_t  reserved17;
    uint32_t  reserved18;
    uint32_t  reserved19;
#endif
    } TSS;

typedef struct segdesc		/* segment descriptor */
    {
    uint16_t	limitLW;	/* limit 15:00			*/
    uint16_t	baseLW;		/* base address 15:00		*/
    uint8_t	baseMB;		/* base address 23:16		*/
    uint8_t	type;		/* P, DPL, S, Type		*/
    uint8_t	limitUB;	/* G, DB, 0, AVL, limit 23:16	*/
    uint8_t	baseUB;		/* base address 31:24		*/
    } SEGDESC;

typedef struct taskGate		/* task gate */
    {
    uint16_t    reserved1;	/* reserved1 15:00		*/
    uint16_t    segment;	/* TSS segment selector 31:16	*/
    uint16_t	reserved2:8;	/* reserved2 07:00		*/
    uint8_t	type:5;		/* Type 12:08			*/
    uint8_t	dpl:2;		/* DPL 14:13			*/
    uint8_t	p:1;		/* present 15:15		*/
    uint16_t    reserved3;      /* reserved3 31:16		*/
#ifdef LP64
    uint32_t	padding1;	/* padding to align to IDT_ENTRY*/
    uint32_t	padding2;
#endif
    } TGATE_DESC;

typedef struct callGate		/* call gate */
    {
    uint16_t	offsetLo;
    uint16_t	selector;
    uint8_t	params;
    uint8_t	type;
    uint16_t	offsetHi;
   } CGATE_DESC;

typedef struct intGate		/* interrupt gate */
    {
    uint16_t    offsetLo;	/* offset 15:00			*/
    uint16_t    segment;	/* segment selector 31:16	*/
    uint8_t	reserved:5;	/* reserved2 04:00		*/
    uint8_t	zero:3;		/* zeros 07:05			*/
    uint8_t	type:5;		/* type 12:08			*/
    uint8_t	dpl:2;		/* DPL 14:13			*/
    uint8_t	p:1;		/* present 15:15		*/
    uint16_t    offsetHi;	/* offset 31:16			*/
#ifdef LP64
    uint32_t	padding1;	/* padding to align to IDT_ENTRY*/
    uint32_t	padding2;
#endif
    } IGATE_DESC;

#define IGATE_OFFSET00_SHIFT	00
#define IGATE_OFFSET01_SHIFT	16
#define IGATE_OFFSET(p)	((p->offsetHi << IGATE_OFFSET01_SHIFT) | p->offsetLo)


typedef struct trapGate		/* trap gate */
    {
    uint16_t    offsetLo;	/* offset 15:00			*/
    uint16_t    segment;	/* segment selector 31:16	*/
    uint8_t	reserved:5;	/* reserved2 04:00		*/
    uint8_t	zero:3;		/* zeros 07:05			*/
    uint8_t	type:5;		/* type 12:08			*/
    uint8_t	dpl:2;		/* DPL 14:13			*/
    uint8_t	p:1;		/* present 15:15		*/
    uint16_t    offsetHi;	/* offset 31:16			*/
#ifdef LP64
    uint32_t	padding1;	/* padding to align to IDT_ENTRY*/
    uint32_t	padding2;
#endif
    } TRGATE_DESC;

#define TRGATE_OFFSET00_SHIFT	00
#define TRGATE_OFFSET01_SHIFT	16
#define TRGATE_OFFSET(p) ((p->offsetHi << TRGATE_OFFSET01_SHIFT) | p->offsetLo)

/* convenience structure to access 32-bit fields of a 64-bit MSR */

typedef union
    {
    struct
	{
	uint32_t low;		/* low 32-bits of value */
	uint32_t high;		/* high 32-bits of value */
	} field;
    uint64_t word;		/* 64-bit MSR value */
    } MSR_VALUE;

/* some common names for registers */

#define fpReg		ebp	/* frame pointer */
#define spReg		esp	/* stack pointer */
#define reg_pc		pc	/* program counter */
#define reg_sp		spReg	/* stack pointer */
#define reg_fp		fpReg	/* frame pointer */

#define  G_REG_BASE	0x00	/* data reg's base offset to HREG_SET */
#define  G_REG_OFFSET(n)	(G_REG_BASE + (n)*sizeof(uint32_t))
#define  SR_OFFSET		G_REG_OFFSET(GREG_NUM)
#define  PC_OFFSET		(SR_OFFSET + sizeof(uint32_t))

/* register access functions */

extern _RType	x86CR0Get (void);
extern void	x86CR0Set (_RType value);
extern _RType	x86CR2Get (void);
extern void	x86CR2Set (_RType value);
extern _RType	x86CR3Get (void);
extern void	x86CR3Set (_RType value);
extern _RType	x86CR4Get (void);
extern void	x86CR4Set (_RType value);
extern void	x86GdtrGet (GDTR *value);
extern void	x86GdtrSet (GDTR *value);
extern void	x86IdtrGet (IDTR *value);
extern void	x86IdtrSet (IDTR *value);
extern void	x86LdtrGet (LDTR *value);
extern void	x86LdtrSet (LDTR *value);
extern _RType	x86TrGet (void);
extern void	x86TrSet (_RType value);
extern void	x86MsrGet (uint32_t addr, uint64_t *value);
extern void	x86MsrSet (uint32_t addr, uint64_t *value);
extern _RType	x86FlagsGet (void);
extern _RType	x86CsGet (void);
extern _RType	x86SsGet (void);
extern _RType	x86DsGet (void);
extern _RType	x86EsGet (void);
extern _RType	x86FsGet (void);
extern _RType	x86GsGet (void);
extern void	x86TscGet (uint64_t *value);
extern void	x86GetCpuid (uint32_t operation,
			     uint32_t *eaxValue, uint32_t *ebxValue,
			     uint32_t *ecxValue, uint32_t *edxValue);

extern void	x87StateSave (void *);
extern void	x87StateRestore (void *);

#endif	/* _ASMLANGUAGE */

/* CPU FAMILY & FPU type */

#define X86CPU_386	  0	/* CPU FAMILY: 80386		*/
#define X86CPU_486	  1	/* CPU FAMILY: 80486		*/
#define X86CPU_PENTIUM	  2	/* CPU FAMILY: Pentium/P5	*/
#define X86CPU_NS486	  3	/* CPU FAMILY: NS486		*/
#define X86CPU_PENTIUMPRO 4	/* CPU FAMILY: Pentiumpro/P6	*/
#define X86CPU_PENTIUM4   5	/* CPU FAMILY: Pentium4/P7      */
#define X86FPU_387	  1	/* FPU: 80387			*/
#define X86FPU_487	  2	/* FPU: 80487			*/

/* offset to registers in HREG_SET */

#ifdef LP64
#define HREG_RAX		0x00
#define HREG_RBX		0x08
#define HREG_RCX		0x10
#define HREG_RDX		0x18
#define HREG_RSP		0x20
#define HREG_RBP		0x28
#define HREG_RSI		0x30
#define HREG_RDI		0x38
#define HREG_R8			0x40
#define HREG_R9			0x48
#define HREG_R10		0x50
#define HREG_R11		0x58
#define HREG_R12		0x60
#define HREG_R13		0x68
#define HREG_R14		0x70
#define HREG_R15		0x78
#define HREG_PC			0x80
#define HREG_RFLAGS		0x88
#define HREG_CR0		0x90
#define HREG_CR2		0x98
#define HREG_CR3		0xa0
#define HREG_CR4		0xa8
#define HREG_TSP		0xb0
#define HREG_CS			0xb8
#define HREG_DS			0xba
#define HREG_SS			0xbc
#define HREG_ES			0xbe
#define HREG_FS			0xc0
#define HREG_GS			0xc2
#else
#define HREG_EDI		0x00
#define HREG_ESI		0x04
#define HREG_EBP		0x08
#define HREG_ESP		0x0c
#define HREG_EBX		0x10
#define HREG_EDX		0x14
#define HREG_ECX		0x18
#define HREG_EAX		0x1c
#define HREG_EFLAGS		0x20
#define HREG_PC			0x24
#define HREG_CR0		0x28
#define HREG_CR2		0x2C
#define HREG_CR3		0x30
#define HREG_CR4		0x34
#define HREG_CS			0x38
#define HREG_DS			0x3C
#define HREG_SS			0x40
#define HREG_ES			0x44
#define HREG_FS			0x48
#define HREG_GS			0x4C
#define HREG_TSP		0x50
#endif /* LP64 */

#define NUM_CR_REG	5		/* number of CR registers */

/* bits on EFLAGS */

#define EFLAGS_EMPTY	0x00000020	/* empty eflags */
#define EFLAGS_BRANDNEW	0x00000200	/* brand new EFLAGS */
#define EFLAGS_N_MASK	0xffffbfff	/* N(nested task flag) bit mask */
#define EFLAGS_TF_MASK	0xfffffeff	/* TF(trap flag) bit mask */

#define EFLAGS_CF	0x00000001	/* CF(carry flag) bit */
#define EFLAGS_PF	0x00000004	/* PF(parity flag) bit */
#define EFLAGS_AF	0x00000010	/* AF(borrow flag) bit */
#define EFLAGS_ZF	0x00000040	/* ZF(zero flag) bit */
#define EFLAGS_SF	0x00000080	/* AF(sign flag) bit */
#define EFLAGS_TF	0x00000100	/* TF(trap flag) bit */
#define EFLAGS_IF	0x00000200	/* IF(interrupt enable flag) bit */
#define EFLAGS_IOPL	0x00003000	/* IOPL(IO privilege level) bits */
#define EFLAGS_NT	0x00004000	/* NT(nested task flag) bit */
#define EFLAGS_RF	0x00010000	/* RF(resume flag) bit */
#define EFLAGS_VM	0x00020000	/* VM(virtual 8086 mode) bit */
#define EFLAGS_AC	0x00040000	/* AC(alignment check) bit */
#define EFLAGS_VIF	0x00080000	/* VIF(virtual int flag) bit */
#define EFLAGS_VIP	0x00100000	/* VIP(virtual int pending) bit */
#define EFLAGS_ID	0x00200000	/* ID(identification flag) bit */

/* control and test registers */

#define CR0		1
#define CR1		2
#define CR2		3
#define CR3		4
#define TR3		5
#define TR4		6
#define TR5		7
#define TR6		8
#define TR7		9

/* bits on CR0 */

#define CR0_PE		0x00000001	/* protection enable */
#define CR0_MP		0x00000002	/* math present */
#define CR0_EM		0x00000004	/* emulation */
#define CR0_TS		0x00000008	/* task switch */
#define CR0_ET		0x00000010	/* extension type */
#define CR0_NE		0x00000020	/* numeric error */
#define CR0_WP		0x00010000	/* write protect */
#define CR0_AM		0x00040000	/* alignment mask */
#define CR0_NW		0x20000000	/* not write through */
#define CR0_CD		0x40000000	/* cache disable */
#define CR0_PG		0x80000000	/* paging */
#define CR0_NW_NOT	0xdfffffff	/* write through */
#define CR0_CD_NOT	0xbfffffff	/* cache disable */

/* bits on CR4 */

#define CR4_VME		0x00000001	/* virtual-8086 mode extensions */
#define CR4_PVI		0x00000002	/* protected-mode virtual interrupts */
#define CR4_TSD		0x00000004	/* timestamp disable */
#define CR4_DE		0x00000008	/* debugging extensions */
#define CR4_PSE		0x00000010	/* page size extensions */
#define CR4_PAE		0x00000020	/* physical address extension */
#define CR4_MCE		0x00000040	/* machine check enable */
#define CR4_PGE		0x00000080	/* page global enable */
#define CR4_PCE		0x00000100	/* performance-monitoring enable */
#define CR4_OSFXSR	0x00000200	/* use fxsave/fxrstor instructions */
#define CR4_OSXMMEXCEPT	0x00000400	/* streaming SIMD exception */
#define CR4_VMXE	0x00002000	/* virtual machine extensions */

/* CPUID: signature bit definitions */

#define CPUID_STEPID	0x0000000f	/* processor stepping id mask	*/
#define CPUID_MODEL	0x000000f0	/* processor model mask		*/
#define CPUID_FAMILY	0x00000f00	/* processor family mask	*/
#define CPUID_TYPE	0x00003000	/* processor type mask		*/
#define CPUID_EXT_MODEL	0x000f0000	/* processor extended model mask */
#define CPUID_EXT_FAMILY 0x0ff00000	/* processor extended family mask */
#define CPUID_486	0x00000400	/* family: 486			*/
#define CPUID_PENTIUM	0x00000500	/* family: Pentium		*/
#define CPUID_PENTIUMPRO 0x00000600	/* family: Pentium PRO		*/
#define CPUID_EXTENDED	0x00000f00	/* family: Extended		*/
#define CPUID_PENTIUM4	0x00000000	/* extended family: PENTIUM4	*/
#define CPUID_ORIG      0x00000000	/* type: original OEM		*/
#define CPUID_OVERD     0x00001000	/* type: overdrive		*/
#define CPUID_DUAL      0x00002000	/* type: dual			*/
#define CPUID_CHUNKS	0x0000ff00	/* bytes flushed by CLFLUSH mask */

/* CPUID: feature bit definitions (EDX) */

#define CPUID_FPU	0x00000001	/* FPU on chip			*/
#define CPUID_VME	0x00000002	/* virtual 8086 mode enhancement*/
#define CPUID_DE	0x00000004	/* debugging extensions		*/
#define CPUID_PSE	0x00000008	/* page size extension		*/
#define CPUID_TSC	0x00000010	/* time stamp counter		*/
#define CPUID_MSR	0x00000020	/* RDMSR and WRMSR support	*/
#define CPUID_PAE	0x00000040	/* physical address extensions	*/
#define CPUID_MCE	0x00000080	/* machine check exception	*/
#define CPUID_CXS	0x00000100	/* CMPXCHG8 inst		*/
#define CPUID_APIC	0x00000200	/* APIC on chip			*/
#define CPUID_SEP	0x00000800	/* SEP, Fast System Call	*/
#define CPUID_MTRR	0x00001000	/* MTRR				*/
#define CPUID_PGE	0x00002000	/* PTE global bit		*/
#define CPUID_MCA	0x00004000	/* machine check arch.		*/
#define CPUID_CMOV	0x00008000	/* cond. move/cmp. inst		*/
#define CPUID_PAT	0x00010000	/* page attribute table		*/
#define CPUID_PSE36	0x00020000	/* 36 bit page size extension	*/
#define CPUID_PSNUM	0x00040000	/* processor serial number	*/
#define CPUID_CLFLUSH	0x00080000	/* CLFLUSH inst supported	*/
#define CPUID_DTS	0x00200000	/* Debug Store			*/
#define CPUID_ACPI	0x00400000	/* TM and SCC supported		*/
#define CPUID_MMX	0x00800000	/* MMX technology supported	*/
#define CPUID_FXSR	0x01000000	/* fast FP save and restore	*/
#define CPUID_SSE	0x02000000	/* SSE supported		*/
#define CPUID_SSE2	0x04000000	/* SSE2 supported		*/
#define CPUID_SS	0x08000000	/* Self Snoop supported		*/
#define CPUID_HTT	0x10000000	/* Hyper Threading Technology   */
#define CPUID_TM	0x20000000	/* Thermal Monitor supported	*/
#define CPUID_IA64	0x40000000	/* IA64 Capabilities		*/
#define CPUID_PBE	0x80000000	/* Pending Break Enable		*/

/* CPUID: extended feature bit definitions (ECX) */

#define CPUID_GV3	0x00000080	/* Geyserville 3 supported	*/

#define CPUID_SSE3	0x00000001	/* SSE3 Extensions		*/
#define CPUID_MULDQ	0x00000002	/* support PCLMULDQ instruction	*/
#define CPUID_DTES64	0x00000004	/* 64-bit Debug Store		*/
#define CPUID_MON	0x00000008	/* Monitor/wait			*/
#define CPUID_DS_CPL	0x00000010	/* CPL qualified Debug Store	*/
#define CPUID_VMX	0x00000020	/* Virtual Machine Technology	*/
#define CPUID_SMX	0x00000040	/* Safer Mode Extensions	*/
#define CPUID_EST	0x00000080	/* Enhanced Speedstep Technology*/
#define CPUID_TM2	0x00000100	/* Thermal Monitor 2 supported	*/
#define CPUID_SSSE3	0x00000200	/* SSSE3 Extensions		*/
#define CPUID_CID	0x00000400	/* L1 context ID		*/
#define CPUID_CX16	0x00002000	/* CMPXCHG16B			*/
#define CPUID_XTPR	0x00004000	/* Update control		*/
#define CPUID_PDCM	0x00008000	/* Performance/Debug capability	*/
#define CPUID_DCA	0x00040000	/* Direct Cache Access		*/
#define CPUID_SSE41	0x00080000	/* SIMD Extensions 4.1		*/
#define CPUID_SSE42	0x00100000	/* SIMD Extensions 4.2		*/
#define CPUID_x2APIC	0x00200000	/* x2APIC supported		*/
#define CPUID_MOVBE	0x00400000	/* MOVBE instruction supported	*/
#define CPUID_POPCNT	0x00800000	/* POPCNT instruction supported	*/
#define CPUID_AES	0x02000000	/* AES instruction supported	*/
#define CPUID_XSAVE	0x04000000	/* XSAVE states supported	*/
#define CPUID_OSXSAVE	0x08000000	/* extended state management	*/

/* CPUID: offset in CPUID structure */

#define CPUID_HIGHVALUE		0	/* offset to highestValue	*/
#define CPUID_VENDORID		4	/* offset to vendorId		*/
#define CPUID_SIGNATURE		16	/* offset to signature		*/
#define CPUID_FEATURES_EBX	20	/* offset to featuresEbx	*/
#define CPUID_FEATURES_ECX	24	/* offset to featuresEcx	*/
#define CPUID_FEATURES_EDX	28	/* offset to featuresEdx	*/
#define CPUID_CACHE_EAX		32	/* offset to cacheEax		*/
#define CPUID_CACHE_EBX		36	/* offset to cacheEbx		*/
#define CPUID_CACHE_ECX		40	/* offset to cacheEcx		*/
#define CPUID_CACHE_EDX		44	/* offset to cacheEdx		*/
#define CPUID_SERIALNO		48	/* offset to serialNo64		*/
#define CPUID_BRAND_STR		56	/* offset to brandString[0]	*/

/* MSR_VMX_EPT_VPID_CAP: EOT abd VPID capabilities */

#define EPT_VPID_RWX_X_ONLY	0	/* execute only supported */
#define EPT_VPID_RWX_W_ONLY	1	/* write only supported */
#define EPT_VPID_RWX_XW_ONLY	2	/* execute and write supported */

#define EPT_VPID_GAW_21BITS	0x01	/* guest address width: 21-bits */
#define EPT_VPID_GAW_30BITS	0x02	/* guest address width: 30-bits */
#define EPT_VPID_GAW_39BITS	0x04	/* guest address width: 39-bits */
#define EPT_VPID_GAW_48BITS	0x08	/* guest address width: 48-bits */
#define EPT_VPID_GAW_57BITS	0x10	/* guest address width: 57-bits */

#define EPT_VPID_EMT_UC		0x01	/* EMT memory: uncached memory */
#define EPT_VPID_EMT_WC		0x02	/* EMT memory: write cache memory */
#define EPT_VPID_EMT_WT		0x10	/* EMT memory: write through memory */
#define EPT_VPID_EMT_WP		0x20	/* EMT memory: write protect memory */
#define EPT_VPID_EMT_WB		0x40	/* EMT memory: write back memory */

#define EPT_VPID_INVEPT_INDV	0x01	/* individual address sync */
#define EPT_VPID_INVEPT_CTX	0x02	/* context sync */
#define EPT_VPID_INVEPT_GLOBAL	0x08	/* global sync */

#define EPT_VPID_INVVPID_INDV	0x01	/* individual address sync */
#define EPT_VPID_INVVPID_CTX_1	0x02	/* single-context sync */
#define EPT_VPID_INVVPID_CTX_A	0x08	/* all context sync */
#define EPT_VPID_INVVPID_CTX_G	0x20	/* single context sync, keep global */

#define EPT_VPID_SP_2MB		0x01	/* super page support: 2MB */
#define EPT_VPID_SP_1GB		0x02	/* super page support: 1GB */
#define EPT_VPID_SP_512GB	0x04	/* super page support: 512GB */
#define EPT_VPID_SP_256TB	0x08	/* super page support: 256TB */

/* MSR, Model Specific Registers */

/* MSR, P5 only */

#define MSR_P5_MC_ADDR		0x0000
#define MSR_P5_MC_TYPE		0x0001
#define MSR_TSC			0x0010
#define MSR_CESR                0x0011
#define MSR_CTR0                0x0012
#define MSR_CTR1                0x0013

/* MSR, P5 and P6 */

#define MSR_APICBASE		0x001b
#define MSR_EBL_CR_POWERON	0x002a
#define MSR_TEST_CTL		0x0033
#define MSR_BIOS_UPDT_TRIG	0x0079
#define MSR_BBL_CR_D0		0x0088	/* P6 only */
#define MSR_BBL_CR_D1		0x0089	/* P6 only */
#define MSR_BBL_CR_D2		0x008a	/* P6 only */
#define MSR_BIOS_SIGN		0x008b
#define MSR_PERFCTR0		0x00c1
#define MSR_PERFCTR1		0x00c2
#define MSR_MTRR_CAP		0x00fe
#define MSR_BBL_CR_ADDR		0x0116	/* P6 only */
#define MSR_BBL_CR_DECC		0x0118	/* P6 only */
#define MSR_BBL_CR_CTL		0x0119	/* P6 only */
#define MSR_BBL_CR_TRIG		0x011a	/* P6 only */
#define MSR_BBL_CR_BUSY		0x011b	/* P6 only */
#define MSR_BBL_CR_CTL3		0x011e	/* P6 only */
#define MSR_SYSENTER_CS		0x0174	/* P6 + SEP only */
#define MSR_SYSENTER_ESP	0x0175	/* P6 + SEP only */
#define MSR_SYSENTER_EIP	0x0176	/* P6 + SEP only */
#define MSR_MCG_CAP		0x0179
#define MSR_MCG_STATUS		0x017a
#define MSR_MCG_CTL		0x017b
#define MSR_EVNTSEL0		0x0186
#define MSR_EVNTSEL1		0x0187
#define MSR_DEBUGCTLMSR		0x01d9
#define MSR_LASTBRANCH_FROMIP	0x01db
#define MSR_LASTBRANCH_TOIP	0x01dc
#define MSR_LASTINT_FROMIP	0x01dd
#define MSR_LASTINT_TOIP	0x01de
#define MSR_ROB_CR_BKUPTMPDR6	0x01e0
#define MSR_MTRR_PHYS_BASE0	0x0200
#define MSR_MTRR_PHYS_MASK0	0x0201
#define MSR_MTRR_PHYS_BASE1	0x0202
#define MSR_MTRR_PHYS_MASK1	0x0203
#define MSR_MTRR_PHYS_BASE2	0x0204
#define MSR_MTRR_PHYS_MASK2	0x0205
#define MSR_MTRR_PHYS_BASE3	0x0206
#define MSR_MTRR_PHYS_MASK3	0x0207
#define MSR_MTRR_PHYS_BASE4	0x0208
#define MSR_MTRR_PHYS_MASK4	0x0209
#define MSR_MTRR_PHYS_BASE5	0x020a
#define MSR_MTRR_PHYS_MASK5	0x020b
#define MSR_MTRR_PHYS_BASE6	0x020c
#define MSR_MTRR_PHYS_MASK6	0x020d
#define MSR_MTRR_PHYS_BASE7	0x020e
#define MSR_MTRR_PHYS_MASK7	0x020f
#define MSR_MTRR_FIX_00000	0x0250
#define MSR_MTRR_FIX_80000	0x0258
#define MSR_MTRR_FIX_A0000	0x0259
#define MSR_MTRR_FIX_C0000	0x0268
#define MSR_MTRR_FIX_C8000	0x0269
#define MSR_MTRR_FIX_D0000	0x026a
#define MSR_MTRR_FIX_D8000	0x026b
#define MSR_MTRR_FIX_E0000	0x026c
#define MSR_MTRR_FIX_E8000	0x026d
#define MSR_MTRR_FIX_F0000	0x026e
#define MSR_MTRR_FIX_F8000	0x026f
#define MSR_MTRR_DEFTYPE	0x02ff
#define MSR_MC0_CTL		0x0400
#define MSR_MC0_STATUS		0x0401
#define MSR_MC0_ADDR		0x0402
#define MSR_MC0_MISC		0x0403
#define MSR_MC1_CTL		0x0404
#define MSR_MC1_STATUS		0x0405
#define MSR_MC1_ADDR		0x0406
#define MSR_MC1_MISC		0x0407
#define MSR_MC2_CTL		0x0408
#define MSR_MC2_STATUS		0x0409
#define MSR_MC2_ADDR		0x040a
#define MSR_MC2_MISC		0x040b
#define MSR_MC4_CTL		0x040c
#define MSR_MC4_STATUS		0x040d
#define MSR_MC4_ADDR		0x040e
#define MSR_MC4_MISC		0x040f
#define MSR_MC3_CTL		0x0410
#define MSR_MC3_STATUS		0x0411
#define MSR_MC3_ADDR		0x0412
#define MSR_MC3_MISC		0x0413

/* MSR, Core2 */

#define MSR_FSB_FREQ		0x00cd

/* MSR, Nehalem */

#define MSR_PLATFORM_INFO		0x00ce
#define MSR_FLEX_RATIO			0x0194
#define MSR_CORE_THREAD_COUNT		0x0035
#define MSR_CORE_PERF_FIXED_CTR0	0x0309
#define MSR_CORE_PERF_FIXED_CTR1	0x030a
#define MSR_CORE_PERF_FIXED_CTR2	0x030b
#define MSR_CORE_PERF_FIXED_CTR_CTRL	0x038d
#define MSR_CORE_PERF_GLOBAL_STATUS	0x038e
#define MSR_CORE_PERF_GLOBAL_CTRL	0x038f
#define MSR_CORE_PERF_GLOBAL_OVF_CTRL	0x0390

/* MSR, VMX specific */

#define MSR_VMX_FEATURE			0x003A
#define MSR_VMX_BASIC			0x0480
#define MSR_VMX_PINBASED_CTLS_MSR	0x0481
#define MSR_VMX_PROCBASED_CTLS_MSR	0x0482
#define MSR_VMX_EXIT_CTLS_MSR		0x0483
#define MSR_VMX_ENTRY_CTLS_MSR		0x0484
#define MSR_VMX_MISC_MSR		0x0485
#define MSR_VMX_CR0_FIXED0		0x0486
#define MSR_VMX_CR0_FIXED1		0x0487
#define MSR_VMX_CR4_FIXED0		0x0488
#define MSR_VMX_CR4_FIXED1		0x0489
#define MSR_VMX_VMCS_ENUM		0x048A
#define MSR_VMX_PROCBASED_CTLS2_MSR	0x048B
#define MSR_VMX_EPT_VPID_CAP		0x048C
#define MSR_VMX_TRUE_PINBASED_CTLS	0x048D
#define MSR_VMX_TRUE_PROCBASED_CTLS	0x048E
#define MSR_VMX_TRUE_EXIT_CTLS		0x048F
#define MSR_VMX_TRUE_ENTRY_CTLS		0x0490

/* MSR, Architectural MSRs (common MSRs in IA32) */

#define IA32_P5_MC_ADDR		MSR_P5_MC_ADDR		/* P5 */
#define IA32_P5_MC_TYPE		MSR_P5_MC_TYPE		/* P5 */
#define IA32_TIME_STAMP_COUNTER	MSR_TSC			/* P5 */
#define IA32_PLATFORM_ID	0x0017			/* P6 */
#define IA32_APIC_BASE		MSR_APICBASE		/* P6 */
#define IA32_BIOS_UPDT_TRIG	MSR_BIOS_UPDT_TRIG	/* P6 */
#define IA32_BIOS_SIGN_ID	MSR_BIOS_SIGN		/* P6 */
#define IA32_MTRRCAP		MSR_MTRR_CAP		/* P6 */
#define IA32_MISC_CTL		MSR_BBL_CR_CTL		/* P6 */
#define IA32_SYSENTER_CS	MSR_SYSENTER_CS		/* P6 */
#define IA32_SYSENTER_ESP	MSR_SYSENTER_ESP	/* P6 */
#define IA32_SYSENTER_EIP	MSR_SYSENTER_EIP	/* P6 */
#define IA32_MCG_CAP		MSR_MCG_CAP		/* P6 */
#define IA32_MCG_STATUS		MSR_MCG_STATUS		/* P6 */
#define IA32_MCG_CTL		MSR_MCG_CTL		/* P6 */
#define IA32_MCG_EAX		0x0180			/* Pentium4 */
#define IA32_MCG_EBX		0x0181			/* Pentium4 */
#define IA32_MCG_ECX		0x0182			/* Pentium4 */
#define IA32_MCG_EDX		0x0183			/* Pentium4 */
#define IA32_MCG_ESI		0x0184			/* Pentium4 */
#define IA32_MCG_EDI		0x0185			/* Pentium4 */
#define IA32_MCG_EBP		0x0186			/* Pentium4 */
#define IA32_MCG_ESP		0x0187			/* Pentium4 */
#define IA32_MCG_EFLAGS		0x0188			/* Pentium4 */
#define IA32_MCG_EIP		0x0189			/* Pentium4 */
#define IA32_MCG_MISC		0x018a			/* Pentium4 */
#define IA32_THERM_CONTROL	0x019a			/* Pentium4 */
#define IA32_THERM_INTERRUPT	0x019b			/* Pentium4 */
#define IA32_THERM_STATUS	0x019c			/* Pentium4 */
#define IA32_MISC_ENABLE	0x01a0			/* Pentium4 */
#define IA32_DEBUGCTL		MSR_DEBUGCTLMSR		/* P6 */
#define IA32_MTRR_PHYSBASE0	MSR_MTRR_PHYS_BASE0	/* P6 */
#define IA32_MTRR_PHYSMASK0	MSR_MTRR_PHYS_MASK0	/* P6 */
#define IA32_MTRR_PHYSBASE1	MSR_MTRR_PHYS_BASE1	/* P6 */
#define IA32_MTRR_PHYSMASK1	MSR_MTRR_PHYS_MASK1	/* P6 */
#define IA32_MTRR_PHYSBASE2	MSR_MTRR_PHYS_BASE2	/* P6 */
#define IA32_MTRR_PHYSMASK2	MSR_MTRR_PHYS_MASK2	/* P6 */
#define IA32_MTRR_PHYSBASE3	MSR_MTRR_PHYS_BASE3	/* P6 */
#define IA32_MTRR_PHYSMASK3	MSR_MTRR_PHYS_MASK3	/* P6 */
#define IA32_MTRR_PHYSBASE4	MSR_MTRR_PHYS_BASE4	/* P6 */
#define IA32_MTRR_PHYSMASK4	MSR_MTRR_PHYS_MASK4	/* P6 */
#define IA32_MTRR_PHYSBASE5	MSR_MTRR_PHYS_BASE5	/* P6 */
#define IA32_MTRR_PHYSMASK5	MSR_MTRR_PHYS_MASK5	/* P6 */
#define IA32_MTRR_PHYSBASE6	MSR_MTRR_PHYS_BASE6	/* P6 */
#define IA32_MTRR_PHYSMASK6	MSR_MTRR_PHYS_MASK6	/* P6 */
#define IA32_MTRR_PHYSBASE7	MSR_MTRR_PHYS_BASE7	/* P6 */
#define IA32_MTRR_PHYSMASK7	MSR_MTRR_PHYS_MASK7	/* P6 */
#define IA32_MTRR_FIX64K_00000	MSR_MTRR_FIX_00000	/* P6 */
#define IA32_MTRR_FIX16K_80000	MSR_MTRR_FIX_80000	/* P6 */
#define IA32_MTRR_FIX16K_A0000	MSR_MTRR_FIX_A0000	/* P6 */
#define IA32_MTRR_FIX4K_C0000	MSR_MTRR_FIX_C0000	/* P6 */
#define IA32_MTRR_FIX4K_C8000	MSR_MTRR_FIX_C8000	/* P6 */
#define IA32_MTRR_FIX4K_D0000	MSR_MTRR_FIX_D0000	/* P6 */
#define IA32_MTRR_FIX4K_D8000	MSR_MTRR_FIX_D8000	/* P6 */
#define IA32_MTRR_FIX4K_E0000	MSR_MTRR_FIX_E0000	/* P6 */
#define IA32_MTRR_FIX4K_E8000	MSR_MTRR_FIX_E8000	/* P6 */
#define IA32_MTRR_FIX4K_F0000	MSR_MTRR_FIX_F0000	/* P6 */
#define IA32_MTRR_FIX4K_F8000	MSR_MTRR_FIX_F8000	/* P6 */
#define IA32_CR_PAT		0x0277			/* P6 */
#define IA32_MTRR_DEF_TYPE	MSR_MTRR_DEFTYPE	/* P6 */
#define IA32_PEBS_ENABLE	0x03f1			/* Pentium4 */
#define IA32_MC0_CTL		MSR_MC0_CTL		/* P6 */
#define IA32_MC0_STATUS		MSR_MC0_STATUS		/* P6 */
#define IA32_MC0_ADDR		MSR_MC0_ADDR		/* P6 */
#define IA32_MC0_MISC		MSR_MC0_MISC		/* P6 */
#define IA32_MC1_CTL		MSR_MC1_CTL		/* P6 */
#define IA32_MC1_STATUS		MSR_MC1_STATUS		/* P6 */
#define IA32_MC1_ADDR		MSR_MC1_ADDR		/* P6 */
#define IA32_MC1_MISC		MSR_MC1_MISC		/* P6 */
#define IA32_MC2_CTL		MSR_MC2_CTL		/* P6 */
#define IA32_MC2_STATUS		MSR_MC2_STATUS		/* P6 */
#define IA32_MC2_ADDR		MSR_MC2_ADDR		/* P6 */
#define IA32_MC2_MISC		MSR_MC2_MISC		/* P6 */
#define IA32_MC3_CTL		0x040c			/* P6, addr changed */
#define IA32_MC3_STATUS		0x040d			/* P6, addr changed */
#define IA32_MC3_ADDR		0x040e			/* P6, addr changed */
#define IA32_MC3_MISC		0x040f			/* P6, addr changed */
#define IA32_DS_AREA		0x0600			/* Pentium4 */
#define IA32_PERF_STAT		0x0198			/* Core2 */
#define IA32_PERF_CTRL		0x0199			/* Core2 */
#define IA32_EFER		0xc0000080		/* Core2 - extended feature */
#define IA32_STAR		0xc0000081		/* */
#define IA32_LSTAR		0xc0000082		/* */
#define IA32_CSTAR		0xc0000083		/* */
#define IA32_FMASK		0xc0000084		/* */
#define IA32_FSBASE		0xc0000100		/* */

#define IA32_GSBASE		0xc0000101		/* P4 */
#define IA32_KERNEL_GSBASE	0xc0000102		/* P4 */

/* P-state hardware coordination feedback registers */
#define IA32_MPERF		0x00E7
#define IA32_APERF		0x00E8

/* MSR, IA32_DEBUGCTL, in Pentium4, bits */

#define DBG_P7_LBR		0x00000001
#define DBG_P7_BTF		0x00000002
#define DBG_P7_TR		0x00000004
#define DBG_P7_BTS		0x00000008
#define DBG_P7_BTINT		0x00000010

/* MSR, IA32_DEBUGCTL, in P6, bits */

#define DBG_P6_LBR		0x00000001
#define DBG_P6_BTF		0x00000002
#define DBG_P6_PB0		0x00000004
#define DBG_P6_PB1		0x00000008
#define DBG_P6_PB2		0x00000010
#define DBG_P6_PB3		0x00000020
#define DBG_P6_TR		0x00000040

/* MSR, MSR_LASTBRANCH_TOS, in Pentium4, bits */

#define TOS_MASK		0x00000003

/* MSR, IA32_MISC_ENABLE bits */

#define MSC_FAST_STRING_ENABLE	0x00000001
#define MSC_FOPCODE_ENABLE	0x00000004
#define MSC_THERMAL_MON_ENABLE	0x00000008
#define MSC_SPLIT_LOCK_DISABLE	0x00000010
#define MSC_PMON_AVAILABLE	0x00000080
#define MSC_BTS_UNAVAILABLE	0x00000800
#define MSC_PEBS_UNAVAILABLE	0x00001000
#define MSC_GV1_EN		0x00008000
#define MSC_GV3_EN		0x00010000
#define MSC_GV_SEL_LOCK		0x00100000
#define MSC_LEAF_REPORTING	0x00400000

/* MSR, IA32_PEBS_ENABLE bits */

#define PEBS_METRICS		0x00001fff
#define PEBS_UOP_TAG		0x01000000
#define PEBS_ENABLE		0x02000000

/* MSR, IA32_PLATFORM_ID bits (upper 32) */

#define PFM_PLATFORM_ID		0x001c0000
#define PFM_MOBILE_GV		0x00040000

/* MSR, IA32_PLATFORM_ID bits (lower 32) */

#define PFM_MAX_VID		0x0000003f
#define PFM_MAX_FREQ		0x00000f80
#define PFM_RATIO_LOCKED	0x00008000
#define PFM_GV3_TM_DISABLED	0x00010000
#define PFM_GV3_DISABLED	0x00020000
#define PFM_GV1_DISABLED	0x00040000
#define PFM_TM_DISABLED		0x00080000
#define PFM_L2_CACHE_SIZE	0x06000000
#define PFM_SAMPLE		0x08000000

/* MSR, IA32_THERM_CONTROL bits */

#define THERM_DUTY_CYCLE	0x0000000e
#define THERM_TCC_EN		0x00000010

/* MSR, IA32_THERM_STATUS bits */

#define THERM_HOT_NOW		0x00000001
#define THERM_HOT_LOG		0x00000002

/* MSR, IA32_THERM_INTERRUPT bits */

#define THERM_HOT_INT_EN	0x00000001
#define THERM_COLD_INT_EN	0x00000002

/* PMC, Performance Monitoring Event Select MSR bits */

/* P5 specific */

#define P5PMC_PC                0x00000200
#define P5PMC_CC_DISABLE        0x00000000
#define P5PMC_CC_EVT_CPL012     0x00000040
#define P5PMC_CC_EVT_CPL3       0x00000080
#define P5PMC_CC_EVT            0x000000C0
#define P5PMC_CC_CLK_CPL012     0x00000140
#define P5PMC_CC_CLK_CPL3       0x00000180
#define P5PMC_CC_CLK            0x000001C0

/* P6 specific */

#define PMC_USR               0x00010000
#define PMC_OS                0x00020000
#define PMC_E                 0x00040000
#define PMC_PC                0x00080000
#define PMC_INT               0x00100000
#define PMC_EN                0x00400000
#define PMC_INV               0x00800000

/* MSR, MSR_VMX_ENTRY_CTLS bits */

#define VMX_ENTRY_CTLS_IA32e		0x00000200
#define VMX_ENTRY_CTLS_SMM		0x00000400
#define VMX_ENTRY_CTLS_DUAL		0x00000800

/* MSR, MSR_VMX_EXIT_CTLS_MSR bits */

#define VMX_EXIT_CTLS_64BIT		0x00000200
#define VMX_EXIT_CTLS_IACK		0x00008000

/* MSR, IA32_EFER bits */

#define EFER_SCE	0x00000001	/* SYSCALL/SYSRET (bit 0) */
#define EFER_LME	0x00000100	/* Long mode enable (bit 8) */
#define EFER_LMA	0x00000400	/* Long mode active (r/o) 10 */
#define EFER_NX	0x00000800	/* No execute enable (bit 11) */

/* PMC, Performance Monitoring Events */

/* P5 PMC event list */

#define P5PMC_DATA_RD			0x00
#define P5PMC_DATA_WR			0x01
#define P5PMC_DATA_TBL_MISS		0x02
#define P5PMC_DATA_RD_MISS		0x03
#define P5PMC_DATA_WR_MISS		0x04
#define P5PMC_WR_HIT_M_E_STATE_LINE	0x05
#define P5PMC_DCACHE_WR_BACK		0x06
#define P5PMC_EXT_SNOOPS		0x07
#define P5PMC_EXT_DCACHE_SNOOPS_HIT	0x08
#define P5PMC_MEM_ACCESS_BOTH_PIPES	0x09
#define P5PMC_BANK_CONFLICT		0x0A
#define P5PMC_MISC_DMEM_IO_REF		0x0B
#define P5PMC_CODE_RD			0x0C
#define P5PMC_CODE_TBL_MISS		0x0D
#define P5PMC_CODE_CACHE_MISS		0x0E
#define P5PMC_SEGMENT_REG_LOAD		0x0F
#define P5PMC_BRANCH			0x12
#define P5PMC_BTB_HIT			0x13
#define P5PMC_TAKEN_BRANCH_BTB_HIT	0x14
#define P5PMC_PIPELINE_FLUSH		0x15
#define P5PMC_INST_EXECUTED		0x16
#define P5PMC_INST_EXECUTED_VPIPE	0x17
#define P5PMC_BUS_CYC_DURATION		0x18
#define P5PMC_WR_BUF_FULL_STALL_DURATION 0x19
#define P5PMC_WAIT_MEM_RD_STALL_DURATION 0x1A
#define P5PMC_STALL_ON_WR_M_E_STATE_LINE 0x1B
#define P5PMC_LOCKED_BUS_CYC		0x1C
#define P5PMC_IO_RD_WR_CYC		0x1D
#define P5PMC_NONCACHE_MEM_RD		0x1E
#define P5PMC_PIPELINE_AGI_STALL	0x1F
#define P5PMC_FLOPS			0x22
#define P5PMC_BK_MATCH_DR0		0x23
#define P5PMC_BK_MATCH_DR1		0x24
#define P5PMC_BK_MATCH_DR2		0x25
#define P5PMC_BK_MATCH_DR3		0x26
#define P5PMC_HW_INT			0x27
#define P5PMC_DATA_RD_WR		0x28
#define P5PMC_DATA_RD_WR_MISS		0x29
#define P5PMC_BUS_OWNER_LATENCY		0x2A
#define P5PMC_BUS_OWNER_TRANSFER	0x2A
#define P5PMC_MMX_INST_UPIPE		0x2B
#define P5PMC_MMX_INST_VPIPE		0x2B
#define P5PMC_CACHE_M_LINE_SHARE	0x2C
#define P5PMC_CACHE_LINE_SHARE		0x2C
#define P5PMC_EMMS_INTS_EXECUTED	0x2D
#define P5PMC_TRANS_MMX_FP_INST		0x2D
#define P5PMC_BUS_UTIL_PROCESSOR_ACT	0x2D
#define P5PMC_WR_NOCACHEABLE_MEM	0x2E
#define P5PMC_SATURATING_MMX_INST	0x2F
#define P5PMC_SATURATION_PERFORMED	0x2F
#define P5PMC_NUM_CYC_NOT_HALT_STATE	0x30
#define P5PMC_DCACHE_TLB_MISS_STALL_DUR	0x30
#define P5PMC_MMX_INST_DATA_RD		0x31
#define P5PMC_MMX_INST_DATA_RD_MISS	0x31
#define P5PMC_FP_STALL_DUR		0x32
#define P5PMC_TAKEN_BRANCH		0x32
#define P5PMC_D1_STARV_FIFO_EMPTY	0x33
#define P5PMC_D1_STARV_ONE_INST_FIFO	0x33
#define P5PMC_MMX_INST_DATA_WR		0x34
#define P5PMC_MMX_INST_DATA_WR_MISS	0x34
#define P5PMC_PL_FLUSH_WRONG_BR_PREDIC	0x35
#define P5PMC_PL_FLUSH_WRONG_BR_PREDIC_WB 0x35
#define P5PMC_MISALIGN_DMEM_REF_MMX	0x36
#define P5PMC_PL_STALL_MMX_DMEM_RD	0x36
#define P5PMC_MISPREDIC_UNPREDIC_RET	0x37
#define P5PMC_PREDICED_RETURN		0x37
#define P5PMC_MMX_MUL_UNIT_INTERLOCK	0x38
#define P5PMC_MOVD_MOVQ_STALL_PREV_MMX	0x38
#define P5PMC_RETURN			0x39
#define P5PMC_BTB_FALSE_ENTRY		0x3A
#define P5PMC_BTB_MISS_PREDIC_NOT_TAKEN_BR 0x3A
#define P5PMC_FULL_WR_BUF_STALL_MMX	0x3B
#define P5PMC_STALL_MMX_WR_E_M_STATE_LINE 0x3B

/* P6 PMC event list */

#define PMC_DATA_MEM_REFS		0x43
#define PMC_DCU_LINES_IN		0x45
#define PMC_DCU_M_LINES_IN		0x46
#define PMC_DCU_M_LINES_OUT		0x47
#define PMC_DCU_MISS_OUTSTANDING	0x48
#define PMC_IFU_IFETCH			0x80
#define PMC_IFU_IFETCH_MISS		0x81
#define PMC_ITLB_MISS			0x85
#define PMC_IFU_MEM_STALL		0x86
#define PMC_IDL_STALL			0x87
#define PMC_L2_IFETCH			0x28
#define PMC_L2_LD			0x29
#define PMC_L2_ST			0x2a
#define PMC_L2_LINES_IN			0x24
#define PMC_L2_LINES_OUT		0x26
#define PMC_L2_M_LINES_INM		0x25
#define PMC_L2_M_LINES_OUTM		0x27
#define PMC_L2_RQSTS			0x2e
#define PMC_L2_ADS			0x21
#define PMC_L2_DBUS_BUSY		0x22
#define PMC_L2_DBUS_BUSY_RD		0x23
#define PMC_BUS_DRDY_CLOCKS		0x62
#define PMC_BUS_LOCK_CLOCKS		0x63
#define PMC_BUS_REQ_OUTSTANDING		0x60
#define PMC_BUS_TRAN_BRD		0x65
#define PMC_BUS_TRAN_RFO		0x66
#define PMC_BUS_TRANS_WB		0x67
#define PMC_BUS_TRAN_IFETCH		0x68
#define PMC_BUS_TRAN_INVAL		0x69
#define PMC_BUS_TRAN_PWR		0x6a
#define PMC_BUS_TRANS_P			0x6b
#define PMC_BUS_TRANS_IO		0x6c
#define PMC_BUS_TRAN_DEF		0x6d
#define PMC_BUS_TRAN_BURST		0x6e
#define PMC_BUS_TRAN_ANY		0x70
#define PMC_BUS_TRAN_MEM		0x6f
#define PMC_BUS_DATA_RCV		0x64
#define PMC_BUS_BNR_DRV			0x61
#define PMC_BUS_HIT_DRV			0x7a
#define PMC_BUS_HITM_DRV		0x7b
#define PMC_BUS_SNOOP_STALL		0x7e
#define PMC_FLOPS			0xc1
#define PMC_FP_COMP_OPS_EXE		0x10
#define PMC_FP_ASSIST			0x11
#define PMC_MUL				0x12
#define PMC_DIV				0x13
#define PMC_CYCLES_DIV_BUSY		0x14
#define PMC_LD_BLOCKS			0x03
#define PMC_SB_DRAINS			0x04
#define PMC_MISALIGN_MEM_REF		0x05
#define PMC_INST_RETIRED		0xc0
#define PMC_UOPS_RETIRED		0xc2
#define PMC_INST_DECODER		0xd0
#define PMC_HW_INT_RX			0xc8
#define PMC_CYCLES_INT_MASKED		0xc6
#define PMC_CYCLES_INT_PENDING_AND_MASKED 0xc7
#define PMC_BR_INST_RETIRED		0xc4
#define PMC_BR_MISS_PRED_RETIRED	0xc5
#define PMC_BR_TAKEN_RETIRED		0xc9
#define PMC_BR_MISS_PRED_TAKEN_RETIRED	0xca
#define PMC_BR_INST_DECODED		0xe0
#define PMC_BTB_MISSES			0xe2
#define PMC_BR_BOGUS			0xe4
#define PMC_BACLEARS			0xe6
#define PMC_RESOURCE_STALLS		0xa2
#define PMC_PARTIAL_RAT_STALLS		0xd2
#define PMC_SEGMENT_REG_LOADS		0x06
#define PMC_CPU_CLK_UNHALTED		0x79
#define PMC_UMASK_00			0x00
#define PMC_UMASK_0F			0x0f
#define PMC_UMASK_SELF			0x00
#define PMC_UMASK_ANY			0x20

/* MTRR related defines */

#define MTRR_UC				0x00
#define MTRR_WC				0x01
#define MTRR_WT				0x04
#define MTRR_WP				0x05
#define MTRR_WB				0x06
#define MTRR_E				0x00000800
#define MTRR_FE				0x00000400
#define MTRR_VCNT			0x000000FF
#define MTRR_FIX_SUPPORT		0x00000100
#define MTRR_WC_SUPPORT			0x00000400

/* MCA related defines */

#define MCG_CTL_P			0x00000100
#define MCG_COUNT			0x000000ff
#define MCG_MCIP			0x00000004
#define MCG_EIPV			0x00000002
#define MCG_RIPV			0x00000001
#define MCI_VAL				0x80000000
#define MCI_OVER			0x40000000
#define MCI_UC				0x20000000
#define MCI_EN				0x10000000
#define MCI_MISCV			0x08000000
#define MCI_ADDRV			0x04000000
#define MCI_PCC				0x02000000

/* segment descriptor: types for application code, data segment */

#define SEG_DATA_RO_U		0x00000000	/* read only */
#define SEG_DATA_RW_U		0x00000200	/* read write */
#define SEG_DATA_RO_D		0x00000400	/* read only expand down */
#define SEG_DATA_RW_D		0x00000600	/* read write expand down */
#define SEG_CODE_EO		0x00000800	/* exec only */
#define SEG_CODE_ER		0x00000a00	/* exec read */
#define SEG_CODE_EO_C		0x00000c00	/* exec only conform. */
#define SEG_CODE_ERO_C		0x00000e00	/* exec read only conform. */

/* segment descriptor: types for system segment and gate */

#define SEG_LDT			0x00000200	/* LDT */
#define TASK_GATE		0x00000500	/* Task Gate */
#define TSS32			0x00000900	/* 32 bit TSS (available) */
#define CALL_GATE32		0x00000c00	/* 32 bit CALL gate */
#define INT_GATE32		0x00000e00	/* 32 bit INT  gate */
#define TRAP_GATE32		0x00000f00	/* 32 bit TRAP gate */

/* segment descriptor: descriptor type */

#define SYS_DESC		0x00000000	/* system descriptors */
#define APP_DESC		0x00001000	/* application descriptors */

/* segment descriptor: privilege level */

#define DPL0			0x00000000	/* privilege level 0 */
#define DPL1			0x00002000	/* privilege level 1 */
#define DPL2			0x00004000	/* privilege level 2 */
#define DPL3			0x00006000	/* privilege level 3 */

/* segment descriptor: privilege level */

#define SEG_P			0x00008000	/* present */

/* segment descriptor: default operation size */

#define DB_16			0x00000000	/* 16 bit segment */
#define DB_32			0x00400000	/* 32 bit segment */

/* segment descriptor: granularity */

#define G_BYTE			0x00000000	/* byte granularity */
#define G_4K			0x00800000	/* 4K byte granularity */

/* segment descriptor: mask bits for attribute */

#define SEG_ATTR_MASK		0x00f0ff00	/* mask bits */

/* segment descriptor: pSegdesc->type: present and busy bit */

#define SEG_PRESENT		0x80		/* present bit */
#define SEG_BUSY		0x02		/* busy bit */

/* segment register requested privilege levels */

#define SEG_RPL_RING_0		0x0
#define SEG_RPL_RING_1		0x1
#define SEG_RPL_RING_2		0x2
#define SEG_RPL_RING_3		0x3
#define SEG_RPL_MASK		0x3

/* IDT related macros */

#define IDT_TASK_GATE	0x0000e500	/* task gate: P=1, DPL=3, task id */
#define IDT_TRAP_GATE	0x0000ef00	/* trap gate: P=1, DPL=3, trap id */
#define IDT_INT_GATE	0x0000ee00	/* int gate: P=1, DPL=3, int id */

/* TSS related macros */

#define TSS_LINK		  0	/* offset: link to previous task */
#define TSS_ESP0		  4	/* offset: privilege level 0 SP */
#define TSS_SS0			  8	/* offset:   ''              SS */
#define TSS_ESP1		 12	/* offset: privilege level 1 SP */
#define TSS_SS1			 16	/* offset:   ''              SS */
#define TSS_ESP2		 20	/* offset: privilege level 2 SP */
#define TSS_SS2			 24	/* offset:   ''              SS */
#define TSS_CR3			 28	/* offset: control register CR3 */
#define TSS_EIP			 32	/* offset: program counter  EIP */
#define TSS_EFLAGS		 36	/* offset: status register  EFLAGS */
#define TSS_EAX			 40	/* offset: general register EAX */
#define TSS_ECX			 44	/* offset: general register ECX */
#define TSS_EDX			 48	/* offset: general register EDX */
#define TSS_EBX			 52	/* offset: general register EBX */
#define TSS_ESP			 56	/* offset: stack pointer ESP */
#define TSS_EBP			 60	/* offset: frame pointer EBP */
#define TSS_ESI			 64	/* offset: general register ESI */
#define TSS_EDI			 68	/* offset: general register EDI */
#define TSS_ES			 72	/* offset: segment selector ES */
#define TSS_CS			 76	/* offset: segment selector CS */
#define TSS_SS			 80	/* offset: segment selector SS */
#define TSS_DS			 84	/* offset: segment selector DS */
#define TSS_FS			 88	/* offset: segment selector FS */
#define TSS_GS			 92	/* offset: segment selector GS */
#define TSS_LDT			 96	/* offset: segment selector LDT */
#define TSS_TFLAG		100	/* offset: debug trap flag T */
#define TSS_IOMAPB		102	/* offset: IO map base address */
#define TSS_IOBMAP		104	/* offset: IO bit map array */

#define TSS_BUSY_MASK	0xfffffdff	/* TSS descriptor BUSY bit mask */

/* VMX realted defines */

#define VMX_MEMORY_TYPE_UC	0	/* Strong uncacheable (UC) */
#define VMX_MEMORY_TYPE_WB	6	/* Write back (WB) */

/* inline version of intLock()/intUnlock() : used in mmuI86Lib.c */

#define INT_LOCK(oldLevel) \
    WRS_ASM ("pushf ; popl %0 ; andl $0x00000200, %0 ; cli" \
    : "=rm" (oldLevel) : /* no input */ : "memory")

#define INT_UNLOCK(oldLevel) \
    WRS_ASM ("testl $0x00000200, %0 ; jz 0f ; sti ; 0:" \
    : /* no output */ : "rm" (oldLevel) : "memory")

#endif	/* __ASM_REG_VBI_H */
