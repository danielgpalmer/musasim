.extern _bss_start
.extern _bss_end
.extern _text_end
.extern _data_start
.extern _data_end

.section .startup

.text

vectors:
	dc.l	0x200000
	dc.l	start		/* Start of program Code */
	dc.l	INT		/* Bus error */
	dc.l	INT 	/* Address error */
	dc.l	ILL		/* Illegal instruction */
	dc.l	INT		/* Division by zero */
	dc.l	INT		/* CHK exception */
	dc.l	INT		/* TRAPV exception */
	dc.l	INT		/* Privilage violation */
	dc.l	INT		/* TRACE exception */
	dc.l	INT		/* Line-A emulator */
	dc.l	INT		/* Line-F emulator */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Co-processor protocol violation */
	dc.l	INT		/* Format error */
	dc.l	INT		/* Uninitialized Interrupt */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Spurious Interrupt */
	dc.l	INT		/* IRQ Level 1 */
	dc.l	INT		/* IRQ Level 2 */
	dc.l	INT		/* IRQ Level 3 */
	dc.l	INT		/* IRQ Level 4 */
	dc.l	INT		/* IRQ Level 5 */
	dc.l	INT		/* IRQ Level 6 */
	dc.l	INT		/* IRQ Level 7 */
	dc.l	INT		/* TRAP #00 Exception */
	dc.l	INT		/* TRAP #01 Exception */
	dc.l	INT		/* TRAP #02 Exception */
	dc.l	INT		/* TRAP #03 Exception */
	dc.l	INT		/* TRAP #04 Exception */
	dc.l	INT		/* TRAP #05 Exception */
	dc.l	INT		/* TRAP #06 Exception */
	dc.l	INT		/* TRAP #07 Exception */
	dc.l	INT		/* TRAP #08 Exception */
	dc.l	INT		/* TRAP #09 Exception */
	dc.l	INT		/* TRAP #10 Exception */
	dc.l	INT		/* TRAP #11 Exception */
	dc.l	INT		/* TRAP #12 Exception */
	dc.l	INT		/* TRAP #13 Exception */
	dc.l	INT		/* TRAP #14 Exception */
	dc.l	INT		/* TRAP #15 Exception */
	dc.l	INT		/* (FP) Branch or Set on Unordered Condition */
	dc.l	INT		/* (FP) Inexact Result */
	dc.l	INT		/* (FP) Divide by Zero */
	dc.l	INT		/* (FP) Underflow */
	dc.l	INT		/* (FP) Operand Error */
	dc.l	INT		/* (FP) Overflow */
	dc.l	INT		/* (FP) Signaling NAN */
	dc.l	INT		/* (FP) Unimplemented Data Type */
	dc.l	INT		/* MMU Configuration Error */
	dc.l	INT		/* MMU Illegal Operation Error */
	dc.l	INT		/* MMU Access Violation Error */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */
	dc.l	INT		/* Reserved (NOT USED) */

start:

testslots: /*this should be a loop.. but something is broked with tg68 */
	move.w  #0x55AA, %d0
	lea.l   0x0,%a0 
	move.w  %d0, (%a0)
	lea.l   0x100000,%a0  
        move.w  %d0, (%a0)
	lea.l   0x200000,%a0  
        move.w  %d0, (%a0)
	lea.l   0x300000,%a0  
        move.w  %d0, (%a0)
	lea.l   0x400000,%a0  
        move.w  %d0, (%a0)	
	lea.l   0x500000,%a0  
        move.w  %d0, (%a0)
	lea.l   0x600000,%a0  
        move.w  %d0, (%a0)
	lea.l   0x700000,%a0  
        move.w  %d0, (%a0)

writeramlong:
        #this is for testing..
        lea.l   0x100000,%a0 
        move.l  #0x100100,%d0
        move.l  #0xAABBCCDD, %d1
1:      cmp.l   %d0,%a0
        beq.s   2f
        move.l  %d1, (%a0)+
        bra.s   1b
2:

writerambyte:
        lea.l   0x100100,%a0 
        move.l  #0x100200,%d0
        move.b  #0xEE, %d1
1:      cmp.l   %d0,%a0
        beq.s   2f
        move.b  %d1, (%a0)+
        bra.s   1b
2:

writeramword:
        #this is for testing..
        lea.l   0x100200,%a0 
        move.l  #0x100300,%d0
	move.w	#0x55AA, %d1
1:      cmp.l   %d0,%a0
        beq.s   2f
  	move.w	%d1, (%a0)+
        bra.s   1b
2:

registerwork:
        #this is for testing..
        lea.l   0x100300,%a0 
        move.l  #0x100400,%d0
        move.l  #0, %d1
1:      cmp.l   %d0,%a0 
        beq.s   2f
        move.b  %d1, (%a0)+
        add	#1, %d1
	bra.s   1b
2:



sramreads:
        #this is for testing..
	lea.l   0x100300,%a0 
        lea.l   0x100400,%a1
        move.l  #0x100500,%d0
1:      cmp.l   %d0,%a1
        beq.s   2f
 	move.b  (%a0)+, %d1
	not.b 	%d1
	move.b  %d1, (%a1)+
        bra.s   1b
2:

jmp sramreads

clearbss:
	#clear bss
	lea.l	_bss_start,%a0	/* Clear bss */
	move.l	#_bss_end,%d0
1:	cmp.l	%d0,%a0
	beq.s	2f
	clr.b	(%a0)+
	bra.s	1b
2:

copydata:
	#copy data

	lea.l	_text_end,%a0	/* data is located after the text */
	lea.l	_data_start,%a1 /* the data's real start address */
	move.l	#_data_end,%d0  /* the data's real end address */
1:	cmp.l	%a1,%d0		/* copy data from where ever a0 points until a1 equals the end of the data */
	beq.s	2f
	move.b	(%a0)+,(%a1)+
	bra.s	1b
2:

	jmp	main


ILL:
	stop	#0x0003

INT:
	stop	#0x0001


