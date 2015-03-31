**I was totally unaware that people are actually attempting to use this thing.. but apparently some people are!! (shocked face). This stuff is totally untested on windows,.. and is very hacky. Drop me an email if you have any fixes, suggestions etc.**

To build this thing:

  * clone the main repo
  * ./bootstrap.sh
  * make

It will take a loooong time to build at first as it will build it's own local m68k toolchain. It currently isn't installable, you run it from the build directory.



**Build requirements**

sdl2
sdl2ttf
glib2
argtable2
fontconfig



# What this is #

Simply; m68k based machine simulator with GDB server based on Musashi.

I'm writing this as an emulator for a machine that I'm building.. I want to get the
hardware working nicely in software before I spend a long time writing VHDL and making
it actually work.

Currently this is the Musashi m68k emulator, source code taken from the latest zip
I could find on the interwebs (The zip is in the downloads are for anyone that wants it), plus all the stuff I could backport from MAME..
The copy of Musashi in MAME now has 040 emulation etc but is pretty MAMEified and
I can't be bothered to work out how to split it back out. I only want 000 emulation
anyhow. On top of that there is a basic machine simulator that implements address decoding logic etc and some peripherals. There is also a crappy GDB remote protocol
implementation that you can use to do debugging, the ability to dump gprof profiling data and some file format conversion tools.

# Features #
  * emulator only and gdb/debugging enabled binaries: musasim and gdbserver
  * gprof output
  * ELF binary loader

# Tools #
  * A support library called libunagipai (Taking the piss out of raspberry pi) that makes setting up DMA transfers etc easy. It also includes FATFS support for loading files off of the emulated CF card.
  * m68k-elf toolchain; There is a m68k-elf toolchain in the downloads, this is built on Debian unstable for linux/amd64,.. it's very possible it won't work for you.
  * A script that can be used to build a suitable toolchain pretty much automatically
  * Some format conversion tools (i.e. PNG -> Big Endian 16bit raw RGB)
  * I have uploaded some misc m68k related tools that have dropped off of the internet (like the m68ktester) so that they aren't lost forever.

# Emulated Hardware #

## Implemented ##
  * A "[motherboard](http://code.google.com/p/musasim/source/browse/musasim/hardware/board.c)" that has "slots" and handles mapping addresses to slots, IRQs and bus requests.
  * A "[ROM card](http://code.google.com/p/musasim/source/browse/musasim/hardware/cards/romcard.c)" that contains a ROM image that can be uploaded at startup and a lump of RAM for the machine. The ROM part of the card can be disabled and remapped to the RAM section.
  * A [dual 16550 style UART with FIFOs](http://code.google.com/p/musasim/source/browse/musasim/hardware/cards/uartcard.c) (Really horrible implementation!!) I/O with the host machine is done via PTYs.. So you can attach minicom etc. Interrupts do work.

  * [Basic 8bit framebuffer](http://code.google.com/p/musasim/source/browse/musasim/hardware/cards/basicvideo.c) based video that can fit into a Spartan 3E 500k's BlockRAM
  * [Double buffered 16bit framebuffer](http://code.google.com/p/musasim/source/browse/musasim/hardware/cards/videocard.c). There will be a command driven blitter at some point.
  * Untested 8 channel signed 16 bit sound system.
  * [1 bit PWM sound](http://code.google.com/p/musasim/source/browse/musasim/hardware/cards/basicsound.c).
  * A custom DMA controller that can do block copy, block fills and block copy + mutation (some bitwise operation with the source data and the data register).. and some mask apply modes for doing cookie cutting etc
  * An input card that takes keyboard presses and turns them into registers that the m68k can read.. the buttons translate to buttons on a Megadrive/Genesis pad.
  * Tiny bits of an IDE interface with a CF card attached. It's very hacky but it works enough for files to be read from a FAT fs.
  * Multiple (you could have as many as could fit into 2MB address space) 16bit timers with to match registers and interrupts.

## Planned ##
  * RTC

The screenshot below shows it in action;

The large terminal window is all the output coming from the simulator (configurable at compile time). The weird pattern window is the framebuffer video (using SDL), the screen has been cleared with a pattern using the DMA controller and the m68k is drawing a bouncing pixel trail over of the top of it a pixel per VBlank. The smaller window shows the m68k side pushing "Hello World!" out of the UART.


![http://wiki.musasim.googlecode.com/git/musasim.png](http://wiki.musasim.googlecode.com/git/musasim.png)

# FPGA based hardware #

A partial TG68 mc68000 softcore based version of the hardware (pictured below) exists. The video and sound hardware are much simpler due to the amount of logic required for the mc68000 core. I'll upload the VHDL source for this once it isn't so arse.

![http://wiki.musasim.googlecode.com/git/fpgapie.png](http://wiki.musasim.googlecode.com/git/fpgapie.png)