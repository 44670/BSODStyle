#include <ntddk.h>
__declspec(dllimport) ULONG VidBitBlt(PVOID pbuff,ULONG x,ULONG y);
__declspec(dllimport) ULONG InbvAcquireDisplayOwnership();
__declspec(dllimport) ULONG InbvResetDisplay();
__declspec(dllimport) VOID  InbvSolidColorFill	(	IN ULONG 	Left,
IN ULONG 	Top,
IN ULONG 	Width,
IN ULONG 	Height,
IN ULONG 	Color	 
)			;
__declspec(dllimport)VOID
InbvSetTextColor(
    IN ULONG Color
);
__declspec(dllimport) VOID InbvSetScrollRegion(
    IN ULONG Left,
    IN ULONG Top,
    IN ULONG Width,
    IN ULONG Height
);
