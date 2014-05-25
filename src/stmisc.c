#include <ntddk.h>
#include "stmisc.h"
#include "ps.h"

PVOID misc_get_func_address(PCWSTR fnname)
{
	UNICODE_STRING unistr;
	RtlInitUnicodeString(&unistr,fnname);
	return MmGetSystemRoutineAddress(&unistr);
}

