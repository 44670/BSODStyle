#include <ntddk.h>
#include "inbv.h"
#include "ihook.h"
#define DEVICE_NAME L"\\Device\\test" //Driver Name
#define LINK_NAME L"\\DosDevices\\test" //Link Name
#define IOCTL_BASE    0x800
#define TEMPLATE_CTL_CODE(i) \
    CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE+i, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_LOADBITMAP TEMPLATE_CTL_CODE(1)


NTSTATUS DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTSTATUS DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp);

UCHAR Bitmap[700000];
PVOID pregion;
ULONG off_region;
__declspec(naked) VOID orig_setregion(    IN ULONG Left,
    IN ULONG Top,
    IN ULONG Width,
    IN ULONG Height)
{
	__asm { jmp pregion}
}
 VOID __stdcall hooked_setregion(IN ULONG Left,
    IN ULONG Top,
    IN ULONG Width,
    IN ULONG Height)
{
	InbvSolidColorFill(0,0,639,479,15);
	VidBitBlt(&Bitmap[0xe],0,0);
	InbvSetTextColor(1);
	orig_setregion(36,36,603,438);
}

VOID UnloadDriver(
	IN PDRIVER_OBJECT DriverObject	// a pointer to the object that represents our driver
	)
{
	UNICODE_STRING strLink;
    RtlInitUnicodeString(&strLink, LINK_NAME);
    IoDeleteSymbolicLink(&strLink);
    IoDeleteDevice(DriverObject->DeviceObject);
	ihook_remove_hook(&pregion,InbvSetScrollRegion,off_region);
}
NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,	// a pointer to the object that represents this device
	IN PUNICODE_STRING RegistryPath	// a pointer to our Services key in the registry
	)
{
	NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING ustrLinkName;
    UNICODE_STRING ustrDevName;    
    PDEVICE_OBJECT pDevObj;

	DriverObject->DriverUnload = (PDRIVER_UNLOAD)UnloadDriver;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
	RtlInitUnicodeString(&ustrDevName, DEVICE_NAME);
    status = IoCreateDevice(DriverObject, 
        0,
        &ustrDevName, 
        FILE_DEVICE_UNKNOWN,
        0,
        FALSE,
        &pDevObj);
    if(!NT_SUCCESS(status))
    {
        return status;
    }
    RtlInitUnicodeString(&ustrLinkName, LINK_NAME);
    status = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName); 
    if(!NT_SUCCESS(status))
    {
        IoDeleteDevice(pDevObj); 
        return status;
    }
	pregion = (PVOID)InbvSetScrollRegion ;
	off_region = ihook_install_hook(&pregion,(PVOID)hooked_setregion);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    PIO_STACK_LOCATION pIrpStack;
    ULONG uIoControlCode;
    PULONG pIoBuffer;
    ULONG uInSize;
    ULONG uOutSize;
    long hProcess;

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
    pIoBuffer = (PULONG)pIrp->AssociatedIrp.SystemBuffer;
    uInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    uOutSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    switch(uIoControlCode)
    {
	case IOCTL_LOADBITMAP:
		RtlCopyMemory(Bitmap,pIoBuffer,uInSize);
		status = STATUS_SUCCESS;
		break;
    }


    if(status == STATUS_SUCCESS)
        pIrp->IoStatus.Information = uOutSize;
    else
        pIrp->IoStatus.Information = 0;

    pIrp->IoStatus.Status = status;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return status;
}


NTSTATUS DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
    pIrp->IoStatus.Status = STATUS_SUCCESS;
    pIrp->IoStatus.Information = 0;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
