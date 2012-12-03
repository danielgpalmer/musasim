#NO_APP
	.file	"main.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"kick1.as2"
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
.LFB0:
	.file 1 "main.c"
	.loc 1 14 0
	.cfi_startproc
	link.w %fp,#-108
.LCFI0:
	.cfi_offset 14, -8
	.cfi_def_cfa 14, 8
	move.l %a3,-(%sp)
	move.l %a2,-(%sp)
	.cfi_offset 11, -120
	.cfi_offset 10, -124
	.loc 1 17 0
	lea (-106,%fp),%a3
	move.l %a3,-(%sp)
	jsr pf_mount
.LVL0:
	.loc 1 18 0
	move.l %d0,-(%sp)
	lea util_printffresult,%a2
	jsr (%a2)
.LVL1:
	.loc 1 19 0
	move.l %a3,-(%sp)
	jsr util_printfat
.LVL2:
	.loc 1 20 0
	pea .LC0
	jsr pf_open
.LVL3:
	.loc 1 21 0
	move.l %d0,-(%sp)
	jsr (%a2)
.LVL4:
	.loc 1 24 0
	pea -108(%fp)
	pea 63.w
	lea (-64,%fp),%a2
	move.l %a2,-(%sp)
	jsr pf_read
.LVL5:
	.loc 1 26 0
	lea (32,%sp),%sp
	moveq #0,%d0
.LVL6:
	move.w -108(%fp),%d0
.LVL7:
	move.l %d0,-(%sp)
	clr.l -(%sp)
	clr.l -(%sp)
	move.l %a2,-(%sp)
	jsr sound_uploadsample
.LVL8:
	lea (16,%sp),%sp
.L2:
	jra .L2
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.text
.Letext0:
	.file 2 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/stdint.h"
	.file 3 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/pff.h"
	.file 4 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/utils.h"
	.file 5 "/home/daniel/coding/musasim/toolchains/inst/m68k-elf/lib/gcc/m68k-elf/4.7.2/../../../../m68k-elf/include/libunagipai/sound.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x2fa
	.word	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF42
	.byte	0x1
	.long	.LASF43
	.long	.LASF44
	.long	.Ldebug_ranges0+0
	.long	0
	.long	0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF0
	.uleb128 0x3
	.long	.LASF3
	.byte	0x2
	.byte	0x2a
	.long	0x3b
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.long	.LASF2
	.uleb128 0x3
	.long	.LASF4
	.byte	0x2
	.byte	0x36
	.long	0x54
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF5
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.long	.LASF6
	.uleb128 0x3
	.long	.LASF7
	.byte	0x2
	.byte	0x50
	.long	0x6d
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF8
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF9
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF10
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF11
	.uleb128 0x5
	.byte	0x2a
	.byte	0x3
	.byte	0x45
	.long	0x15d
	.uleb128 0x6
	.long	.LASF12
	.byte	0x3
	.byte	0x46
	.long	0x30
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x6
	.long	.LASF13
	.byte	0x3
	.byte	0x47
	.long	0x30
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x6
	.long	.LASF14
	.byte	0x3
	.byte	0x48
	.long	0x30
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x6
	.long	.LASF15
	.byte	0x3
	.byte	0x49
	.long	0x30
	.byte	0x2
	.byte	0x23
	.uleb128 0x3
	.uleb128 0x6
	.long	.LASF16
	.byte	0x3
	.byte	0x4a
	.long	0x49
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x6
	.long	.LASF17
	.byte	0x3
	.byte	0x4b
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x6
	.uleb128 0x6
	.long	.LASF18
	.byte	0x3
	.byte	0x4c
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0xa
	.uleb128 0x6
	.long	.LASF19
	.byte	0x3
	.byte	0x4d
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0xe
	.uleb128 0x6
	.long	.LASF20
	.byte	0x3
	.byte	0x4e
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x12
	.uleb128 0x6
	.long	.LASF21
	.byte	0x3
	.byte	0x4f
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x16
	.uleb128 0x6
	.long	.LASF22
	.byte	0x3
	.byte	0x50
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x1a
	.uleb128 0x6
	.long	.LASF23
	.byte	0x3
	.byte	0x51
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x1e
	.uleb128 0x6
	.long	.LASF24
	.byte	0x3
	.byte	0x52
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x22
	.uleb128 0x6
	.long	.LASF25
	.byte	0x3
	.byte	0x53
	.long	0x62
	.byte	0x2
	.byte	0x23
	.uleb128 0x26
	.byte	0
	.uleb128 0x3
	.long	.LASF26
	.byte	0x3
	.byte	0x54
	.long	0x90
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF27
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF28
	.uleb128 0x7
	.byte	0x4
	.byte	0x3
	.byte	0x6c
	.long	0x1af
	.uleb128 0x8
	.long	.LASF29
	.sleb128 0
	.uleb128 0x8
	.long	.LASF30
	.sleb128 1
	.uleb128 0x8
	.long	.LASF31
	.sleb128 2
	.uleb128 0x8
	.long	.LASF32
	.sleb128 3
	.uleb128 0x8
	.long	.LASF33
	.sleb128 4
	.uleb128 0x8
	.long	.LASF34
	.sleb128 5
	.uleb128 0x8
	.long	.LASF35
	.sleb128 6
	.uleb128 0x8
	.long	.LASF36
	.sleb128 7
	.byte	0
	.uleb128 0x3
	.long	.LASF37
	.byte	0x3
	.byte	0x75
	.long	0x176
	.uleb128 0x9
	.byte	0x1
	.long	.LASF45
	.byte	0x1
	.byte	0xe
	.long	0x82
	.long	.LFB0
	.long	.LFE0
	.long	.LLST0
	.byte	0x1
	.long	0x24f
	.uleb128 0xa
	.string	"fs"
	.byte	0x1
	.byte	0xf
	.long	0x15d
	.byte	0x3
	.byte	0x7e
	.sleb128 -106
	.uleb128 0xb
	.long	.LASF38
	.byte	0x1
	.byte	0x10
	.long	0x1af
	.long	.LLST1
	.uleb128 0xa
	.string	"buf"
	.byte	0x1
	.byte	0x16
	.long	0x24f
	.byte	0x2
	.byte	0x7e
	.sleb128 -64
	.uleb128 0xc
	.string	"len"
	.byte	0x1
	.byte	0x17
	.long	0x49
	.long	.LLST2
	.uleb128 0xd
	.long	.LVL0
	.long	0x25f
	.uleb128 0xe
	.long	.LVL1
	.byte	0x2
	.byte	0x7a
	.sleb128 0
	.uleb128 0xd
	.long	.LVL2
	.long	0x27d
	.uleb128 0xd
	.long	.LVL3
	.long	0x291
	.uleb128 0xe
	.long	.LVL4
	.byte	0x2
	.byte	0x7a
	.sleb128 0
	.uleb128 0xd
	.long	.LVL5
	.long	0x2b4
	.uleb128 0xd
	.long	.LVL8
	.long	0x2de
	.byte	0
	.uleb128 0xf
	.long	0x30
	.long	0x25f
	.uleb128 0x10
	.long	0x168
	.byte	0x3f
	.byte	0
	.uleb128 0x11
	.byte	0x1
	.long	.LASF39
	.byte	0x3
	.byte	0x7a
	.byte	0x1
	.long	0x1af
	.byte	0x1
	.long	0x277
	.uleb128 0x12
	.long	0x277
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.long	0x15d
	.uleb128 0x14
	.byte	0x1
	.long	.LASF46
	.byte	0x4
	.byte	0x11
	.byte	0x1
	.byte	0x1
	.long	0x291
	.uleb128 0x12
	.long	0x277
	.byte	0
	.uleb128 0x11
	.byte	0x1
	.long	.LASF40
	.byte	0x3
	.byte	0x7b
	.byte	0x1
	.long	0x1af
	.byte	0x1
	.long	0x2a9
	.uleb128 0x12
	.long	0x2a9
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.long	0x2af
	.uleb128 0x15
	.long	0x16f
	.uleb128 0x11
	.byte	0x1
	.long	.LASF41
	.byte	0x3
	.byte	0x7c
	.byte	0x1
	.long	0x1af
	.byte	0x1
	.long	0x2d6
	.uleb128 0x12
	.long	0x2d6
	.uleb128 0x12
	.long	0x49
	.uleb128 0x12
	.long	0x2d8
	.byte	0
	.uleb128 0x16
	.byte	0x4
	.uleb128 0x13
	.byte	0x4
	.long	0x49
	.uleb128 0x17
	.byte	0x1
	.long	.LASF47
	.byte	0x5
	.byte	0xa
	.byte	0x1
	.byte	0x1
	.uleb128 0x12
	.long	0x2d8
	.uleb128 0x12
	.long	0x89
	.uleb128 0x12
	.long	0x49
	.uleb128 0x12
	.long	0x49
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
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x4
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
	.uleb128 0x8
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
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
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xb
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
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0xc
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
	.uleb128 0xd
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x2113
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x11
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
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x17
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
	.long	.LVL0
	.long	.LVL1-1
	.word	0x1
	.byte	0x50
	.long	.LVL3
	.long	.LVL4-1
	.word	0x1
	.byte	0x50
	.long	0
	.long	0
.LLST2:
	.long	.LVL6
	.long	.LVL7
	.word	0x3
	.byte	0x91
	.sleb128 -116
	.long	.LVL7
	.long	.LVL8-1
	.word	0x1
	.byte	0x50
	.long	0
	.long	0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.word	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0
	.word	0
	.word	0
	.long	.LFB0
	.long	.LFE0-.LFB0
	.long	0
	.long	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.long	.LFB0
	.long	.LFE0
	.long	0
	.long	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF15:
	.string	"pad1"
.LASF14:
	.string	"csize"
.LASF37:
	.string	"FRESULT"
.LASF21:
	.string	"fptr"
.LASF17:
	.string	"n_fatent"
.LASF39:
	.string	"pf_mount"
.LASF33:
	.string	"FR_NO_PATH"
.LASF29:
	.string	"FR_OK"
.LASF16:
	.string	"n_rootdir"
.LASF12:
	.string	"fs_type"
.LASF19:
	.string	"dirbase"
.LASF31:
	.string	"FR_NOT_READY"
.LASF30:
	.string	"FR_DISK_ERR"
.LASF1:
	.string	"unsigned char"
.LASF24:
	.string	"curr_clust"
.LASF8:
	.string	"long unsigned int"
.LASF5:
	.string	"short unsigned int"
.LASF20:
	.string	"database"
.LASF18:
	.string	"fatbase"
.LASF7:
	.string	"uint32_t"
.LASF34:
	.string	"FR_NOT_OPENED"
.LASF26:
	.string	"FATFS"
.LASF32:
	.string	"FR_NO_FILE"
.LASF13:
	.string	"flag"
.LASF45:
	.string	"main"
.LASF11:
	.string	"unsigned int"
.LASF10:
	.string	"long long unsigned int"
.LASF3:
	.string	"uint8_t"
.LASF38:
	.string	"result"
.LASF41:
	.string	"pf_read"
.LASF35:
	.string	"FR_NOT_ENABLED"
.LASF36:
	.string	"FR_NO_FILESYSTEM"
.LASF27:
	.string	"sizetype"
.LASF46:
	.string	"util_printfat"
.LASF9:
	.string	"long long int"
.LASF43:
	.string	"main.c"
.LASF28:
	.string	"char"
.LASF22:
	.string	"fsize"
.LASF42:
	.string	"GNU C 4.7.2"
.LASF2:
	.string	"short int"
.LASF4:
	.string	"uint16_t"
.LASF25:
	.string	"dsect"
.LASF44:
	.string	"/home/daniel/coding/musasim/examples/sound"
.LASF6:
	.string	"long int"
.LASF47:
	.string	"sound_uploadsample"
.LASF0:
	.string	"signed char"
.LASF23:
	.string	"org_clust"
.LASF40:
	.string	"pf_open"
	.ident	"GCC: (GNU) 4.7.2"
