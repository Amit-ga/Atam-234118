/* TODO: Add the other needed fields.*/
ENTRY(_hw3_dance)

EXTERN(greens)
EXTERN(blacks)

PHDRS
{
  text PT_LOAD FLAGS(3);
  data PT_LOAD FLAGS(2);
  rodata PT_LOAD FLAGS(5);
}

SECTIONS
{
  . = 0x400000;
  .text : 
  {
	*(.text)
  } :text
  
  . = 0x20000;
  .data : 
  {
	*(.data)
	*(.bss)
  } :data
  
  . = 0x90000000;
  .rodata : 
  {
	*(.rodata)
  } :rodata

  

}