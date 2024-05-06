#include <asm/desc.h>

// TO_DO: store the address of the IDT at the parameter given
void my_store_idt(struct desc_ptr *idtr) {
// <STUDENT FILL>

asm("SIDT %0;"
:"=m"(*idtr) /* output */
: /* input */
:/* clobbered */
);
    
// </STUDENT FILL>
}////

// TO_DO: load the address of the IDT from the parameter given
void my_load_idt(struct desc_ptr *idtr) {
// <STUDENT FILL>
asm("LIDT %0;"
: /* output */
:"m"(*idtr) /* input */
:/* clobbered */
);
// <STUDENT FILL>
}

// TO_DO: set the address in the right place at the parameter gate
// try to remember - how this information is stored?
void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
// <STUDENT FILL>
gate->offset_low=(u16)(addr);
gate->offset_middle=(u16)(addr >> 16);
gate->offset_high=(u32)(addr >> 32);

// </STUDENT FILL>
}

// TO_DO: return val is the address stored at the parameter gate_desc gate
// try to remember - how this information is stored?
unsigned long my_get_gate_offset(gate_desc *gate) {
// <STUDENT FILL>
return gate->offset_low | ((unsigned long)gate->offset_middle << 16) |
        ((unsigned long) gate->offset_high << 32);
// </STUDENT FILL>
}
