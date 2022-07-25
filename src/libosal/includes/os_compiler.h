/*
 * Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
 * 
 * PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
 * the subject matter of this material. All manufacturing, reproduction,
 * use, and sales rights pertaining to this subject matter are governed
 * by the license agreement. The recipient of this software implicitly
 * accepts the terms of the license.
 * 
 * Filename:
 *  $Workfile: os_compiler.h $
 *
 * Last Modification:
 *  $Author: u010391 $
 *  $Modtime: 20.04.2011 $
 *  $Revision: 65684 $
 */
#ifndef OS_COMPILER_H_
#define OS_COMPILER_H_


/****************************************************************************************/
/**
 * @defgroup __PACKING Structure Packing
 *
 * The syntax of several protocols will be described by structures. The size of a
 * structure and the position of its members depends on the used processor.
 *
 * For example, the one byte alignment instruction forces the alignment of the
 * structure members to one. Normally the alignment of the members depend on the
 * bit size of the used processor. For example, a 32bit processor has a 4byte
 * alignment, a 64bit processor a 8byte boundary. Based on these information a
 * structure, which contains two members of type unsigned char, has a size of
 * 8 bytes on a 32 bit processor. Both members are aligned to the 4byte boundary.
 * On a 64bit processor the structure size is 8 bytes and both members are aligned
 * to the 8byte boundary.
 *
 * To force the compiler to use a one byte alignment according to the structure,
 * the structure size is always 2 bytes and the members are aligned to a one byte
 * boundary. For each structure, which will be used to exchange information with
 * remote systems, the alignment instructions have to be used.
 *
 * The following example shows a typical definition of a structure using the
 * one byte alignment instructions to provide always the same syntax.
 *
 * @code
 *
 * #if (__PACKING_PRAGMA_USE != 0)
 * #pragma __PACKING_ALIGN1_PRAGMA_ENTER(enter_example)
 * #endif
 *
 * typedef struct example_syntax_tag      example_syntax_t;
 *
 * __PACKING_ALIGN1_PRE
 * struct __PACKING_ALIGN1_POST example_syntax_tag
 * {
 *   uint16_t        value16;
 *   uint8_t         value8;
 *   uint32_t        value32;
 * };
 *
 * #if (__PACKING_PRAGMA_USE != 0)
 * #pragma __PACKING_ALIGN1_PRAGMA_LEAVE(enter_example)
 * #endif
 *
 * @endcode
 *
 *
 * The packing instructions depends on the compiler. Therefore several macros have
 * to be defined:
 *  - the macro __PACKING_PRAGMA_USE
 *  - the macro __PACKING_ALIGN1_PRAGMA_ENTER
 *  - the macro __PACKING_ALIGN1_PRAGMA_LEAVE
 *  - the macro __PACKING_ALIGN1_PRE
 *  - the macro __PACKING_ALIGN1_POST
 *
 *
 * @section Pragma Packing
 *
 * Several compilers use pragma pack directives such like the MICROSOFT compilers. In
 * this case the value of this macro __PACKING_PRAGMA_USE has to be set to 1. To enter
 * a one byte alignment section, the __PACKING_ALIGN1_PRAGMA_ENTER macro have to be set
 * to a compiler specific pragma pack directives. To leave a one byte alignment section,
 * the __PACKING_ALIGN1_PRAGMA_LEAVE macro have to be set to a compiler specific pragma
 * pack instruction. According to these pragma pack directives the __PACKING_ALIGN1_PRE
 * macro and the __PACKING_ALIGN1_POST are not required. Whenever a MICROSOFT compiler
 * will be used, the macros have to be set to the directives shown in the following
 * example:
 *
 * @code
 * #define __PACKING_PRAGMA_USE                   (1)
 * #define __PACKING_ALIGN1_PRAGMA_ENTER(lable)   pack( push, lable, 1)
 * #define __PACKING_ALIGN1_PRAGMA_LEAVE(lable)   pack( pop, lable, 1)
 * #define __PACKING_ALIGN1_PRE
 * #define __PACKING_ALIGN1_POST
 * @endcode
 *
 * The value 1 specifies a one byte alignment. The current packing alignment will be
 * stored to the compiler stack by using the pragma pack directive with the @e push
 * argument. To restore the stored packing alignment the pragma pack directive with
 * the @e pop argument has to be used.
 *
 *
 * @section Pre-packing
 *
 * Compilers, which uses a packing attribute in front of the struct directives,
 * the __PACKING_ALIGN1_PRE has to be set to the compiler specific directives.
 * In this case the macro __PACKING_PRAGMA_USE has to be set to 0. The pragma pack
 * macros __PACKING_ALIGN1_PRAGMA_ENTER and __PACKING_ALIGN1_PRAGMA_LEAVE are empty
 * just as the __PACKING_ALIGN1_POST macro.
 *
 * The REALVIEW compilers for example uses the @e __packed attribute. Whenever this
 * REALVIEW compiler will be used, the macros have to be set to the directives shown
 * in the following example:
 *
 * @code
 * #define __PACKING_PRAGMA_USE                   (0)
 * #define __PACKING_ALIGN1_PRAGMA_ENTER(lable)
 * #define __PACKING_ALIGN1_PRAGMA_LEAVE(lable)
 * #define __PACKING_ALIGN1_PRE                   __packed
 * #define __PACKING_ALIGN1_POST
 * @endcode
 *
 *
 * @section Post-packing
 *
 * Compilers such like the GNU compiler provides a packing attribute, which has to
 * be set behind the struct directives. In this case the __PACKING_ALIGN1_POST has
 * to be set to the compiler specific directives and the macro __PACKING_PRAGMA_USE
 * to 0. The macros __PACKING_ALIGN1_PRAGMA_ENTER, __PACKING_ALIGN1_PRAGMA_LEAVE and
 * __PACKING_ALIGN1_PRE are empty. Whenever a GNU compiler will be used, the macros
 * have to be set to the directives shown in the following example:
 *
 * @code
 * #define __PACKING_PRAGMA_USE                   (0)
 * #define __PACKING_ALIGN1_PRAGMA_ENTER(lable)
 * #define __PACKING_ALIGN1_PRAGMA_LEAVE(lable)
 * #define __PACKING_ALIGN1_PRE
 * #define __PACKING_ALIGN1_POST                  __attribute__((__packed__))
 * @endcode
 */

/* macro is set by the GNU compiler */
#ifdef __GNUC__
#define __PACKING_PRAGMA_USE                      (0)
#define __PACKING_ALIGN1_PRAGMA_ENTER(lable)
#define __PACKING_ALIGN1_PRAGMA_LEAVE(lable)
#define __PACKING_ALIGN1_PRE
#define __PACKING_ALIGN1_POST                     __attribute__((__packed__))

/* macro is set by the Microsoft compiler */
#elif defined _MSC_VER

#define __PACKING_PRAGMA_USE                      (1)
#define __PACKING_ALIGN1_PRAGMA_ENTER(lable)      pack(push, lable, 1)
#define __PACKING_ALIGN1_PRAGMA_LEAVE(lable)      pack(pop, lable, 1)
#define __PACKING_ALIGN1_PRE
#define __PACKING_ALIGN1_POST

/* macro is set by the RVDS compiler */
#elif defined __arm__

#define __PACKING_PRAGMA_USE                      (0)
#define __PACKING_ALIGN1_PRAGMA_ENTER(lable)
#define __PACKING_ALIGN1_PRAGMA_LEAVE(lable)
#define __PACKING_ALIGN1_PRE                      __packed
#define __PACKING_ALIGN1_POST

/* unsupported compiler */
#else
#error "There is no support for the selected compiler!"
#endif




/****************************************************************************************/
/**
 * @defgroup os_atomic Atomic References
 *
 * The atomic routines provide a simple mechanism for synchronizing access to a variable
 * that is shared by multiple threads. Each call to one of these routines are atomic with
 * respect to calls to other atomic routines.
 *
 * Atomic operations depend on the processor. Consequently the routines have to be
 * implemented according to the various series such like
 *  - ARM
 *  - x86
 *  - superH
 *  - powerPC
 *  - ...
 *
 * Because of using assembler instructions the implementations depend on the
 * compiler, too.
 */

/* GNU compiler (predefined macro) */
#if defined __GNUC__ || GCC

	/* ARM specific GNU compiler (predefined macro) */
	#if defined __arm || __arm__

		#if (__ARM_ARCH__ >= 6)

			static __inline unsigned int
			os_atomic_exchange (volatile unsigned int * const addr, unsigned int exch)
			{
			   unsigned int tmp, a;

			  /* a = *addr; *addr = exch; return a; */
			  asm volatile (
				  "1: ldrex %0, [%2]\n\t"      /* load old value of address exclusive: a = *addr */
				  "strex  %1, %3, [%2]\n\t"    /* store new value: *addr = exch */
				  "teq  %1, #0\n\t"            /* test the access right */
				  "bne  1b\n\t"                /* try again if the access to the address is denied */
				  : "=&r" (a), "=&r"(tmp)     /* %0, %1 */
				  : "r" (addr), "r" (exch)    /* %2, %3 */
				  :  "cc","memory");

			  return a;
			}

			static __inline unsigned int
			os_atomic_compare (volatile unsigned int * const addr, unsigned int exch, unsigned int comp)
			{
			  unsigned int tmp, a;

			  /* at the moment the instructions clrex is not supported by the assembler */
			  /* explicitly defined values will be used for the instructions */

			  /* a = *addr; if (a == comp){*addr = exch;} return a; */
			  asm volatile (
				  "1: ldrex %0, [%2]\n\t"      /* load old value of address exclusive: a = *addr */
				  "cmp %0, %4\n\t"             /* compare old value (a) with the comperand (comp) */
				  "strexeq  %1, %3, [%2]\n\t"  /* if these values were equal, attempt to store the new value (exch) */
				  "bne  2b\n\t"                /* if not, clear the exclusive tag and return */
				  "teq  %1, #0\n\t"            /* test the access right */
				  "bne  1b\n\t"                /* try again if the access to the address is denied */
				  "beq 3b\n\t"                 /* otherwise, return */
				  "2: dcd #0xF57FF01F"         /* clear the exclusive tag (CLREX) */
				  "3: \n\t"                    /* return */
				  : "=&r" (a), "=&r"(tmp)     /* %0, %1 */
				  : "r" (addr), "r" (exch), "r" (comp)    /* %2, %3, %4 */
				  :  "cc","memory");

			  return a;
			}

			static __inline unsigned int
			os_atomic_increment (volatile unsigned int * const addr)
			{
			  unsigned int tmp, a;

			  /* a = *addr; *addr = a + 1; return a + 1; */
			  asm volatile (
				  "1: ldrex %0, [%2]\n\t"      /* load old value of address exclusive: a = *addr */
				  "add  %0, %0, #1\n\t"        /* incremented by 1: a = a + 1 */
				  "strex  %1, %0, [%2]\n\t"    /* store new value: *addr = a */
				  "teq  %1, #0\n\t"            /* test the access right */
				  "bne  1b\n\t"                /* try again if the access to the address is denied */
				  : "=&r" (a), "=&r"(tmp)     /* %0, %1 */
				  : "r" (addr)                /* %2 */
				  :  "cc","memory");

			  return a;
			}

			static __inline unsigned int
			os_atomic_decrement (volatile unsigned int * const addr)
			{
			  unsigned int tmp, a;

			  /* a = *addr; *addr = a - 1; return a - 1; */
			  asm volatile (
				  "1: ldrex %0, [%2]\n\t"      /* load old value of address exclusive: a = *addr */
				  "sub  %0, %0, #1\n\t"        /* decrement by 1: a = a - 1 */
				  "strex  %1, %0, [%2]\n\t"    /* store new value: *addr = a */
				  "teq  %1, #0\n\t"            /* test the access right */
				  "bne  1b\n\t"                /* try again if the access to the address is denied */
				  : "=&r" (a), "=&r"(tmp)     /* %0, %1 */
				  : "r" (addr)                /* %2 */
				  :  "cc","memory");

			  return a;
			}

		#else /* (__ARM_ARCH__ <= 5) */

			static __inline unsigned int
			os_atomic_exchange (volatile unsigned int * const addr, unsigned int exch)
			{
			   unsigned int a;

			  /* a = *addr; *addr = exch; return a; */
			  asm volatile (
				  "swp %0, %2, [%1]\n\t"      /* exchange */
				  : "=&r" (a)                 /* %0 */
				  : "r" (addr), "r" (exch)    /* %1, %2 */
				  :  "cc","memory");

			  return a;
			}

			static __inline unsigned int
			os_atomic_compare (volatile unsigned int * const addr, unsigned int exch, unsigned int comp)
			{
			  unsigned int a, xcpsr, r_value;

			  /* a = *addr; if (a == comp){*addr = exch;} return a; */
			  asm volatile (
				  "mrs %1, cpsr\n\t"       /* disable flags */
				  "orr %2, %1, #0xC0\n\t"
				  "msr cpsr_c, %2\n\t"
				  "ldr %2, [%3]\n\t"            /* r_value = *addr */
				  "cmp %2, %5\n\t"              /* compare r_value and comp */
				  "streq %4, [%3]\n\t"          /* if (r_value == comp) {*addr = exch} */
				  "msr cpsr_c, %1\n\t"          /* restore flags */
				  "mov %0, %2\n\t"              /* return a (r_value) */
				  : "=&r" (a), "=&r" (xcpsr), "=&r" (r_value)   /* %0, %1, %2 */
				  : "r" (addr), "r" (exch), "r" (comp)        /* %3, %4, %5 */
				  :  "cc","memory");

			  return a;
			}

			static __inline unsigned int
			os_atomic_increment (volatile unsigned int * const addr)
			{
			  unsigned int a, xcpsr, inc_value;

			  /* a = *addr; *addr = a + 1; return a + 1; */
			  asm volatile (
				  "mrs %1, cpsr\n\t"            /* disable flags */
				  "orr %2, %1, #0xC0\n\t"
				  "msr cpsr_c, %2\n\t"
				  "ldr %2, [%3]\n\t"            /* inc_value = *addr */
				  "add %2, %2, #1\n\t"          /* inc_value++ */
				  "str %2, [%3]\n\t"            /* *addr = inc_value */
				  "msr cpsr_c, %1\n\t"          /* restore flags */
				  "mov %0, %2\n\t"              /* return a (inc_value) */
				  : "=&r" (a), "=&r" (xcpsr), "=&r" (inc_value)  /* %0, %1, %2 */
				  : "r" (addr)                                 /* %3 */
				  :  "cc","memory");

			  return a;
			}

			static __inline unsigned int
			os_atomic_decrement (volatile unsigned int * const addr)
			{
			  unsigned int a, xcpsr, inc_value;

			  /* a = *addr; *addr = a + 1; return a + 1; */
			  asm volatile (
				  "mrs %1, cpsr\n\t"            /* disable flags */
				  "orr %2, %1, #0xC0\n\t"
				  "msr cpsr_c, %2\n\t"
				  "ldr %2, [%3]\n\t"            /* inc_value = *addr */
				  "sub %2, %2, #1\n\t"          /* inc_value-- */
				  "str %2, [%3]\n\t"            /* *addr = inc_value */
				  "msr cpsr_c, %1\n\t"          /* restore flags */
				  "mov %0, %2\n\t"              /* return a (inc_value) */
				  : "=&r" (a), "=&r" (xcpsr), "=&r" (inc_value)  /* %0, %1, %2 */
				  : "r" (addr)                                 /* %3 */
				  :  "cc","memory");

			  return a;
			}

		#endif

		/* macro is set by the TMS470 compiler */
		#elif defined __TMS470__

			  static __inline unsigned int
			  os_atomic_exchange (volatile unsigned int * const addr, unsigned int exch)
			  {
				unsigned int a = 0;

				_disable_FIQ();
				/* a = *addr; *addr = exch; return a; */
				asm (" SWP  R12, R1, [R0]");
				asm (" STR  R12, [R13, #8]");
				_enable_FIQ();

				return a;
			  }

			  static __inline unsigned int
			  os_atomic_compare (volatile unsigned int * const addr, unsigned int exch, unsigned int comp)
			  {
				unsigned int oldv;

				/* retv = *addr; if (retv == comp){*addr = exch;} return retv; */
		//        asm(" MRS   R6, cpsr ");              /* get interrupt flags */
		//        asm(" ORR   retv, flags, #0xC0");        /* disable IRQ and FIQ */
		//        asm(" MSR   cpsr_c, retv");              /* set new flags */
		//        asm(" LDR   oldv, [addr]");              /* oldv = *addr */
		//        asm("cmp   oldv, comp");                /* compare oldv and comp */
		//        asm("streq exch, [addr]");              /* if (oldv == comp){*addr = exch} */
		//        asm(" MSR   cpsr_c, flags");             /* restore old flags, enable IRQ and FIQ */
		//        asm("mov   retv, oldv");               /* return retv (oldv) */

				_disable_FIQ();
				oldv = *addr;
				if (oldv == comp){*addr = exch;}
				_enable_FIQ();

				return oldv;
			  }

			  static __inline unsigned int
			  os_atomic_increment (volatile unsigned int * const addr)
			  {
				unsigned int incv;

				/* incv = *addr; incv++; *addr = incv; return incv; */
		//        asm (" MRS   R6, cpsr");               /* get interrupt flags */
		//        asm (" ORR   incv, flags, #0xC0");        /* disable IRQ and FIQ */
		//        asm (" MSR   cpsr_c, incv");              /* set new flags */
		//        asm (" LDR   incv, [addr]");              /* incv = *addr */
		//        asm (" ADD   incv, incv, #1");            /* incv++ */
		//        asm (" STR   incv, [addr]");              /* *addr = incv */
		//        asm (" MSR   cpsr_c, flags");             /* restore old flags, enable IRQ and FIQ */
				_disable_FIQ();
				incv = *addr;
				incv++;
				_enable_FIQ();

				return incv;

			  }

			  static __inline unsigned int
			  os_atomic_decrement (volatile unsigned int * const addr)
			  {
				unsigned int incv;

				/* incv = *addr; incv--; *addr = incv; return incv; */
		//        asm (" MRS   R6, cpsr");               /* get interrupt flags */
		//        asm (" ORR   incv, flags, #0xC0");        /* disable IRQ and FIQ */
		//        asm (" MSR   cpsr_c, incv");              /* set new flags */
		//        asm (" LDR   incv, [addr]");              /* incv = *addr */
		//        asm ("sub   incv, incv, #1");            /* incv-- */
		//        asm (" STR   incv, [addr]");              /* *addr = incv */
		//        asm (" MSR   cpsr_c, flags");             /* restore old flags, enable IRQ and FIQ */

				_disable_FIQ();
				incv = *addr;
				incv--;
				_enable_FIQ();

				return incv;

			  }


	/* macro is set by the ALPHA specific compiler */
	#elif defined ALPHA

	/* macro is set by the POWERPC specific compiler */
	#elif defined __ppc__

	/* macro is set by the SPARC specific compiler */
	#elif defined SPARC

	/* macro is set by the I386 specific compiler */
	#elif defined __i386__ || __I386__ || X86

	static __inline unsigned int
	os_atomic_exchange (volatile unsigned int * const addr, unsigned int exch)
	{
	  /* a = *addr; *addr = exch; return a; */
	  asm volatile("xchgl %0,%1"
				  :"=r" (exch)
				  :"m" (addr), "0" (exch)
				  :"cc","memory");

	  return exch;
	}

	static __inline unsigned int
	os_atomic_compare (volatile unsigned int * const addr, unsigned int exch, unsigned int comp)
	{
	  unsigned int a;

	  asm volatile("lock; cmpxchgl %2, %1"
				  : "=a" (a), "=m" (*addr)
				  : "r" (comp), "m" (*addr), "0" (exch)
				  :"cc","memory");

	  return a;
	}

	static __inline unsigned int
	os_atomic_increment (volatile unsigned int * const addr)
	{
	  unsigned int a;

	  /* a = *addr; *addr = a + 1; return a + 1; */
	  asm volatile("lock; xaddl %0, %1"
				  : "=r" (a), "=m" (*addr)
				  : "0" (1), "m" (*addr)
				  :"cc","memory");

	  return (a + 1);
	}

	static __inline unsigned int
	os_atomic_decrement (volatile unsigned int * const addr)
	{
	  unsigned int a;

	  /* a = *addr; *addr = a - 1; return a - 1; */
	  asm volatile("lock; xaddl %0, %1"
				  : "=r" (a), "=m" (*addr)
				  : "0" (-1), "m" (*addr)
				  :"cc","memory");

	  return (a - 1);
	}


	/* macro is set by the IA64 specific compiler */
	#elif defined IA64

	/* unsupported processor series */
#else
	#error "There is no support for the selected processor series!"
#endif

#elif defined __ARMCC_VERSION

static __inline unsigned int
os_atomic_exchange (volatile unsigned int * const addr, unsigned int exch)
{
   unsigned int a;

  /* a = *addr; *addr = exch; return a; */
  __asm
  {
     swp  a, exch, [addr]
  }

  return a;
}

static __inline unsigned int
os_atomic_compare (volatile unsigned int * const addr, unsigned int exch, unsigned int comp)
{
  unsigned int retv, oldv, flags;

  /* retv = *addr; if (retv == comp){*addr = exch;} return retv; */
  __asm
  {
    mrs   flags, cpsr               /* get interrupt flags */
    orr   retv, flags, #0xC0        /* disable IRQ and FIQ */
    msr   cpsr_c, retv              /* set new flags */
    ldr   oldv, [addr]              /* oldv = *addr */
    cmp   oldv, comp                /* compare oldv and comp */
    streq exch, [addr]              /* if (oldv == comp){*addr = exch} */
    msr   cpsr_c, flags             /* restore old flags, enable IRQ and FIQ */
    mov   retv, oldv                /* return retv (oldv) */
  }

  return retv;
}

static __inline unsigned int
os_atomic_increment (volatile unsigned int * const addr)
{
  unsigned int incv, flags;

  /* incv = *addr; incv++; *addr = incv; return incv; */
  __asm
  {
    mrs   flags, cpsr               /* get interrupt flags */
    orr   incv, flags, #0xC0        /* disable IRQ and FIQ */
    msr   cpsr_c, incv              /* set new flags */
    ldr   incv, [addr]              /* incv = *addr */
    add   incv, incv, #1            /* incv++ */
    str   incv, [addr]              /* *addr = incv */
    msr   cpsr_c, flags             /* restore old flags, enable IRQ and FIQ */
  }

  return incv;
}

static __inline unsigned int
os_atomic_decrement (volatile unsigned int * const addr)
{
  unsigned int incv, flags;

  /* incv = *addr; incv--; *addr = incv; return incv; */
  __asm
  {
    mrs   flags, cpsr               /* get interrupt flags */
    orr   incv, flags, #0xC0        /* disable IRQ and FIQ */
    msr   cpsr_c, incv              /* set new flags */
    ldr   incv, [addr]              /* incv = *addr */
    sub   incv, incv, #1            /* incv-- */
    str   incv, [addr]              /* *addr = incv */
    msr   cpsr_c, flags             /* restore old flags, enable IRQ and FIQ */
  }

  return incv;
}

/* unsupported compiler */
#else
#error "There is no support for the selected compiler!"
#endif


/****************************************************************************************/
/* end of header */
#endif /* OS_COMPILER_H_ */
