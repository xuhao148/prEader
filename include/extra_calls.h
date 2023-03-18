#ifndef _XTRA_CALLS_H
#define _XTRA_CALLS_H
int ProcessPrintChars(long x);
void EnableGetkeyToMainFunctionReturn(void);
void DisableGetkeyToMainFunctionReturn(void);

__asm__(".text; .align 2; _ProcessPrintChars: mov.l sc_addr, r2; "
        "mov.l printchars_id, r0; jmp @r2; nop; "
        "printchars_id: .long 0x1300; "
        "sc_addr: .long 0x80020070");

__asm__(".text; .align 2; _EnableGetkeyToMainFunctionReturn: mov.l sc_addr2, r2; "
        "mov.l enablemenu_id, r0; jmp @r2; nop; "
        "enablemenu_id: .long 0x1EA6; "
        "sc_addr2: .long 0x80020070");

__asm__(".text; .align 2; _DisableGetkeyToMainFunctionReturn: mov.l sc_addr3, r2; "
        "mov.l disablemenu_id, r0; jmp @r2; nop; "
        "disablemenu_id: .long 0x1EA7; "
        "sc_addr3: .long 0x80020070");
#endif