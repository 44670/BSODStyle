#include <ntddk.h>


ULONG ade32_disasm(IN PVOID opcode0);
ULONG ihook_install_hook(PVOID *ppSysFn,PVOID pCallbackFn);
ULONG ihook_remove_hook(PVOID *ppSysFn,PVOID pOldSysFn,ULONG offset);
VOID ihook_wpoff();
VOID ihook_wpon();