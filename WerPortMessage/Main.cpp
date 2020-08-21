#define PHNT_VERSION PHNT_WINBLUE
#include <phnt_windows.h>
#include <phnt.h>
#include <fmt/format.h>
#include <string>
#include <vector>
#include "weralpc.h"
#include "Messages.h"

#pragma comment(lib, "ntdll.lib")

WNF_STATE_NAME WNF_WER_SERVICE_START = {
    0xa3bc0875, 0x41940b3a };

BOOL InitWERSvcHeader(PWERPORTMSG* WerMsg)
{
    *WerMsg = nullptr;

    USHORT usWerMsgSize = sizeof(WERMSG);
    USHORT totalSize = usWerMsgSize + sizeof(PORT_MESSAGE); // 0x578

    auto localWerMsg = (PWERPORTMSG)malloc(totalSize);
    RtlZeroMemory(localWerMsg, totalSize);

    // set this size initially
    localWerMsg->Header.u2.s2.Type = 0;
    localWerMsg->Header.u1.s1.TotalLength = totalSize;
    localWerMsg->Data.Version = 0x20000000;
    *WerMsg = localWerMsg;
    return TRUE;
}

// init alpc port attributes
// void InitializeAlpcPortAttributes(PALPC_PORT_ATTRIBUTES *AlpcPortAttributes)
// {
//     SIZE_T MaxMessageLength;
//     SIZE_T MemoryBandwidth = 0;
//     SIZE_T MaxPoolUsage = 0;
//     SIZE_T MaxSectionSize = 0;
//     SIZE_T MaxViewSize = 0;
//     ULONG DupObjectTypes = 0;
//     *AlpcPortAttributes = nullptr;
//     //RtlZeroMemory(&AlpcPortAttributes, sizeof(ALPC_PORT_ATTRIBUTES));
// 
//     MaxMessageLength = 0x578; // for WER, this is max message length.
// 
//     AlpcPortAttributes.Flags = 0;
//     AlpcPortAttributes.MaxMessageLength = MaxMessageLength;
//     AlpcPortAttributes.MemoryBandwidth = 0;
//     AlpcPortAttributes.MaxMessageLength = 0;
//     AlpcPortAttributes.MaxSectionSize = 0;
//     AlpcPortAttributes.MaxViewSize = 0;
//     AlpcPortAttributes.DupObjectTypes = 0;
// 
//     // AlpcPortAttributes.SecurityQos.Length = sizeof(AlpcPortAttributes.SecurityQos);
//     // AlpcPortAttributes.SecurityQos.ImpersonationLevel = SECURITY_IMPERSONATION_LEVEL::SecurityImpersonation;
//     // AlpcPortAttributes.SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;
//     // AlpcPortAttributes.SecurityQos.EffectiveOnly = false;
// }

int main(int argc, char** argv)
{
    SID_IDENTIFIER_AUTHORITY sidId = SECURITY_NT_AUTHORITY;
    PSID pSid = nullptr;
    int pvBuffer = 0;
    LARGE_INTEGER ulPortTimeouts;
    ULONG dwRetLen;
    NTSTATUS status;
    UNICODE_STRING ucWerPortString;
    UNICODE_STRING ucErrorPortReady;
    OBJECT_ATTRIBUTES obj;
    HANDLE hAlpcPortHandle = NULL;
    HANDLE hEventHandle = NULL;

    memset((PVOID)&ucWerPortString, 0, sizeof(UNICODE_STRING));
    memset((PVOID)&ucErrorPortReady, 0, sizeof(UNICODE_STRING));
    RtlInitUnicodeString(&ucWerPortString, L"\\WindowsErrorReportingServicePort");
    RtlInitUnicodeString(&ucErrorPortReady, L"\\KernelObjects\\SystemErrorPortReady");

    /*InitializeObjectAttributes(&obj, NULL, NULL, NULL, NULL);*/

    // NTSTATUS status = ZwQueryWnfStateNameInformation(&WNF_WER_SERVICE_START,
    //                                                  WnfInfoSubscribersPresent,
    //                                                  (PVOID)0, (PVOID)&pvBuffer,
    //                                                  4);
    // if(!pvBuffer)
    // {
    //     fmt::print("No Buffer value, Buffer value: {}\n", pvBuffer);
    //     std::getchar();
    //     return 0;
    // }

    // fmt::print("Buffer value: {}\n", pvBuffer);
    // std::getchar();

    // start the wer service
    ZwUpdateWnfStateData(&WNF_WER_SERVICE_START,
                         0, 0, 0, 0, 0, 0);

    // Get the timeout value to wait for service init
    status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemErrorPortTimeouts,
                                      (PVOID)&ulPortTimeouts, 
                                      sizeof(LARGE_INTEGER), 
                                      &dwRetLen);
    if (!NT_SUCCESS(status))
    {
        fmt::print("Query System Info Failed with status and value: {:X}: {}\n", status,
                   ulPortTimeouts.LowPart);
        std::getchar();
        return 0;
    }
    fmt::print("Port Timeout Value: high{}:low{}:quad:{:X}\n",
               ulPortTimeouts.HighPart, ulPortTimeouts.LowPart, ulPortTimeouts.QuadPart);

    /* Not Important */
    OBJECT_ATTRIBUTES objTimeoutObject;
    memset(&objTimeoutObject, 0, sizeof(OBJECT_ATTRIBUTES));
    objTimeoutObject.Length = sizeof(OBJECT_ATTRIBUTES);
    objTimeoutObject.RootDirectory = 0;
    objTimeoutObject.ObjectName = &ucErrorPortReady;
    objTimeoutObject.SecurityDescriptor = NULL;
    //werobj.SecurityQualityOfService = NULL;

    status = NtOpenEvent(&hEventHandle, EVENT_QUERY_STATE, &objTimeoutObject);
    if (!NT_SUCCESS(status))
    {
        fmt::print("Failed to Open Event\n");
        std::getchar();
        return 0;
    }
    NtWaitForSingleObject(hEventHandle, 0, 0);
    NtClose(hEventHandle);

    // allocate space for sid +1 subauthority
    ULONG ulSidLen = RtlLengthRequiredSid(1);
    fmt::print("Required SID length: {:X}\n", ulSidLen);

    //PVOID sidspace = VirtualAlloc(NULL, 1, MEM_COMMIT, PAGE_READWRITE);
    char* sidspace = (char*)malloc(ulSidLen);
    memset(sidspace, 0, ulSidLen);
    pSid = (PSID)sidspace;

    RtlInitializeSid(pSid, &sidId, 1);
    *(RtlSubAuthoritySid(pSid, 0)) = 18; // LocalSystem

    PUNICODE_STRING szSidString = (PUNICODE_STRING)malloc(sizeof(WCHAR) * 150 + sizeof(UNICODE_STRING));
    szSidString->MaximumLength = sizeof(WCHAR) * 150;
    status = RtlConvertSidToUnicodeString(szSidString, pSid, FALSE);

    if (!NT_SUCCESS(status))
    {
        fmt::print("Failed to Init SID: error {:x}\n", status);
        free(szSidString);
        free(sidspace);
        std::getchar();
        return 0;
    }

    fmt::print(L"Sid String = {}\n", szSidString->Buffer);

    Sleep(5);

    // initialize port attributes
    fmt::print("Initializing Port Attributes\n");
    ALPC_PORT_ATTRIBUTES alpcPortAttributes;
    memset(&alpcPortAttributes, 0, sizeof(ALPC_PORT_ATTRIBUTES));
    alpcPortAttributes.MaxMessageLength = 0x578;

   /* InitializeAlpcPortAttributes(&alpcPortAttributes);*/

    memset(&obj, 0, sizeof(OBJECT_ATTRIBUTES));
    obj.Length = sizeof(OBJECT_ATTRIBUTES);
    obj.RootDirectory = 0;
    obj.ObjectName = NULL;
    obj.SecurityDescriptor = NULL;

    /*InitializeObjectAttributes(&obj, NULL, NULL, NULL, NULL);*/
    // Connect to the Alpc Port
    fmt::print("Connecting to port\n");

    status = NtAlpcConnectPort(&hAlpcPortHandle,
                               &ucWerPortString,
                               &obj,
                               &alpcPortAttributes,
                               ALPC_MESSAGE_FLAGS::kAlpcMessageFlagSyncRequest,
                               pSid,
                               NULL, NULL,
                               NULL, NULL, 
                               &ulPortTimeouts);
    std::printf("Status: %X\n", status);
    if (!NT_SUCCESS(status))
    {

        fmt::print("Failed to connect to the port. Failed with status { :X}\n",
                   RtlNtStatusToDosError(status));
        std::getchar();
        if (hAlpcPortHandle)
            CloseHandle(hAlpcPortHandle);
        return 0;
    }

    fmt::print("Connected to port. Press a key to cont.\n");
    std::getchar();

    // Attempt to send messages to the port

    return 0;
}