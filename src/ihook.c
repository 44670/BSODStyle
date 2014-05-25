#include <ntddk.h>
#include "stmisc.h"
#include "ihook.h"
VOID ihook_wpoff()
{
    __asm
    {
        cli;
        push eax;
        mov eax, cr0;
        and eax, 0FFFEFFFFh;
        mov cr0, eax;
        pop eax;
    }
}

VOID ihook_wpon()
{
    __asm
    {
        push eax;
        mov eax, cr0;
        or eax, 10000h;
        mov cr0, eax;
        pop eax;
        sti;
    }
}

ULONG ihook_jmpcode(PVOID src,PVOID dest)
{
	return ((ULONG)dest - (ULONG)src - 5);
}

ULONG ihook_install_hook(PVOID *ppSysFn,PVOID pCallbackFn)
{
	ULONG bufferlength = 5;
	ULONG offset = 0;
	ULONG length;
	ULONG i;
	PVOID pbuffer;
	PVOID pSysFn;
	KIRQL oldirql;
	pSysFn = *ppSysFn;
	__try
	{
		while (offset < bufferlength)
		{
			length = ade32_disasm((PVOID)((ULONG)pSysFn + offset));
			if (length == 0) return 0;
			if (offset > 64) return 0;
			for (i=1;i<=length;i++)
			{
				dprintf("%x ",*(PUCHAR)((ULONG)pSysFn + offset + i - 1));
			}
			offset+= length;
			dprintf("offset = %d \n ",offset);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		assert();
		return 0;
	}
	bufferlength = offset + 6;
	pbuffer = ExAllocatePoolWithTag(NonPagedPool,bufferlength,0);
	if (!pbuffer) return 0;
	//assert();
	RtlCopyMemory(pbuffer,pSysFn,offset);
	*(PUCHAR)((ULONG)pbuffer+offset) = 0xe9;
	*(PULONG)((ULONG)pbuffer+offset+1) = ihook_jmpcode((PVOID)((ULONG)pbuffer+offset),(PVOID)((ULONG)pSysFn + offset));
	*(PUCHAR)((ULONG)pbuffer+offset+5) = 0x90;
	*ppSysFn = pbuffer;
	oldirql = KeRaiseIrqlToDpcLevel();
	ihook_wpoff();
	*(PUCHAR)(pSysFn) = 0xe9;
	*(PULONG)((ULONG)pSysFn+1) = ihook_jmpcode(pSysFn,pCallbackFn);
	ihook_wpon();
	KeLowerIrql(oldirql);
	dprintf("oldirql %x\n",oldirql);
	return offset;
}

ULONG ihook_remove_hook(PVOID *ppSysFn,PVOID pOldSysFn,ULONG offset)
{
	KIRQL oldirql;
	if (offset < 5) {assert();return 0;}
	oldirql = KeRaiseIrqlToDpcLevel();
	ihook_wpoff();
	RtlCopyMemory(pOldSysFn,*ppSysFn,offset);
	ihook_wpon();
	KeLowerIrql(oldirql);
	ExFreePoolWithTag(*ppSysFn,0);
	*ppSysFn = pOldSysFn;
	return 1;
}
