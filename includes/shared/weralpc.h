#pragma once

#define PHNT_VERSION PHNT_WINBLUE
#define UNICODE
#include <phnt_windows.h>
#include <phnt.h>

#define SystemErrorPortTimeouts 0x73


typedef struct _WERHANDLETABLE
{
    ULONG NumHandles;
    HANDLE *Handles;
}WERHANDLETABLE, * PWERHANDLETABLE;

typedef struct _WERMSG
{
    ULONG MsgId;
    ULONG Version;
    CLIENT_ID ProcessId;
    PWERHANDLETABLE FileMapTablePtr;
    BYTE Pad[0x538];
    //BYTE Padding[4];
}WERMSG, * PWERMSG;




typedef struct _WERPORTMSG
{
    PORT_MESSAGE Header;
    WERMSG Data;
} WERPORTMSG, * PWERPORTMSG;

WNF_STATE_NAME WNF_WER_SERVICE_START = {
    0xa3bc0875, 0x41940b3a };


typedef enum _ALPC_PORT_ATTRIBUTES_VALUES
{
    kAlpcPortAttributesNone = 0,
    kAlpcPortAttributesLpcPort = 0x1000, // Not accessible outside the kernel.
    kAlpcPortAttributesAllowImpersonation = 0x10000,
    kAlpcPortAttributesAllowLpcRequests = 0x20000,
    kAlpcPortAttributesWaitablePort = 0x40000,
    kAlpcPortAttributesAllowDupObject = 0x80000,
    kAlpcPortAttributesSystemProcess = 0x100000, // Not accessible outside the kernel.
    kAlpcPortAttributesLrpcWakePolicy1 = 0x200000,
    kAlpcPortAttributesLrpcWakePolicy2 = 0x400000,
    kAlpcPortAttributesLrpcWakePolicy3 = 0x800000,
    kAlpcPortAttributesDirectMessage = 0x1000000,
    kAlpcPortAttributesAllowMultiHandleAttribute = 0x2000000, /// If set then object duplication won't complete.
                                                              /// Used by RPC to ensure multi-handle attributes don't fail when receiving
} ALPC_PORT_ATTRIBUTES_VALUES,
* PALPC_PORT_ATTRIBUTES_VALUES;

typedef enum _ALPC_MESSAGE_FLAGS
{
    kAlpcMessageFlagNone = 0,
    kAlpcMessageFlagReplyMessage = 0x1,
    kAlpcMessageFlagLpcMode = 0x2,
    kAlpcMessageFlagReleaseMessage = 0x10000,
    kAlpcMessageFlagSyncRequest = 0x20000,
    kAlpcMessageFlagTrackPortReferences = 0x40000,
    kAlpcMessageFlagWaitUserMode = 0x100000,
    kAlpcMessageFlagWaitAlertable = 0x200000,
    kAlpcMessageFlagWaitChargePolicy = 0x400000,
    kAlpcMessageFlagUnknown1000000 = 0x1000000,
    kAlpcMessageFlagWow64Call = 0x40000000, /// When used all structures passed to kernel need to be 64 bit versions.
} ALPC_MESSAGE_FLAGS,
* PALPC_MESSAGE_FLAGS;