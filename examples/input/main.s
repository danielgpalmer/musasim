#NO_APP
	.file	"main.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.align	2
	.type	update, @function
update:
.LFB0:
	.file 1 "main.c"
	.loc 1 37 0
	.cfi_startproc
	link.w %fp,#0
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	movem.l #8240,-(%sp)
	.cfi_offset 2, -20
	.cfi_offset 10, -16
	.cfi_offset 11, -12
	.loc 1 38 0
	jsr video_begin
.LVL0:
	lea p0,%a2
.LBB2:
	.loc 1 40 0
	moveq #0,%d2
	lea rect_drawrect,%a3
.LVL1:
.L4:
	.loc 1 41 0
	moveq #0,%d1
	move.b port0,%d1
	moveq #0,%d0
	not.w %d0
	btst %d2,%d1
	jne .L2
	move.w #-2048,%d0
.L2:
	.loc 1 41 0 is_stmt 0 discriminator 3
	move.l %d0,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a3)
.LVL2:
	.loc 1 40 0 is_stmt 1 discriminator 3
	addq.l #1,%d2
.LVL3:
	lea (16,%a2),%a2
	addq.l #8,%sp
	moveq #8,%d0
	cmp.l %d2,%d0
	jne .L4
	lea p1,%a2
	.loc 1 40 0 is_stmt 0
	moveq #0,%d2
.LVL4:
.L7:
.LBE2:
.LBB3:
	.loc 1 45 0 is_stmt 1
	moveq #0,%d1
	move.b port1,%d1
	moveq #0,%d0
	not.w %d0
	btst %d2,%d1
	jne .L5
	move.w #-2048,%d0
.L5:
	.loc 1 45 0 is_stmt 0 discriminator 3
	move.l %d0,-(%sp)
	move.l %a2,-(%sp)
	jsr (%a3)
.LVL5:
	.loc 1 44 0 is_stmt 1 discriminator 3
	addq.l #1,%d2
.LVL6:
	lea (16,%a2),%a2
	addq.l #8,%sp
	moveq #8,%d0
	cmp.l %d2,%d0
	jne .L7
.LBE3:
	.loc 1 49 0
	movem.l -12(%fp),#3076
.LVL7:
	unlk %fp
	.loc 1 48 0
	jra video_commit
.LVL8:
	.cfi_endproc
.LFE0:
	.size	update, .-update
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Starting input test"
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB1:
	.loc 1 51 0
	.cfi_startproc
	link.w %fp,#0
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	movem.l #56,-(%sp)
	.cfi_offset 10, -20
	.cfi_offset 11, -16
	.cfi_offset 12, -12
	.loc 1 53 0
	pea .LC0
	jsr puts
.LVL9:
	.loc 1 54 0
	lea video_waitforvblank,%a2
	jsr (%a2)
.LVL10:
	.loc 1 55 0
	clr.l -(%sp)
	clr.l -(%sp)
	jsr video_setconfig
.LVL11:
	.loc 1 57 0
	lea update,%a4
	jsr (%a4)
.LVL12:
	.loc 1 58 0
	jsr (%a2)
.LVL13:
	.loc 1 59 0
	lea video_flip,%a3
	jsr (%a3)
.LVL14:
	lea (12,%sp),%sp
.L13:
.LBB4:
	.loc 1 62 0
	move.b 14680064,%d0
.LVL15:
	.loc 1 63 0
	move.b 14680066,%d1
.LVL16:
	.loc 1 64 0
	cmp.b port0.l,%d0
	jne .L14
	.loc 1 64 0 is_stmt 0 discriminator 1
	cmp.b port1.l,%d1
	jeq .L15
.L14:
	.loc 1 65 0 is_stmt 1
	move.b %d0,port0
	.loc 1 66 0
	move.b %d1,port1
	.loc 1 67 0
	jsr (%a4)
.LVL17:
	.loc 1 68 0
	jsr (%a2)
.LVL18:
	.loc 1 69 0
	jsr (%a3)
.LVL19:
	jra .L13
.LVL20:
.L15:
	.loc 1 72 0
	jsr (%a2)
.LVL21:
	jra .L13
.LBE4:
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.data
	.type	port1, @object
	.size	port1, 1
port1:
	.byte	-1
	.type	port0, @object
	.size	port0, 1
port0:
	.byte	-1
	.align	2
	.type	p1, @object
	.size	p1, 128
p1:
	.long	10
	.long	10
	.long	50
	.long	50
	.long	10
	.long	10
	.long	50
	.long	50
	.long	10
	.long	10
	.long	50
	.long	50
	.long	10
	.long	10
	.long	50
	.long	50
	.long	10
	.long	10
	.long	50
	.long	50
	.long	10
	.long	10
	.long	50
	.long	50
	.long	10
	.long	10
	.long	50
	.long	50
	.long	10
	.long	10
	.long	50
	.long	50
	.align	2
	.type	p0, @object
	.size	p0, 128
p0:
	.long	100
	.long	100
	.long	20
	.long	20
	.long	65
	.long	100
	.long	20
	.long	20
	.long	40
	.long	100
	.long	20
	.long	20
	.long	50
	.long	40
	.long	20
	.long	20
	.long	75
	.long	40
	.long	20
	.long	20
	.long	90
	.long	40
	.long	20
	.long	20
	.long	110
	.long	40
	.long	20
	.long	20
	.long	135
	.long	40
	.long	20
	.long	20
	.text
.Letext0:
	.file 2 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/m68k-elf/include/libunagipai/rect.h"
	.file 3 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/m68k-elf/include/stdint.h"
	.file 4 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/m68k-elf/include/libunagipai/video.h"
	.file 5 "<built-in>"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x24a
	.word	0x4
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF24
	.byte	0x1
	.long	.LASF25
	.long	.LASF26
	.long	.Ldebug_ranges0+0
	.long	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF0
	.uleb128 0x3
	.long	.LASF11
	.byte	0x3
	.byte	0x2a
	.long	0x37
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
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF8
	.uleb128 0x5
	.byte	0x10
	.byte	0x2
	.byte	0xd
	.long	0xab
	.uleb128 0x6
	.string	"x"
	.byte	0x2
	.byte	0xe
	.long	0x68
	.byte	0
	.uleb128 0x6
	.string	"y"
	.byte	0x2
	.byte	0xf
	.long	0x68
	.byte	0x4
	.uleb128 0x7
	.long	.LASF9
	.byte	0x2
	.byte	0x10
	.long	0x68
	.byte	0x8
	.uleb128 0x7
	.long	.LASF10
	.byte	0x2
	.byte	0x11
	.long	0x68
	.byte	0xc
	.byte	0
	.uleb128 0x3
	.long	.LASF12
	.byte	0x2
	.byte	0x12
	.long	0x76
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF13
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF14
	.uleb128 0x8
	.long	.LASF27
	.byte	0x1
	.byte	0x25
	.long	.LFB0
	.long	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.long	0x122
	.uleb128 0x9
	.long	.LBB2
	.long	.LBE2-.LBB2
	.long	0xf4
	.uleb128 0xa
	.string	"r"
	.byte	0x1
	.byte	0x28
	.long	0x68
	.long	.LLST0
	.byte	0
	.uleb128 0x9
	.long	.LBB3
	.long	.LBE3-.LBB3
	.long	0x10f
	.uleb128 0xa
	.string	"r"
	.byte	0x1
	.byte	0x2c
	.long	0x68
	.long	.LLST1
	.byte	0
	.uleb128 0xb
	.long	.LVL0
	.long	0x209
	.uleb128 0xc
	.long	.LVL8
	.long	0x210
	.byte	0
	.uleb128 0xd
	.long	.LASF21
	.byte	0x1
	.byte	0x33
	.long	.LFB1
	.long	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.long	0x1ac
	.uleb128 0xe
	.long	.LASF28
	.byte	0x5
	.byte	0
	.long	0x68
	.long	0x14d
	.uleb128 0xf
	.long	0x1ac
	.uleb128 0x10
	.byte	0
	.uleb128 0x9
	.long	.LBB4
	.long	.LBE4-.LBB4
	.long	0x179
	.uleb128 0x11
	.long	.LASF15
	.byte	0x1
	.byte	0x3e
	.long	0x2c
	.long	.LLST2
	.uleb128 0x11
	.long	.LASF16
	.byte	0x1
	.byte	0x3f
	.long	0x2c
	.long	.LLST3
	.byte	0
	.uleb128 0xb
	.long	.LVL9
	.long	0x217
	.uleb128 0x12
	.long	.LVL10
	.uleb128 0x2
	.byte	0x7a
	.sleb128 0
	.uleb128 0xb
	.long	.LVL11
	.long	0x230
	.uleb128 0x12
	.long	.LVL12
	.uleb128 0x2
	.byte	0x7c
	.sleb128 0
	.uleb128 0x12
	.long	.LVL13
	.uleb128 0x2
	.byte	0x7a
	.sleb128 0
	.uleb128 0x12
	.long	.LVL14
	.uleb128 0x2
	.byte	0x7b
	.sleb128 0
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.long	0x1b2
	.uleb128 0x14
	.long	0xbd
	.uleb128 0x15
	.long	0xab
	.long	0x1c7
	.uleb128 0x16
	.long	0xb6
	.byte	0x7
	.byte	0
	.uleb128 0x17
	.string	"p0"
	.byte	0x1
	.byte	0xe
	.long	0x1b7
	.uleb128 0x5
	.byte	0x3
	.long	p0
	.uleb128 0x17
	.string	"p1"
	.byte	0x1
	.byte	0x18
	.long	0x1b7
	.uleb128 0x5
	.byte	0x3
	.long	p1
	.uleb128 0x18
	.long	.LASF17
	.byte	0x1
	.byte	0x22
	.long	0x2c
	.uleb128 0x5
	.byte	0x3
	.long	port0
	.uleb128 0x18
	.long	.LASF18
	.byte	0x1
	.byte	0x23
	.long	0x2c
	.uleb128 0x5
	.byte	0x3
	.long	port1
	.uleb128 0x19
	.long	.LASF19
	.byte	0x4
	.byte	0x16
	.uleb128 0x19
	.long	.LASF20
	.byte	0x4
	.byte	0x1c
	.uleb128 0x1a
	.long	.LASF22
	.byte	0x5
	.byte	0
	.long	.LASF29
	.long	0x68
	.long	0x230
	.uleb128 0xf
	.long	0x1ac
	.byte	0
	.uleb128 0x1b
	.long	.LASF30
	.byte	0x4
	.byte	0x39
	.long	0x246
	.uleb128 0xf
	.long	0x246
	.uleb128 0xf
	.long	0x246
	.byte	0
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.long	.LASF23
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
	.uleb128 0x6
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
	.uleb128 0x7
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
	.uleb128 0x8
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
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
	.uleb128 0x9
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
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
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x2115
	.uleb128 0x19
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
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
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x10
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x11
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
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x2113
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x17
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
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x18
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
	.uleb128 0x19
	.uleb128 0x2e
	.byte	0
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
	.byte	0
	.byte	0
	.uleb128 0x1a
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
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
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
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.long	.LVL0
	.long	.LVL1
	.word	0x2
	.byte	0x30
	.byte	0x9f
	.long	.LVL1
	.long	.LVL4
	.word	0x1
	.byte	0x52
	.long	0
	.long	0
.LLST1:
	.long	.LVL4
	.long	.LVL7
	.word	0x1
	.byte	0x52
	.long	0
	.long	0
.LLST2:
	.long	.LVL15
	.long	.LVL17-1
	.word	0x1
	.byte	0x50
	.long	.LVL20
	.long	.LVL21-1
	.word	0x1
	.byte	0x50
	.long	0
	.long	0
.LLST3:
	.long	.LVL16
	.long	.LVL17-1
	.word	0x1
	.byte	0x51
	.long	.LVL20
	.long	.LVL21-1
	.word	0x1
	.byte	0x51
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
	.long	.LFB1
	.long	.LFE1-.LFB1
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.Ltext0
	.long	.Letext0
	.long	.LFB1
	.long	.LFE1
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF22:
	.string	"__builtin_puts"
.LASF17:
	.string	"port0"
.LASF18:
	.string	"port1"
.LASF2:
	.string	"short int"
.LASF13:
	.string	"sizetype"
.LASF21:
	.string	"main"
.LASF26:
	.string	"/home/daniel/coding/musasim/examples/input"
.LASF11:
	.string	"uint8_t"
.LASF9:
	.string	"width"
.LASF6:
	.string	"long long int"
.LASF27:
	.string	"update"
.LASF4:
	.string	"long int"
.LASF28:
	.string	"printf"
.LASF1:
	.string	"unsigned char"
.LASF24:
	.string	"GNU C 4.8.1 -fpreprocessed -mcpu=68000 -ggdb -O2 -std=gnu99"
.LASF0:
	.string	"signed char"
.LASF7:
	.string	"long long unsigned int"
.LASF19:
	.string	"video_begin"
.LASF10:
	.string	"height"
.LASF8:
	.string	"unsigned int"
.LASF29:
	.string	"puts"
.LASF3:
	.string	"short unsigned int"
.LASF20:
	.string	"video_commit"
.LASF14:
	.string	"char"
.LASF23:
	.string	"_Bool"
.LASF25:
	.string	"main.c"
.LASF5:
	.string	"long unsigned int"
.LASF30:
	.string	"video_setconfig"
.LASF12:
	.string	"rect"
.LASF15:
	.string	"newport0"
.LASF16:
	.string	"newport1"
	.ident	"GCC: (GNU) 4.8.1"
