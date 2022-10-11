#include "Driver.h"

VOID  Unload(IN  PDRIVER_OBJECT  pDriverObject) {
   //this deletes the device
	IoDeleteDevice(pDriverObject->DeviceObject);

	return;
}



NTSTATUS FindGameProcessByName(CHAR* process_name, PEPROCESS* process, int range)
{
	PEPROCESS sys_process = PsInitialSystemProcess;
	PEPROCESS cur_entry = sys_process;

	CHAR image_name[300];

	do
	{
		RtlCopyMemory((PVOID)(&image_name), (PVOID)((uintptr_t)cur_entry + 0x450) /*EPROCESS->ImageFileName*/, sizeof(image_name));

		if ( !utils::mouse.service_callback || !utils::mouse.mouse_device )
		utils::setup_mouclasscallback( &utils::mouse );

	switch ( pstruct->request_key ) {

	case DRIVER_GETPOOL:
		return pstruct->allocation = utils::find_guarded_region();

	case DRIVER_READVM:
		return readvm( pstruct );

	case DRIVER_MOUSE:
		return move_mouse( pstruct );
	}

	return true;

// IOCTL handler for memory commands

NTSTATUS ProcessReadWriteMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T Size)
{
	SIZE_T Bytes = 0;

	if (NT_SUCCESS(MmCopyVirtualMemory(SourceProcess, SourceAddress, TargetProcess, TargetAddress, Size, UserMode, &Bytes)))
		return STATUS_SUCCESS;
	else
		return STATUS_ACCESS_DENIED;
}

#define SIOCTL_TYPE 40000

#define IOCTL_CODE 0x800

#define IOCTL_MEMORY_COMMAND\
 CTL_CODE( SIOCTL_TYPE, IOCTL_CODE, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)

NTSTATUS Function_IRP_MJ_CREATE(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	DbgPrint("IRP MJ CREATE received.");
	return STATUS_SUCCESS;
}

NTSTATUS Function_IRP_MJ_CLOSE(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	DbgPrint("IRP MJ CLOSE received.");
	return STATUS_SUCCESS;
}

PEPROCESS valorantProcess;
DWORD64 processBaseAddress;

#define COMMAND_MAGIC 0xDEADBEEF
struct memory_command {
	INT operation;

	DWORD64 magic;

	DWORD64 retval;

	DWORD64 memaddress;
	DWORD64 length;
	PVOID buffer;
};

NTSTATUS Function_IRP_DEVICE_CONTROL(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	PIO_STACK_LOCATION pIoStackLocation;
	struct memory_command* cmd = Irp->AssociatedIrp.SystemBuffer;

	Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;

		_requests* in = ( _requests* )rcx;
		requesthandler( in );
	{
	case IOCTL_MEMORY_COMMAND:
		DbgPrintEx(0, 0, "[ValorHook] IOCTL command received\n");

		if (cmd->magic != COMMAND_MAGIC) {
			Irp->IoStatus.Status = STATUS_ACCESS_DENIED;
			cmd->retval = 2;
			DbgPrintEx(0, 0, "[ValorHook] IOCTL invalid magic\n");
			break;
		}

		switch (cmd->operation) {
		case 0: // read memory
			Irp->IoStatus.Status = STATUS_SUCCESS;

			ProcessReadWriteMemory(valorantProcess, cmd->memaddress, IoGetCurrentProcess(), cmd->buffer, cmd->length);
			break;

		case 1: // write memory
			Irp->IoStatus.Status = STATUS_SUCCESS;

			ProcessReadWriteMemory(IoGetCurrentProcess(), cmd->buffer, valorantProcess, cmd->memaddress, cmd->length);
			break;
		case 2: // find valorant PEPROCESS
			Irp->IoStatus.Status = STATUS_SUCCESS;
			DbgPrintEx(0, 0, "[ValorHook] Setting target PID...\n");

			valorantProcess = NULL;

			PsLookupProcessByProcessId(cmd->retval, &valorantProcess);

			if (!valorantProcess) {
				cmd->retval = NULL;
				break;
			}
			
			cmd->retval = (DWORD64)PsGetProcessSectionBaseAddress(valorantProcess);

			break;
		case 10:
			// just crash windows idk
			Unload(gDeviceObject);
			break;
		}

		break;
	}

	// Finish the I/O operation by simply completing the packet and returning
	// the same status as in the packet itself.
	Irp->IoStatus.Information = sizeof(struct memory_command);
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

auto move_mouse( _requests* in ) -> bool
{
//hackerman https://www.unknowncheats.me/forum/members/1595354.html
	MOUSE_INPUT_DATA input;

	input.LastX = in->x;
	input.LastY = in->y;
	input.ButtonFlags = in->button_flags;

	KIRQL irql;
	KeRaiseIrql( DISPATCH_LEVEL, &irql );

	ULONG ret;
	utils::mouse.service_callback( utils::mouse.mouse_device, &input, ( PMOUSE_INPUT_DATA )&input + 1, &ret );

	KeLowerIrql(irql);

	return true;
}

/// <summary>
/// Initializes the driver and its device.
/// </summary>
/// <param name="DriverObject">The driver object.</param>
/// <param name="RegistryPath">The registry path.</param>
NTSTATUS DriverInitialize(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	DbgPrintEx(0, 0, "[ValorHook] Executing " __FUNCTION__ ".\n");

	NTSTATUS			Status;
	UNICODE_STRING		DeviceName;
	UNICODE_STRING		SymbolicName;
	PDEVICE_OBJECT      DeviceObject;

	UNREFERENCED_PARAMETER(RegistryPath);

	RtlInitUnicodeString(&DeviceName, ConstDeviceName);
	RtlInitUnicodeString(&SymbolicName, ConstSymbolic);

	// Create device
	Status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);

	if (NT_SUCCESS(Status))
	{
		PEPROCESS source_process = NULL;
	if ( in->src_pid == 0 ) return STATUS_UNSUCCESSFUL;
		
		{
			DbgPrintEx(0, 0, "[ValorHook] Created symlink\n");

			for (ULONG i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
			{
				DriverObject->MajorFunction[i] = &UnsupportedCall;
			}

			NTSTATUS status = PsLookupProcessByProcessId( ( HANDLE )in->src_pid, &source_process);
			if (status != STATUS_SUCCESS) return false;

			size_t memsize = 0;

				if ( !NT_SUCCESS( utils::readprocessmemory( source_process, ( void* )in->src_addr, ( void* )in->dst_addr, in->size, &memsize) ) )
				return false;

			// Globals..

			gDriverObject = DriverObject;
			gDeviceObject = DeviceObject;
		}
		else
		{
			IoDeleteDevice(DeviceObject);
		}
	}

	if (DriverObject)
	{
		const PKLDR_DATA_TABLE_ENTRY DriverSection = DriverObject->DriverSection;

		if (DriverSection)
		{
			DriverSection->FullImageName.Buffer[0] = L'\0';
			DriverSection->FullImageName.Length = 0;
			DriverSection->FullImageName.MaximumLength = 0;

			DriverSection->BaseImageName.Buffer[0] = L'\0';
			DriverSection->BaseImageName.Length = 0;
			DriverSection->BaseImageName.MaximumLength = 0;
		}

		DriverObject->DriverSection = NULL;
		DriverObject->DriverStart = NULL;
		DriverObject->DriverSize = 0;
		DriverObject->DriverUnload = NULL;
		DriverObject->DriverInit = NULL;
		DriverObject->DeviceObject = NULL;
		

	return true;
}
