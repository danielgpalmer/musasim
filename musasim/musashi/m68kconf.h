/* ======================================================================== */
/* ========================= LICENSING & COPYRIGHT ======================== */
/* ======================================================================== */
/*
 *                                  MUSASHI
 *                                Version 3.3
 *
 * A portable Motorola M680x0 processor emulation engine.
 * Copyright 1998-2001 Karl Stenerud.  All rights reserved.
 *
 * This code may be freely used for non-commercial purposes as long as this
 * copyright notice remains unaltered in the source code and any binary files
 * containing this code in compiled form.
 *
 * All other lisencing terms must be negotiated with the author
 * (Karl Stenerud).
 *
 * The latest version of this code can be obtained at:
 * http://kstenerud.cjb.net
 */

#ifndef M68KCONF__HEADER
#define M68KCONF__HEADER

#include "../sim.h"
#include "../hardware/board.h"

#ifdef GDBSERVER
#include "../gdbserver.h"
#endif

/* Configuration switches.
 * Use OPT_SPECIFY_HANDLER for configuration options that allow callbacks.
 * OPT_SPECIFY_HANDLER causes the core to link directly to the function
 * or macro you specify, rather than using callback functions whose pointer
 * must be passed in using m68k_set_xxx_callback().
 */
#define OPT_OFF             0
#define OPT_ON              1
#define OPT_SPECIFY_HANDLER 2

/* ======================================================================== */
/* ============================== MAME STUFF ============================== */
/* ======================================================================== */

/* If you're compiling this for MAME, only change M68K_COMPILE_FOR_MAME
 * to OPT_ON and use m68kmame.h to configure the 68k core.
 */
#ifndef M68K_COMPILE_FOR_MAME
#define M68K_COMPILE_FOR_MAME      OPT_OFF
#endif /* M68K_COMPILE_FOR_MAME */

/* ======================================================================== */
/* ============================= CONFIGURATION ============================ */
/* ======================================================================== */

/* Turn on if you want to use the following M68K variants */
#define M68K_EMULATE_010            OPT_OFF
#define M68K_EMULATE_EC020          OPT_OFF
#define M68K_EMULATE_020            OPT_OFF

/* If on, the CPU will call m68k_read_immediate_xx() for immediate addressing
 * and m68k_read_pcrelative_xx() for PC-relative addressing.
 * If off, all read requests from the CPU will be redirected to m68k_read_xx()
 */
#define M68K_SEPARATE_READS         OPT_OFF

/* If on, CPU will call the interrupt acknowledge callback when it services an
 * interrupt.
 * If off, all interrupts will be autovectored and all interrupt requests will
 * auto-clear when the interrupt is serviced.
 */
#define M68K_EMULATE_INT_ACK        OPT_SPECIFY_HANDLER
#define M68K_INT_ACK_CALLBACK(A)    board_ack_interrupt(A)

/* If on, CPU will call the breakpoint acknowledge callback when it encounters
 * a breakpoint instruction and it is running a 68010+.
 */
#define M68K_EMULATE_BKPT_ACK       OPT_OFF
#define M68K_BKPT_ACK_CALLBACK()    your_bkpt_ack_handler_function()

/* If on, the CPU will monitor the trace flags and take trace exceptions
 */
#define M68K_EMULATE_TRACE          OPT_OFF

/* If on, CPU will call the output reset callback when it encounters a reset
 * instruction.
 */
#define M68K_EMULATE_RESET          OPT_SPECIFY_HANDLER
#define M68K_RESET_CALLBACK()       cpu_pulse_reset()

/* If on, CPU will call the output reset callback when it encounters a stop
 * instruction.
 */
#define M68K_EMULATE_STOP          OPT_SPECIFY_HANDLER
#ifdef GDBSERVER
#define M68K_STOP_CALLBACK()       gdb_hitstop()
#else
#define M68K_STOP_CALLBACK()       cpu_pulse_stop()
#endif

/* If on, CPU will call the set fc callback on every memory access to
 * differentiate between user/supervisor, program/data access like a real
 * 68000 would.  This should be enabled and the callback should be set if you
 * want to properly emulate the m68010 or higher. (moves uses function codes
 * to read/write data from different address spaces)
 */
#define M68K_EMULATE_FC             OPT_SPECIFY_HANDLER
#define M68K_SET_FC_CALLBACK(A)     cpu_set_fc(A)

/* If on, CPU will call the pc changed callback when it changes the PC by a
 * large value.  This allows host programs to be nicer when it comes to
 * fetching immediate data and instructions on a banked memory system.
 */

/*
 * Seems to only actually do something when interrupt handlers fire
 */

#ifdef GDBSERVER

#define M68K_MONITOR_PC             OPT_SPECIFY_HANDLER
#define M68K_SET_PC_CALLBACK(A)     gdb_onpcmodified(A)

#else

#define M68K_MONITOR_PC             OPT_OFF
#define M68K_SET_PC_CALLBACK(A)     your_pc_changed_handler_function(A)

#endif

/* If on, CPU will call the instruction hook callback before every
 * instruction.
 */

#ifdef GDBSERVER

#define M68K_INSTRUCTION_HOOK       OPT_SPECIFY_HANDLER
#define M68K_INSTRUCTION_CALLBACK() gdbserver_instruction_hook_callback()

#else

#define M68K_INSTRUCTION_HOOK       OPT_OFF
#define M68K_INSTRUCTION_CALLBACK() your_instruction_hook_function()

#endif

/* If on, the CPU will emulate the 4-byte prefetch queue of a real 68000 */
#define M68K_EMULATE_PREFETCH       OPT_ON

/* If on, the CPU will generate address error exceptions if it tries to
 * access a word or longword at an odd address.
 * NOTE: Do not enable this!  It is not working!
 */
#define M68K_EMULATE_ADDRESS_ERROR  OPT_OFF

/* Turn on to enable logging of illegal instruction calls.
 * M68K_LOG_FILEHANDLE must be #defined to a stdio file stream.
 * Turn on M68K_LOG_1010_1111 to log all 1010 and 1111 calls.
 */
#define M68K_LOG_ENABLE             OPT_OFF
#define M68K_LOG_1010_1111          OPT_OFF
#define M68K_LOG_FILEHANDLE         some_file_handle

/* ----------------------------- COMPATIBILITY ---------------------------- */

/* The following options set optimizations that violate the current ANSI
 * standard, but will be compliant under the forthcoming C9X standard.
 */

/* If on, the enulation core will use 64-bit integers to speed up some
 * operations.
 */

#ifdef __x86_64__
#define M68K_USE_64_BIT  OPT_ON
#else
#define M68K_USE_64_BIT  OPT_OFF
#endif
/* Set to your compiler's static inline keyword to enable it, or
 * set it to blank to disable it.
 * If you define INLINE in the makefile, it will override this value.
 * NOTE: not enabling inline functions will SEVERELY slow down emulation.
 */
#ifndef INLINE
#define INLINE static __inline__
#endif /* INLINE */

/* If your environment requires special prefixes for system callback functions
 * such as the argument to qsort(), then set them here or in the makefile.
 */
#ifndef DECL_SPEC
#define DECL_SPEC
#endif

#ifdef GDBSERVER

#define m68k_read_memory_8(A) gdbserver_m68k_read_byte(A)
#define m68k_read_memory_16(A) gdbserver_m68k_read_word(A)
#define m68k_read_memory_32(A) gdbserver_m68k_read_long(A)
#define m68k_write_memory_8(A, V) gdbserver_m68k_write_byte(A, V)
#define m68k_write_memory_16(A, V) gdbserver_m68k_write_word(A, V)
#define m68k_write_memory_32(A, V) gdbserver_m68k_write_long(A, V)

#else

#define m68k_read_memory_8(A) board_read_byte(A)
#define m68k_read_memory_16(A) board_read_word(A)
#define m68k_read_memory_32(A) board_read_long(A)
#define m68k_write_memory_8(A, V) board_write_byte(A, V)
#define m68k_write_memory_16(A, V) board_write_word(A, V)
#define m68k_write_memory_32(A, V) board_write_long(A, V)

#endif

/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */

#endif /* M68KCONF__HEADER */
