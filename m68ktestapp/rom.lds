MEMORY 
{
	rom	: ORIGIN = 0,        	LENGTH = 1024K    /* ROM */
	ram	: ORIGIN = 0x100000, 	LENGTH = 1024k    /* RAM */
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
		. = ALIGN(2);
		_text_end = .;
	} >rom                                                      
	
	.data :     
	{
		_data_start = .;                                         
		*(.data*)                                              
		. = ALIGN(2);
		_data_end = .;                           
	} >ram AT >rom
	
	.bss :                                                         
	{
		_bss_start = .;
		*(.bss*)                                                
		. = ALIGN(2);                                           
		_bss_end = . ;                                          
	} >ram                                                          	
}
	_end = .;                                                       
	end = .;

