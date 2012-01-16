MEMORY 
{
        flash                           : ORIGIN = 0,        	LENGTH = 1024K    /* ROM */
        ram                             : ORIGIN = 0x100000, 	LENGTH = 1024k    /* RAM */
}

SECTIONS 
{
	. = 0;

        .startup : 
	{ 
		*(.text)
	} >flash

	. = ALIGN(2);

        .text :                                                          
        {
                *(.text)                                                
                *(.rodata)                                              /* all .rodata sections (constants, strings, etc.)  */
                *(.rodata*)                                             /* all .rodata* sections (constants, strings, etc.)  */
		*(.eh_frame)
		. = ALIGN(2);
		_text_end = .;                                             /* define a global symbol _etext just after the last code byte */
	} >flash                                                        /* put all the above into FLASH */

        .data :                                                         /* collect all initialized .data sections that go into RAM  */ 
        {
                _data_start = .;                                              /* create a global symbol marking the start of the .data section  */
                *(.data)                                                /* all .data sections  */
                _data_end = .;                                             /* define a global symbol marking the end of the .data section  */
        } >ram AT >flash                                        /* put all the above into RAM (but load the LMA copy into FLASH) */
	. = ALIGN(2);
	
        .bss :                                                          /* collect all uninitialized .bss sections that go into RAM  */
        {
                _bss_start = .;                                 /* define a global symbol marking the start of the .bss section */
                *(.bss)                                                 /* all .bss sections  */
        } >ram                                                          /* put all the above in RAM (it will be cleared in the startup code */

        . = ALIGN(2);                                           /* advance location counter to the next 32-bit boundary */
        _bss_end = . ;                                          /* define a global symbol marking the end of the .bss section */
}
        _end = .;                                                       /* define a global symbol marking the end of application RAM */
        end = .;

