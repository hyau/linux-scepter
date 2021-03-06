/*
 * This file contains miscellaneous low-level functions.
 *    Copyright (C) 1995-1996 Gary Thomas (gdt@linuxppc.org)
 *
 * Largely rewritten by Cort Dougan (cort@cs.nmt.edu)
 * and Paul Mackerras.
 *
 * kexec bits:
 * Copyright (C) 2002-2003 Eric Biederman  <ebiederm@xmission.com>
 * GameCube/ppc32 port Copyright (C) 2004 Albert Herranz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 */

#include <linux/sys.h>
#include <asm/unistd.h>
#include <asm/errno.h>
#include <asm/reg.h>
#include <asm/page.h>
#include <asm/cache.h>
#include <asm/cputable.h>
#include <asm/mmu.h>
#include <asm/ppc_asm.h>
#include <asm/thread_info.h>
#include <asm/asm-offsets.h>
#include <asm/processor.h>
#include <asm/kexec.h>
#include <asm/bug.h>

	.text

#ifdef CONFIG_IRQSTACKS
_GLOBAL(call_do_softirq)
	mflr	r0
	stw	r0,4(r1)
	stwu	r1,THREAD_SIZE-STACK_FRAME_OVERHEAD(r3)
	mr	r1,r3
	bl	__do_softirq
	lwz	r1,0(r1)
	lwz	r0,4(r1)
	mtlr	r0
	blr

_GLOBAL(call_handle_irq)
	mflr	r0
	stw	r0,4(r1)
	mtctr	r6
	stwu	r1,THREAD_SIZE-STACK_FRAME_OVERHEAD(r5)
	mr	r1,r5
	bctrl
	lwz	r1,0(r1)
	lwz	r0,4(r1)
	mtlr	r0
	blr
#endif /* CONFIG_IRQSTACKS */

/*
 * This returns the high 64 bits of the product of two 64-bit numbers.
 */
_GLOBAL(mulhdu)
	cmpwi	r6,0
	cmpwi	cr1,r3,0
	mr	r10,r4
	mulhwu	r4,r4,r5
	beq	1f
	mulhwu	r0,r10,r6
	mullw	r7,r10,r5
	addc	r7,r0,r7
	addze	r4,r4
1:	beqlr	cr1		/* all done if high part of A is 0 */
	mr	r10,r3
	mullw	r9,r3,r5
	mulhwu	r3,r3,r5
	beq	2f
	mullw	r0,r10,r6
	mulhwu	r8,r10,r6
	addc	r7,r0,r7
	adde	r4,r4,r8
	addze	r3,r3
2:	addc	r4,r4,r9
	addze	r3,r3
	blr

/*
 * sub_reloc_offset(x) returns x - reloc_offset().
 */
_GLOBAL(sub_reloc_offset)
	mflr	r0
	bl	1f
1:	mflr	r5
	lis	r4,1b@ha
	addi	r4,r4,1b@l
	subf	r5,r4,r5
	subf	r3,r5,r3
	mtlr	r0
	blr

/*
 * reloc_got2 runs through the .got2 section adding an offset
 * to each entry.
 */
_GLOBAL(reloc_got2)
	mflr	r11
	lis	r7,__got2_start@ha
	addi	r7,r7,__got2_start@l
	lis	r8,__got2_end@ha
	addi	r8,r8,__got2_end@l
	subf	r8,r7,r8
	srwi.	r8,r8,2
	beqlr
	mtctr	r8
	bl	1f
1:	mflr	r0
	lis	r4,1b@ha
	addi	r4,r4,1b@l
	subf	r0,r4,r0
	add	r7,r0,r7
2:	lwz	r0,0(r7)
	add	r0,r0,r3
	stw	r0,0(r7)
	addi	r7,r7,4
	bdnz	2b
	mtlr	r11
	blr

/*
 * call_setup_cpu - call the setup_cpu function for this cpu
 * r3 = data offset, r24 = cpu number
 *
 * Setup function is called with:
 *   r3 = data offset
 *   r4 = ptr to CPU spec (relocated)
 */
_GLOBAL(call_setup_cpu)
	addis	r4,r3,cur_cpu_spec@ha
	addi	r4,r4,cur_cpu_spec@l
	lwz	r4,0(r4)
	add	r4,r4,r3
	lwz	r5,CPU_SPEC_SETUP(r4)
	cmpwi	0,r5,0
	add	r5,r5,r3
	beqlr
	mtctr	r5
	bctr

#if defined(CONFIG_CPU_FREQ_PMAC) && defined(CONFIG_6xx)

/* This gets called by via-pmu.c to switch the PLL selection
 * on 750fx CPU. This function should really be moved to some
 * other place (as most of the cpufreq code in via-pmu
 */
_GLOBAL(low_choose_750fx_pll)
	/* Clear MSR:EE */
	mfmsr	r7
	rlwinm	r0,r7,0,17,15
	mtmsr	r0

	/* If switching to PLL1, disable HID0:BTIC */
	cmplwi	cr0,r3,0
	beq	1f
	mfspr	r5,SPRN_HID0
	rlwinm	r5,r5,0,27,25
	sync
	mtspr	SPRN_HID0,r5
	isync
	sync

1:
	/* Calc new HID1 value */
	mfspr	r4,SPRN_HID1	/* Build a HID1:PS bit from parameter */
	rlwinm	r5,r3,16,15,15	/* Clear out HID1:PS from value read */
	rlwinm	r4,r4,0,16,14	/* Could have I used rlwimi here ? */
	or	r4,r4,r5
	mtspr	SPRN_HID1,r4

	/* Store new HID1 image */
	rlwinm	r6,r1,0,0,(31-THREAD_SHIFT)
	lwz	r6,TI_CPU(r6)
	slwi	r6,r6,2
	addis	r6,r6,nap_save_hid1@ha
	stw	r4,nap_save_hid1@l(r6)

	/* If switching to PLL0, enable HID0:BTIC */
	cmplwi	cr0,r3,0
	bne	1f
	mfspr	r5,SPRN_HID0
	ori	r5,r5,HID0_BTIC
	sync
	mtspr	SPRN_HID0,r5
	isync
	sync

1:
	/* Return */
	mtmsr	r7
	blr

_GLOBAL(low_choose_7447a_dfs)
	/* Clear MSR:EE */
	mfmsr	r7
	rlwinm	r0,r7,0,17,15
	mtmsr	r0
	
	/* Calc new HID1 value */
	mfspr	r4,SPRN_HID1
	insrwi	r4,r3,1,9	/* insert parameter into bit 9 */
	sync
	mtspr	SPRN_HID1,r4
	sync
	isync

	/* Return */
	mtmsr	r7
	blr

#endif /* CONFIG_CPU_FREQ_PMAC && CONFIG_6xx */

/*
 * complement mask on the msr then "or" some values on.
 *     _nmask_and_or_msr(nmask, value_to_or)
 */
_GLOBAL(_nmask_and_or_msr)
	mfmsr	r0		/* Get current msr */
	andc	r0,r0,r3	/* And off the bits set in r3 (first parm) */
	or	r0,r0,r4	/* Or on the bits in r4 (second parm) */
	SYNC			/* Some chip revs have problems here... */
	mtmsr	r0		/* Update machine state */
	isync
	blr			/* Done */

#ifdef CONFIG_40x

/*
 * Do an IO access in real mode
 */
_GLOBAL(real_readb)
	mfmsr	r7
	ori	r0,r7,MSR_DR
	xori	r0,r0,MSR_DR
	sync
	mtmsr	r0
	sync
	isync
	lbz	r3,0(r3)
	sync
	mtmsr	r7
	sync
	isync
	blr

	/*
 * Do an IO access in real mode
 */
_GLOBAL(real_writeb)
	mfmsr	r7
	ori	r0,r7,MSR_DR
	xori	r0,r0,MSR_DR
	sync
	mtmsr	r0
	sync
	isync
	stb	r3,0(r4)
	sync
	mtmsr	r7
	sync
	isync
	blr

#endif /* CONFIG_40x */


/*
 * Flush instruction cache.
 * This is a no-op on the 601.
 */
_GLOBAL(flush_instruction_cache)
#if defined(CONFIG_8xx)
	isync
	lis	r5, IDC_INVALL@h
	mtspr	SPRN_IC_CST, r5
#elif defined(CONFIG_4xx)
#ifdef CONFIG_403GCX
	li      r3, 512
	mtctr   r3
	lis     r4, KERNELBASE@h
1:	iccci   0, r4
	addi    r4, r4, 16
	bdnz    1b
#else
	lis	r3, KERNELBASE@h
	iccci	0,r3
#endif
#elif CONFIG_FSL_BOOKE
BEGIN_FTR_SECTION
	mfspr   r3,SPRN_L1CSR0
	ori     r3,r3,L1CSR0_CFI|L1CSR0_CLFC
	/* msync; isync recommended here */
	mtspr   SPRN_L1CSR0,r3
	isync
	blr
END_FTR_SECTION_IFSET(CPU_FTR_UNIFIED_ID_CACHE)
	mfspr	r3,SPRN_L1CSR1
	ori	r3,r3,L1CSR1_ICFI|L1CSR1_ICLFR
	mtspr	SPRN_L1CSR1,r3
#else
	mfspr	r3,SPRN_PVR
	rlwinm	r3,r3,16,16,31
	cmpwi	0,r3,1
	beqlr			/* for 601, do nothing */
	/* 603/604 processor - use invalidate-all bit in HID0 */
	mfspr	r3,SPRN_HID0
	ori	r3,r3,HID0_ICFI
	mtspr	SPRN_HID0,r3
#endif /* CONFIG_8xx/4xx */
	isync
	blr

/*
 * Write any modified data cache blocks out to memory
 * and invalidate the corresponding instruction cache blocks.
 * This is a no-op on the 601.
 *
 * flush_icache_range(unsigned long start, unsigned long stop)
 */
_KPROBE(__flush_icache_range)
#ifndef CONFIG_PARAVIRT
	b	native__flush_icache_range
#else
	b	paravirt__flush_icache_range
#endif
_KPROBE(native__flush_icache_range)
BEGIN_FTR_SECTION
	blr				/* for 601, do nothing */
END_FTR_SECTION_IFSET(CPU_FTR_COHERENT_ICACHE)
	li	r5,L1_CACHE_BYTES-1
	andc	r3,r3,r5
	subf	r4,r3,r4
	add	r4,r4,r5
	srwi.	r4,r4,L1_CACHE_SHIFT
	beqlr
	mtctr	r4
	mr	r6,r3
1:	dcbst	0,r3
	addi	r3,r3,L1_CACHE_BYTES
	bdnz	1b
	sync				/* wait for dcbst's to get to ram */
#ifndef CONFIG_44x
	mtctr	r4
2:	icbi	0,r6
	addi	r6,r6,L1_CACHE_BYTES
	bdnz	2b
#else
	/* Flash invalidate on 44x because we are passed kmapped addresses and
	   this doesn't work for userspace pages due to the virtually tagged
	   icache.  Sigh. */
	iccci	0, r0
#endif
	sync				/* additional sync needed on g4 */
	isync
	blr
/*
 * Write any modified data cache blocks out to memory.
 * Does not invalidate the corresponding cache lines (especially for
 * any corresponding instruction cache).
 *
 * clean_dcache_range(unsigned long start, unsigned long stop)
 */
_GLOBAL(clean_dcache_range)
#ifndef CONFIG_PARAVIRT
        b       native_clean_dcache_range
#else
        b       paravirt_clean_dcache_range
#endif
_GLOBAL(native_clean_dcache_range)
	li	r5,L1_CACHE_BYTES-1
	andc	r3,r3,r5
	subf	r4,r3,r4
	add	r4,r4,r5
	srwi.	r4,r4,L1_CACHE_SHIFT
	beqlr
	mtctr	r4

1:	dcbst	0,r3
	addi	r3,r3,L1_CACHE_BYTES
	bdnz	1b
	sync				/* wait for dcbst's to get to ram */
	blr

/*
 * Write any modified data cache blocks out to memory and invalidate them.
 * Does not invalidate the corresponding instruction cache blocks.
 *
 * flush_dcache_range(unsigned long start, unsigned long stop)
 */
_GLOBAL(flush_dcache_range)
#ifndef CONFIG_PARAVIRT
	b	native_flush_dcache_range
#else
	b	paravirt_flush_dcache_range
#endif
_GLOBAL(native_flush_dcache_range)
	li	r5,L1_CACHE_BYTES-1
	andc	r3,r3,r5
	subf	r4,r3,r4
	add	r4,r4,r5
	srwi.	r4,r4,L1_CACHE_SHIFT
	beqlr
	mtctr	r4

1:	dcbf	0,r3
	addi	r3,r3,L1_CACHE_BYTES
	bdnz	1b
	sync				/* wait for dcbst's to get to ram */
	blr

/*
 * Like above, but invalidate the D-cache.  This is used by the 8xx
 * to invalidate the cache so the PPC core doesn't get stale data
 * from the CPM (no cache snooping here :-).
 *
 * invalidate_dcache_range(unsigned long start, unsigned long stop)
 */
_GLOBAL(invalidate_dcache_range)
	li	r5,L1_CACHE_BYTES-1
	andc	r3,r3,r5
	subf	r4,r3,r4
	add	r4,r4,r5
	srwi.	r4,r4,L1_CACHE_SHIFT
	beqlr
	mtctr	r4

1:	dcbi	0,r3
	addi	r3,r3,L1_CACHE_BYTES
	bdnz	1b
	sync				/* wait for dcbi's to get to ram */
	blr

/*
 * Flush a particular page from the data cache to RAM.
 * Note: this is necessary because the instruction cache does *not*
 * snoop from the data cache.
 * This is a no-op on the 601 which has a unified cache.
 *
 *	void __flush_dcache_icache(void *page)
 */
_GLOBAL(__flush_dcache_icache)
#ifndef CONFIG_PARAVIRT
	b	native__flush_dcache_icache
#else
	b	paravirt__flush_dcache_icache
#endif
_GLOBAL(native__flush_dcache_icache)
BEGIN_FTR_SECTION
	blr
END_FTR_SECTION_IFSET(CPU_FTR_COHERENT_ICACHE)
	rlwinm	r3,r3,0,0,31-PAGE_SHIFT		/* Get page base address */
	li	r4,PAGE_SIZE/L1_CACHE_BYTES	/* Number of lines in a page */
	mtctr	r4
	mr	r6,r3
0:	dcbst	0,r3				/* Write line to ram */
	addi	r3,r3,L1_CACHE_BYTES
	bdnz	0b
	sync
#ifndef CONFIG_44x
	/* We don't flush the icache on 44x. Those have a virtual icache
	 * and we don't have access to the virtual address here (it's
	 * not the page vaddr but where it's mapped in user space). The
	 * flushing of the icache on these is handled elsewhere, when
	 * a change in the address space occurs, before returning to
	 * user space
	 */
	mtctr	r4
1:	icbi	0,r6
	addi	r6,r6,L1_CACHE_BYTES
	bdnz	1b
	sync
	isync
#endif /* CONFIG_44x */
	blr

/*
 * Flush a particular page from the data cache to RAM, identified
 * by its physical address.  We turn off the MMU so we can just use
 * the physical address (this may be a highmem page without a kernel
 * mapping).
 *
 *	void __flush_dcache_icache_phys(unsigned long physaddr)
 */
_GLOBAL(__flush_dcache_icache_phys)
#ifndef CONFIG_PARAVIRT
	b	native__flush_dcache_icache_phys
#else
	b	paravirt__flush_dcache_icache_phys
#endif
_GLOBAL(native__flush_dcache_icache_phys)
BEGIN_FTR_SECTION
	blr					/* for 601, do nothing */
END_FTR_SECTION_IFSET(CPU_FTR_COHERENT_ICACHE)
	mfmsr	r10
	rlwinm	r0,r10,0,28,26			/* clear DR */
	mtmsr	r0
	isync
	rlwinm	r3,r3,0,0,31-PAGE_SHIFT		/* Get page base address */
	li	r4,PAGE_SIZE/L1_CACHE_BYTES	/* Number of lines in a page */
	mtctr	r4
	mr	r6,r3
0:	dcbst	0,r3				/* Write line to ram */
	addi	r3,r3,L1_CACHE_BYTES
	bdnz	0b
	sync
	mtctr	r4
1:	icbi	0,r6
	addi	r6,r6,L1_CACHE_BYTES
	bdnz	1b
	sync
	mtmsr	r10				/* restore DR */
	isync
	blr

/*
 * Clear pages using the dcbz instruction, which doesn't cause any
 * memory traffic (except to write out any cache lines which get
 * displaced).  This only works on cacheable memory.
 *
 * void clear_pages(void *page, int order) ;
 */
_GLOBAL(clear_pages)
	li	r0,PAGE_SIZE/L1_CACHE_BYTES
	slw	r0,r0,r4
	mtctr	r0
1:	dcbz	0,r3
	addi	r3,r3,L1_CACHE_BYTES
	bdnz	1b
	blr

/*
 * Copy a whole page.  We use the dcbz instruction on the destination
 * to reduce memory traffic (it eliminates the unnecessary reads of
 * the destination into cache).  This requires that the destination
 * is cacheable.
 */
#define COPY_16_BYTES		\
	lwz	r6,4(r4);	\
	lwz	r7,8(r4);	\
	lwz	r8,12(r4);	\
	lwzu	r9,16(r4);	\
	stw	r6,4(r3);	\
	stw	r7,8(r3);	\
	stw	r8,12(r3);	\
	stwu	r9,16(r3)

_GLOBAL(copy_page)
	addi	r3,r3,-4
	addi	r4,r4,-4

	li	r5,4

#if MAX_COPY_PREFETCH > 1
	li	r0,MAX_COPY_PREFETCH
	li	r11,4
	mtctr	r0
11:	dcbt	r11,r4
	addi	r11,r11,L1_CACHE_BYTES
	bdnz	11b
#else /* MAX_COPY_PREFETCH == 1 */
	dcbt	r5,r4
	li	r11,L1_CACHE_BYTES+4
#endif /* MAX_COPY_PREFETCH */
	li	r0,PAGE_SIZE/L1_CACHE_BYTES - MAX_COPY_PREFETCH
	crclr	4*cr0+eq
2:
	mtctr	r0
1:
	dcbt	r11,r4
	dcbz	r5,r3
	COPY_16_BYTES
#if L1_CACHE_BYTES >= 32
	COPY_16_BYTES
#if L1_CACHE_BYTES >= 64
	COPY_16_BYTES
	COPY_16_BYTES
#if L1_CACHE_BYTES >= 128
	COPY_16_BYTES
	COPY_16_BYTES
	COPY_16_BYTES
	COPY_16_BYTES
#endif
#endif
#endif
	bdnz	1b
	beqlr
	crnot	4*cr0+eq,4*cr0+eq
	li	r0,MAX_COPY_PREFETCH
	li	r11,4
	b	2b

/*
 * void atomic_clear_mask(atomic_t mask, atomic_t *addr)
 * void atomic_set_mask(atomic_t mask, atomic_t *addr);
 */
_GLOBAL(atomic_clear_mask)
10:	lwarx	r5,0,r4
	andc	r5,r5,r3
	PPC405_ERR77(0,r4)
	stwcx.	r5,0,r4
	bne-	10b
	blr
_GLOBAL(atomic_set_mask)
10:	lwarx	r5,0,r4
	or	r5,r5,r3
	PPC405_ERR77(0,r4)
	stwcx.	r5,0,r4
	bne-	10b
	blr

/*
 * Extended precision shifts.
 *
 * Updated to be valid for shift counts from 0 to 63 inclusive.
 * -- Gabriel
 *
 * R3/R4 has 64 bit value
 * R5    has shift count
 * result in R3/R4
 *
 *  ashrdi3: arithmetic right shift (sign propagation)	
 *  lshrdi3: logical right shift
 *  ashldi3: left shift
 */
_GLOBAL(__ashrdi3)
	subfic	r6,r5,32
	srw	r4,r4,r5	# LSW = count > 31 ? 0 : LSW >> count
	addi	r7,r5,32	# could be xori, or addi with -32
	slw	r6,r3,r6	# t1 = count > 31 ? 0 : MSW << (32-count)
	rlwinm	r8,r7,0,32	# t3 = (count < 32) ? 32 : 0
	sraw	r7,r3,r7	# t2 = MSW >> (count-32)
	or	r4,r4,r6	# LSW |= t1
	slw	r7,r7,r8	# t2 = (count < 32) ? 0 : t2
	sraw	r3,r3,r5	# MSW = MSW >> count
	or	r4,r4,r7	# LSW |= t2
	blr

_GLOBAL(__ashldi3)
	subfic	r6,r5,32
	slw	r3,r3,r5	# MSW = count > 31 ? 0 : MSW << count
	addi	r7,r5,32	# could be xori, or addi with -32
	srw	r6,r4,r6	# t1 = count > 31 ? 0 : LSW >> (32-count)
	slw	r7,r4,r7	# t2 = count < 32 ? 0 : LSW << (count-32)
	or	r3,r3,r6	# MSW |= t1
	slw	r4,r4,r5	# LSW = LSW << count
	or	r3,r3,r7	# MSW |= t2
	blr

_GLOBAL(__lshrdi3)
	subfic	r6,r5,32
	srw	r4,r4,r5	# LSW = count > 31 ? 0 : LSW >> count
	addi	r7,r5,32	# could be xori, or addi with -32
	slw	r6,r3,r6	# t1 = count > 31 ? 0 : MSW << (32-count)
	srw	r7,r3,r7	# t2 = count < 32 ? 0 : MSW >> (count-32)
	or	r4,r4,r6	# LSW |= t1
	srw	r3,r3,r5	# MSW = MSW >> count
	or	r4,r4,r7	# LSW |= t2
	blr

/*
 * 64-bit comparison: __ucmpdi2(u64 a, u64 b)
 * Returns 0 if a < b, 1 if a == b, 2 if a > b.
 */
_GLOBAL(__ucmpdi2)
	cmplw	r3,r5
	li	r3,1
	bne	1f
	cmplw	r4,r6
	beqlr
1:	li	r3,0
	bltlr
	li	r3,2
	blr

_GLOBAL(abs)
	srawi	r4,r3,31
	xor	r3,r3,r4
	sub	r3,r3,r4
	blr

/*
 * Create a kernel thread
 *   kernel_thread(fn, arg, flags)
 */
_GLOBAL(original_kernel_thread)
	stwu	r1,-16(r1)
	stw	r30,8(r1)
	stw	r31,12(r1)
	mr	r30,r3		/* function */
	mr	r31,r4		/* argument */
	ori	r3,r5,CLONE_VM	/* flags */
	oris	r3,r3,CLONE_UNTRACED>>16
	li	r4,0		/* new sp (unused) */
	li	r0,__NR_clone
	sc
	bns+	1f		/* did system call indicate error? */
	neg	r3,r3		/* if so, make return code negative */
1:	cmpwi	0,r3,0		/* parent or child? */
	bne	2f		/* return if parent */
	li	r0,0		/* make top-level stack frame */
	stwu	r0,-16(r1)
	mtlr	r30		/* fn addr in lr */
	mr	r3,r31		/* load arg and call fn */
	PPC440EP_ERR42
	blrl
	li	r0,__NR_exit	/* exit if function returns */
	li	r3,0
	sc
2:	lwz	r30,8(r1)
	lwz	r31,12(r1)
	addi	r1,r1,16
	blr

/*
 * This routine is just here to keep GCC happy - sigh...
 */
_GLOBAL(__main)
	blr

#ifdef CONFIG_KEXEC

/* e500mc has 64 entries in TLB1, e500vX have 16 */
#ifdef CONFIG_PPC_E500MC
#define MAS0_ESEL_MASK_MSB 10
#else
#define MAS0_ESEL_MASK_MSB 12
#endif
	/*
	 * Must be relocatable PIC code callable as a C function.
	 */
	.globl relocate_new_kernel
relocate_new_kernel:
	/* CPU0: */
	/* r3 = page_list   */
	/* r4 = reboot_code_buffer */
	/* r5 = start_address      */
	/* CPUn: */
	/* r3 = spin variable   */
	/* r4 = ready variable */
	/* r5 = address of relocate_new_kernel_secondary_spin */

#ifdef CONFIG_FSL_BOOKE
	/*
	 * Since we can't turn off the MMU, we must create an identity
	 * map for kernel low memory.  We start by invalidating the
	 * TLB entries we don't need.
	 *
	 * First, invalidate the TLB0 entries
	 */

/* p4080 might throw machine check exceptions if multiple CPUs do the tlbsync
 * concurrently: use a test-and-set mutex to control access to the
 * instruction */
#if defined(CONFIG_SMP) && defined(CONFIG_P4080_DS)
	bl	1f
1:
	mflr	r31  /* get PC */
	subi	r31, r31, 4 /* substract 1 instruction: r31 now contains
			     * relocated address of relocate_new_kernel */
	li	r30, tlbsync_corenet_lock - relocate_new_kernel
	add	r30, r30, r31 /* r30 contains relocated tlbsync_corenet_lock */
2:
	/* test-and-set lock */
	sync
	lwarx	r29, 0, r30
	cmpwi	r29, 0
	bne-	2b
	li	r29, 1
	stwcx.	r29, 0, r30
	bne-	2b
	sync
#endif
	li	r6, 0x04
	tlbivax	0, r6
#ifdef CONFIG_SMP
	tlbsync
#endif
	msync
#if defined(CONFIG_SMP) && defined(CONFIG_P4080_DS)
	/* release test-and-set lock */
	li	r29, 0
	stw	r29, 0(r30)
	sync
#endif

	/*
	 * Kernel low memory is mapped by TLB1 entries 0, 1, and 2.
	 * Preserve these, but invalidate all other TLB1 entries.
	 */
	li	r7, 3			/* first TLB1 entry */
	mfspr	r6, SPRN_TLB1CFG
	andi.	r6, r6, 0xfff
	mr	r8, r6
	subf	r6, r7, r6
	mtctr	r6

1:
	rlwinm	r6, r7, 16, MAS0_ESEL_MASK_MSB, 15
	oris	r6, r6, 0x1000
	mtspr	SPRN_MAS0, r6
	tlbre
	mfspr	r6, SPRN_MAS1
	rlwinm	r6, r6, 0, 2, 31	/* Clear MAS1 Valid and IPROT */
	mtspr	SPRN_MAS1, r6
	tlbwe
	isync
	addi	r7, r7, 1
	bdnz	1b

	/*
	 * Using TLB1 entries 3, 4, and 5, identity-map kernel low
	 * memory by copying and modifying the contents of TLB1
	 * entries 0, 1 and 2, respectively.
	 */
	li	r7, 0			/* source TLB entry */
	li	r8, 3			/* destination TLB entry */
	li	r6, 3			/* number of TLBs to copy */
	mtctr	r6
1:
	rlwinm	r6, r7, 16, MAS0_ESEL_MASK_MSB, 15
	oris	r6, r6, 0x1000
	mtspr	SPRN_MAS0, r6
	tlbre

	mfspr	r6, SPRN_MAS2
	lis	r0, PAGE_OFFSET@h
	subf	r6, r0, r6		/* identity map */
	mtspr	SPRN_MAS2, r6

	rlwinm	r6, r8, 16, MAS0_ESEL_MASK_MSB, 15
	oris	r6, r6, 0x1000
	mtspr	SPRN_MAS0, r6
	tlbwe
	sync
	isync
	addi	r7, r7, 1
	addi	r8, r8, 1
	bdnz	1b
#endif	/* CONFIG_FSL_BOOKE */

	li	r0, 0

	/*
	 * Set Machine Status Register to a known status,
	 * switch the MMU off and jump to 1: in a single step.
	 */

	mr	r8, r0
	ori     r8, r8, MSR_RI|MSR_ME
	mtspr	SPRN_SRR1, r8
#ifdef CONFIG_SMP
	mfspr	r6, SPRN_PIR
	cmpwi	r6, 0
	beq	cpu0
	addi	r8, r3, 1f - relocate_new_kernel_spin_addr
	b	all_cpus
#endif
cpu0:
	addi	r8, r4, 1f - relocate_new_kernel
all_cpus:
	mtspr	SPRN_SRR0, r8
	sync
	rfi

1:
	/* from this point address translation is turned off */
	/* and interrupts are disabled */

#ifdef CONFIG_P4080_DS
	/* uart setup for all CPUs:
	 *   map 64K@0xffe110000(RPN) in ESEL[6] to 0xfe110000(EPN) */

	#define UART_EPN	0xfe11
	#define UART_RPN	0xfe11
	#define UART_ESEL	0x1006
	#define UART_WIMGE	0x000a	/* xIxGx */
	#define UART_PERMS	0x003f	/* S+rwx, U+rwx */
	#define UART_SIZE_64K	0x0300
	#define UART_LSR	0xc505
	#define UART_IRQ	0xc501
	#define UART_THR	0xc500

	/* fetch MAS1 from ESEL0 */
	lis	r6, 0x1000
	mtspr	SPRN_MAS0, r6
	isync
	tlbre
	sync
	isync

	mfspr	r6, SPRN_MAS1
	isync

	lis	r7, 0x00ff
	ori	r7, r7, 0x1000
	and	r8, r7, r6		/* r8 contains MAS1 TID+TS */

	/* setup MAS0, ESEL6 */
	lis	r6, UART_ESEL
	mtspr	SPRN_MAS0, r6
	isync

	/* setup MAS1, ESEL6 */
	lis	r6, 0xc000		/* valid and IPROT */
	ori	r6, r6, UART_SIZE_64K	/* 64K page */
	or	r6, r6, r8		/* use same TS, TID as ESEL0 */
	mtspr	SPRN_MAS1, r6
	isync

	/* map 0xffe110000 @ 0xfe110000, cache inhibited and guarded */
	lis	r6, UART_EPN		/* EPN@h */
	ori	r6, r6, UART_WIMGE	/* EPN@l + WIMGE: xIxGx */
	mtspr	SPRN_MAS2, r6
	isync

	lis	r6, UART_RPN		/* RPN@h */
	ori	r6, r6, UART_PERMS	/* RPN@l + all permissions on */
	mtspr	SPRN_MAS3, r6
	isync

	li	r6, 0xf			/* high 4-bits of RPN in MAS7 */
	mtspr	SPRN_MAS7, r6
	isync

	tlbwe
	isync
	msync
	sync

	/* stop interrupts */
	lis	r6, UART_EPN
	ori	r6, r6, UART_IRQ
	li	r0, 0
	stb	r0, 0(r6)
	sync

	/* end - uart setup */

#endif	/* CONFIG_P4080_DS */

#ifdef CONFIG_SMP
	/* if not CPU0, jump to spin */
	mfspr	r6, SPRN_PIR
	mtlr	r5
	cmpwi	r6, 0
	bnelr
#endif

	/* set a new stack at the bottom of our page... */
	/* (not really needed now) */
	addi	r1, r4, KEXEC_CONTROL_PAGE_SIZE - 8 /* for LR Save+Back Chain */
	stw	r0, 0(r1)

	/* Do the copies */
	li	r6, 0 /* checksum */
	mr	r0, r3
	b	1f

0:	/* top, read another word for the indirection page */
	lwzu	r0, 4(r3)

1:
	/* is it a destination page? (r8) */
	rlwinm.	r7, r0, 0, 31, 31 /* IND_DESTINATION (1<<0) */
	beq	2f

	rlwinm	r8, r0, 0, 0, 19 /* clear kexec flags, page align */
	b	0b

2:	/* is it an indirection page? (r3) */
	rlwinm.	r7, r0, 0, 30, 30 /* IND_INDIRECTION (1<<1) */
	beq	2f

	rlwinm	r3, r0, 0, 0, 19 /* clear kexec flags, page align */
	subi	r3, r3, 4
	b	0b

2:	/* are we done? */
	rlwinm.	r7, r0, 0, 29, 29 /* IND_DONE (1<<2) */
	beq	2f
	b	3f

2:	/* is it a source page? (r9) */
	rlwinm.	r7, r0, 0, 28, 28 /* IND_SOURCE (1<<3) */
	beq	0b

	rlwinm	r9, r0, 0, 0, 19 /* clear kexec flags, page align */

	li	r7, PAGE_SIZE / 4
	mtctr   r7
	subi    r9, r9, 4
	subi    r8, r8, 4
9:
	lwzu    r0, 4(r9)  /* do the copy */
	xor	r6, r6, r0	/* calculate checksum (unused ?) */
	stwu    r0, 4(r8)
	dcbst	0, r8
	sync
	icbi	0, r8
	bdnz    9b

	addi    r9, r9, 4
	addi    r8, r8, 4
	b	0b

3:

	/* To be certain of avoiding problems with self-modifying code
	 * execute a serializing instruction here.
	 */
	isync
	sync

#ifdef CONFIG_SMP
	/* save the spin address in a C callee-saved register that is not
	 * trampled by purgatory (purgatory invokes some C code): at this
	 * point, r4 is holding the physical address of the control code
	 * page we're currently executing in */
	addi	r25, r4, relocate_new_kernel_spin_addr - relocate_new_kernel
#endif

	/* purgatory expects r3 to contain the boot CPU: make it happy */
	mfspr	r3, SPRN_PIR /* who are we ? */

	/* jump to the entry point, usually the purgatory */
	mtlr	r5
	blrl

1:	b	1b

#ifdef CONFIG_SMP
	.globl relocate_new_kernel_spin_addr
relocate_new_kernel_spin_addr:
	.long 0

	.globl relocate_new_kernel_ready
relocate_new_kernel_ready:
	.long 0

	.globl relocate_new_kernel_secondary_spin
relocate_new_kernel_secondary_spin:
	/* r3 contains the spin address */
	/* r4 contains the ready address */

	/* signal CPU0 that we've left the kernel */
	li r5, 0
1:
	sync	/* relax */
	lwarx	r5, 0, r4
	addi	r5, r5, 1
	stwcx.	r5, 0, r4
	bne-	1b

	/* spin waiting for a non-zero address to branch to */
1:	sync	/* relax */
	lwz	r5, 0(r3)
	cmpwi	r5, 0
	beq	1b
	/* end of spin loop */

	isync
	sync
	mtlr	r5
	blrl
	/* not reached, CPU is now on hold in the new kernel */

#endif	/* CONFIG_SMP */

#if defined(CONFIG_SMP) && defined(CONFIG_P4080_DS)
tlbsync_corenet_lock:
	.long 0
#endif

relocate_new_kernel_end:

	.globl relocate_new_kernel_size
relocate_new_kernel_size:
	.long relocate_new_kernel_end - relocate_new_kernel
#endif

#ifdef CONFIG_KEXEC_POWERPC_SMP_BOOTABLE

	.globl kexec_secondary_hold_addr
kexec_secondary_hold_addr:
	.long 0
	.globl kexec_secondary_cpu_to_wakeup
kexec_secondary_cpu_to_wakeup:
	.long 0

	.globl kexec_secondary_hold
kexec_secondary_hold:
	mfspr	r5, SPRN_PIR /* who are we ? */

	lis	r3, kexec_secondary_cpu_to_wakeup@h
	ori	r3, r3, kexec_secondary_cpu_to_wakeup@l
	subis	r3, r3, PAGE_OFFSET@h

	/* spin waiting for our CPU number to come up */
1:	sync	/* relax */
	lwz	r20, 0(r3)
	cmpw	r20, r5
	bne	1b
	/* end of spin loop */

	/* load address to branch to */
	lis	r3, kexec_secondary_hold_addr@h
	ori	r3, r3, kexec_secondary_hold_addr@l
	subis	r3, r3, PAGE_OFFSET@h
	lwz	r5, 0(r3)

	isync
	sync
	mtlr	r5
	blrl
	/* not reached, jumped to _entry_start in new kernel */

#endif 	/* CONFIG_KEXEC_POWERPC_SMP_BOOTABLE */

