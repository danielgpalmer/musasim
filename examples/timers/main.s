#NO_APP
	.file	"main.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.align	2
	.globl	interrupthandler1
	.type	interrupthandler1, @function
interrupthandler1:
.LFB0:
	.file 1 "main.c"
	.loc 1 12 0
	.cfi_startproc
	link.w %fp,#0
.LCFI0:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 13 0
	unlk %fp
	rte
	.cfi_endproc
.LFE0:
	.size	interrupthandler1, .-interrupthandler1
	.align	2
	.globl	interrupthandler2
	.type	interrupthandler2, @function
interrupthandler2:
.LFB1:
	.loc 1 16 0
	.cfi_startproc
	link.w %fp,#0
.LCFI1:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 17 0
	unlk %fp
	rte
	.cfi_endproc
.LFE1:
	.size	interrupthandler2, .-interrupthandler2
	.align	2
	.globl	interrupthandler3
	.type	interrupthandler3, @function
interrupthandler3:
.LFB2:
	.loc 1 20 0
	.cfi_startproc
	link.w %fp,#0
.LCFI2:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 21 0
	unlk %fp
	rte
	.cfi_endproc
.LFE2:
	.size	interrupthandler3, .-interrupthandler3
	.align	2
	.globl	interrupthandler4
	.type	interrupthandler4, @function
interrupthandler4:
.LFB3:
	.loc 1 24 0
	.cfi_startproc
	link.w %fp,#0
.LCFI3:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 25 0
	unlk %fp
	rte
	.cfi_endproc
.LFE3:
	.size	interrupthandler4, .-interrupthandler4
	.align	2
	.globl	interrupthandler5
	.type	interrupthandler5, @function
interrupthandler5:
.LFB4:
	.loc 1 28 0
	.cfi_startproc
	link.w %fp,#0
.LCFI4:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 29 0
	unlk %fp
	rte
	.cfi_endproc
.LFE4:
	.size	interrupthandler5, .-interrupthandler5
	.align	2
	.globl	interrupthandler6
	.type	interrupthandler6, @function
interrupthandler6:
.LFB5:
	.loc 1 32 0
	.cfi_startproc
	link.w %fp,#0
.LCFI5:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 33 0
	unlk %fp
	rte
	.cfi_endproc
.LFE5:
	.size	interrupthandler6, .-interrupthandler6
	.align	2
	.globl	interrupthandler7
	.type	interrupthandler7, @function
interrupthandler7:
.LFB6:
	.loc 1 36 0
	.cfi_startproc
	link.w %fp,#0
.LCFI6:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 37 0
	unlk %fp
	rte
	.cfi_endproc
.LFE6:
	.size	interrupthandler7, .-interrupthandler7
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB7:
	.loc 1 39 0
	.cfi_startproc
	link.w %fp,#0
.LCFI7:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	.loc 1 41 0
	jsr timers_setup
.LVL0:
	.loc 1 43 0
	jsr machine_getstatusregister
.LVL1:
	.loc 1 44 0
	and.l #63743,%d0
.LVL2:
	move.l %d0,-(%sp)
	jsr machine_setstatusregister
.LVL3:
	addq.l #4,%sp
.L9:
	jra .L9
	.cfi_endproc
.LFE7:
	.size	main, .-main
	.text
.Letext0:
	.file 2 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/libunagipai/timers.h"
	.file 3 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/libunagipai/machine.h"
	.file 4 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.1/../../../../m68k-elf/include/stdint.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x17e
	.word	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF18
	.byte	0x1
	.long	.LASF19
	.long	.LASF20
	.long	.Ldebug_ranges0+0
	.long	0
	.long	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.long	.LASF2
	.uleb128 0x3
	.long	.LASF21
	.byte	0x4
	.byte	0x36
	.long	0x49
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF3
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.long	.LASF4
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF7
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF8
	.uleb128 0x5
	.byte	0x1
	.long	.LASF9
	.byte	0x1
	.byte	0xc
	.long	.LFB0
	.long	.LFE0
	.long	.LLST0
	.byte	0x1
	.uleb128 0x5
	.byte	0x1
	.long	.LASF10
	.byte	0x1
	.byte	0x10
	.long	.LFB1
	.long	.LFE1
	.long	.LLST1
	.byte	0x1
	.uleb128 0x5
	.byte	0x1
	.long	.LASF11
	.byte	0x1
	.byte	0x14
	.long	.LFB2
	.long	.LFE2
	.long	.LLST2
	.byte	0x1
	.uleb128 0x5
	.byte	0x1
	.long	.LASF12
	.byte	0x1
	.byte	0x18
	.long	.LFB3
	.long	.LFE3
	.long	.LLST3
	.byte	0x1
	.uleb128 0x5
	.byte	0x1
	.long	.LASF13
	.byte	0x1
	.byte	0x1c
	.long	.LFB4
	.long	.LFE4
	.long	.LLST4
	.byte	0x1
	.uleb128 0x5
	.byte	0x1
	.long	.LASF14
	.byte	0x1
	.byte	0x20
	.long	.LFB5
	.long	.LFE5
	.long	.LLST5
	.byte	0x1
	.uleb128 0x5
	.byte	0x1
	.long	.LASF15
	.byte	0x1
	.byte	0x24
	.long	.LFB6
	.long	.LFE6
	.long	.LLST6
	.byte	0x1
	.uleb128 0x6
	.byte	0x1
	.long	.LASF22
	.byte	0x1
	.byte	0x27
	.long	0x6c
	.long	.LFB7
	.long	.LFE7
	.long	.LLST7
	.byte	0x1
	.long	0x154
	.uleb128 0x7
	.string	"sr"
	.byte	0x1
	.byte	0x2b
	.long	0x3e
	.long	.LLST8
	.uleb128 0x8
	.long	.LVL0
	.long	0x154
	.uleb128 0x8
	.long	.LVL1
	.long	0x163
	.uleb128 0x8
	.long	.LVL3
	.long	0x171
	.byte	0
	.uleb128 0x9
	.byte	0x1
	.long	.LASF16
	.byte	0x2
	.byte	0xb
	.byte	0x1
	.long	0x163
	.uleb128 0xa
	.byte	0
	.uleb128 0xb
	.byte	0x1
	.long	.LASF23
	.byte	0x3
	.byte	0x15
	.byte	0x1
	.long	0x3e
	.byte	0x1
	.uleb128 0xc
	.byte	0x1
	.long	.LASF17
	.byte	0x3
	.byte	0x1a
	.byte	0x1
	.byte	0x1
	.uleb128 0xd
	.long	0x3e
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.long	.LFB0
	.long	.LCFI0
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI0
	.long	.LFE0
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST1:
	.long	.LFB1
	.long	.LCFI1
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI1
	.long	.LFE1
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST2:
	.long	.LFB2
	.long	.LCFI2
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI2
	.long	.LFE2
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST3:
	.long	.LFB3
	.long	.LCFI3
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI3
	.long	.LFE3
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST4:
	.long	.LFB4
	.long	.LCFI4
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI4
	.long	.LFE4
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST5:
	.long	.LFB5
	.long	.LCFI5
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI5
	.long	.LFE5
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST6:
	.long	.LFB6
	.long	.LCFI6
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI6
	.long	.LFE6
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST7:
	.long	.LFB7
	.long	.LCFI7
	.word	0x2
	.byte	0x7f
	.sleb128 4
	.long	.LCFI7
	.long	.LFE7
	.word	0x2
	.byte	0x7e
	.sleb128 8
	.long	0
	.long	0
.LLST8:
	.long	.LVL1
	.long	.LVL2
	.word	0x1
	.byte	0x50
	.long	0
	.long	0
	.section	.debug_aranges,"",@progbits
	.long	0x24
	.word	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.word	0
	.word	0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	.LFB7
	.long	.LFE7-.LFB7
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.Ltext0
	.long	.Letext0
	.long	.LFB7
	.long	.LFE7
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF17:
	.string	"machine_setstatusregister"
.LASF8:
	.string	"unsigned int"
.LASF16:
	.string	"timers_setup"
.LASF12:
	.string	"interrupthandler4"
.LASF1:
	.string	"unsigned char"
.LASF5:
	.string	"long unsigned int"
.LASF3:
	.string	"short unsigned int"
.LASF22:
	.string	"main"
.LASF20:
	.string	"/home/daniel/coding/musasim/examples/timers"
.LASF7:
	.string	"long long unsigned int"
.LASF6:
	.string	"long long int"
.LASF19:
	.string	"main.c"
.LASF18:
	.string	"GNU C 4.7.1"
.LASF2:
	.string	"short int"
.LASF21:
	.string	"uint16_t"
.LASF9:
	.string	"interrupthandler1"
.LASF10:
	.string	"interrupthandler2"
.LASF11:
	.string	"interrupthandler3"
.LASF4:
	.string	"long int"
.LASF13:
	.string	"interrupthandler5"
.LASF14:
	.string	"interrupthandler6"
.LASF15:
	.string	"interrupthandler7"
.LASF0:
	.string	"signed char"
.LASF23:
	.string	"machine_getstatusregister"
	.ident	"GCC: (GNU) 4.7.1"
