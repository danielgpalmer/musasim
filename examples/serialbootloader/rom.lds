MEMORY 
{
	rom	: ORIGIN = 0,        	LENGTH = 1024K    /* ROM */
	ram	: ORIGIN = 0x1F8000, 	LENGTH = 32k    /* RAM */
}

SECTIONS 
{
	. = 0;

	.startup : 
	{ 
		*(.text)
		. = ALIGN(2);
	} >rom

	.text :                                                          
    {
 		*(.text*)                                                
		*(.rodata*)
		*(.eh_frame)
		. = ALIGN(4);
		_text_end = .;
	} >rom                                                      
	
	.data :     
	{
		_data_start = .;                                         
		*(.data*)                                              
		. = ALIGN(4);
		_data_end = .;                           
	} >ram AT >rom
	
	.bss :                                                         
	{
		_bss_start = .;
		*(.bss*)                                                
		. = ALIGN(4);                                           
		_bss_end = . ;                                          
		_end = .;                                                       
		end = .;
	} >ram                                                          	
}

