#NO_APP
	.file	"main.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB1:
	.file 1 "main.c"
	.loc 1 19 0
	.cfi_startproc
	link.w %fp,#0
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	movem.l #8252,-(%sp)
	.cfi_offset 2, -28
	.cfi_offset 10, -24
	.cfi_offset 11, -20
	.cfi_offset 12, -16
	.cfi_offset 13, -12
	.loc 1 20 0
	lea video_waitforvblank,%a2
	jsr (%a2)
.LVL0:
	.loc 1 21 0
	clr.l -(%sp)
	clr.l -(%sp)
	jsr video_setconfig
.LVL1:
	addq.l #8,%sp
	move.l #video_begin,%d2
	lea rect_drawrect,%a5
	lea video_commit,%a4
	lea video_flip,%a3
.L2:
.LBB4:
.LBB5:
	.loc 1 12 0 discriminator 1
	move.l %d2,%a0
	jsr (%a0)
.LVL2:
	.loc 1 13 0 discriminator 1
	move.l #65535,-(%sp)
	pea p0up
	jsr (%a5)
.LVL3:
	.loc 1 14 0 discriminator 1
	jsr (%a4)
.LVL4:
	.loc 1 15 0 discriminator 1
	jsr (%a2)
.LVL5:
	.loc 1 16 0 discriminator 1
	jsr (%a3)
.LVL6:
	addq.l #8,%sp
	.loc 1 12 0 discriminator 1
	move.l %d2,%a0
	jsr (%a0)
.LVL7:
	.loc 1 13 0 discriminator 1
	move.l #65535,-(%sp)
	pea p0up
	jsr (%a5)
.LVL8:
	.loc 1 14 0 discriminator 1
	jsr (%a4)
.LVL9:
	.loc 1 15 0 discriminator 1
	jsr (%a2)
.LVL10:
	.loc 1 16 0 discriminator 1
	jsr (%a3)
.LVL11:
	addq.l #8,%sp
	jra .L2
.LBE5:
.LBE4:
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.data
	.align	2
	.type	p0up, @object
	.size	p0up, 16
p0up:
	.long	10
	.long	10
	.long	50
	.long	50
	.text
.Letext0:
	.file 2 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/m68k-elf/include/libunagipai/rect.h"
	.file 3 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/m68k-elf/include/libunagipai/video.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x12d
	.word	0x4
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF12
	.byte	0x1
	.long	.LASF13
	.long	.LASF14
	.long	.Ldebug_ranges0+0
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
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF8
	.uleb128 0x4
	.byte	0x10
	.byte	0x2
	.byte	0xd
	.long	0xa0
	.uleb128 0x5
	.string	"x"
	.byte	0x2
	.byte	0xe
	.long	0x5d
	.byte	0
	.uleb128 0x5
	.string	"y"
	.byte	0x2
	.byte	0xf
	.long	0x5d
	.byte	0x4
	.uleb128 0x6
	.long	.LASF9
	.byte	0x2
	.byte	0x10
	.long	0x5d
	.byte	0x8
	.uleb128 0x6
	.long	.LASF10
	.byte	0x2
	.byte	0x11
	.long	0x5d
	.byte	0xc
	.byte	0
	.uleb128 0x7
	.long	.LASF15
	.byte	0x2
	.byte	0x12
	.long	0x6b
	.uleb128 0x8
	.long	.LASF16
	.byte	0x1
	.byte	0xb
	.byte	0x1
	.uleb128 0x9
	.long	.LASF17
	.byte	0x1
	.byte	0x13
	.long	0x5d
	.long	.LFB1
	.long	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.long	0x102
	.uleb128 0xa
	.long	0xab
	.long	.LBB4
	.long	.LBE4-.LBB4
	.byte	0x1
	.byte	0x18
	.long	0xf0
	.uleb128 0xb
	.long	.LVL2
	.uleb128 0x2
	.byte	0x72
	.sleb128 0
	.uleb128 0xb
	.long	.LVL7
	.uleb128 0x2
	.byte	0x72
	.sleb128 0
	.byte	0
	.uleb128 0xb
	.long	.LVL0
	.uleb128 0x2
	.byte	0x7a
	.sleb128 0
	.uleb128 0xc
	.long	.LVL1
	.long	0x113
	.byte	0
	.uleb128 0xd
	.long	.LASF18
	.byte	0x1
	.byte	0x8
	.long	0xa0
	.uleb128 0x5
	.byte	0x3
	.long	p0up
	.uleb128 0xe
	.long	.LASF19
	.byte	0x3
	.byte	0x39
	.long	0x129
	.uleb128 0xf
	.long	0x129
	.uleb128 0xf
	.long	0x129
	.byte	0
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.long	.LASF11
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
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
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
	.uleb128 0x4
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x7
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
	.uleb128 0x8
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x20
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
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
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2116
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x2113
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.word	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.word	0
	.word	0
	.long	.LFB1
	.long	.LFE1-.LFB1
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.LFB1
	.long	.LFE1
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF16:
	.string	"update"
.LASF14:
	.string	"/home/daniel/coding/musasim/examples/input"
.LASF1:
	.string	"unsigned char"
.LASF5:
	.string	"long unsigned int"
.LASF3:
	.string	"short unsigned int"
.LASF10:
	.string	"height"
.LASF19:
	.string	"video_setconfig"
.LASF17:
	.string	"main"
.LASF18:
	.string	"p0up"
.LASF8:
	.string	"unsigned int"
.LASF7:
	.string	"long long unsigned int"
.LASF15:
	.string	"rect"
.LASF6:
	.string	"long long int"
.LASF13:
	.string	"main.c"
.LASF9:
	.string	"width"
.LASF2:
	.string	"short int"
.LASF12:
	.string	"GNU C 4.8.1 -fpreprocessed -mcpu=68000 -ggdb -O2 -std=gnu99"
.LASF4:
	.string	"long int"
.LASF0:
	.string	"signed char"
.LASF11:
	.string	"_Bool"
	.ident	"GCC: (GNU) 4.8.1"
