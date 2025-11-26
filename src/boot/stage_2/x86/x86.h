#ifndef X86_H
#define X86_H

extern void interrupt_Bios_Out_Char(char c, int pos) __attribute__((cdecl));

void x86_outChar(char c, int pos);

#endif