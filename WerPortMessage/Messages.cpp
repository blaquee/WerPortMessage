#define PHNT_VERSION PHNT_WINBLUE
#include <phnt_windows.h>
#include <phnt.h>
#include <cstdlib>
#include "weralpc.h"


VOID DeAllocMsg(PWERPORTMSG *Msg)
{
    if (*Msg)
    { 
        free(*Msg);
        *Msg = nullptr;
    }
}

VOID AllocReplyMsg(PWERPORTMSG* WerMsg)
{
    *WerMsg = nullptr;
    PWERPORTMSG Msg = nullptr;
    Msg = (PWERPORTMSG)malloc(sizeof(WERPORTMSG));
    RtlZeroMemory(Msg, sizeof(WERPORTMSG));
    *WerMsg = Msg;
}

VOID AllocElevatedLaunchMsg(PWERPORTMSG* WerMsg)
{
    *WerMsg = nullptr;

    PWERPORTMSG Msg = nullptr;
    PWERMSG Body = nullptr;
    Body = (PWERMSG)malloc(sizeof(WERMSG));
    Msg = (PWERPORTMSG)malloc(sizeof(WERPORTMSG));
    RtlZeroMemory(Body, sizeof(WERMSG));
    RtlZeroMemory(Msg, sizeof(WERPORTMSG));

    ULONG ulTotalSize = sizeof(PORT_MESSAGE) +
        sizeof(WERMSG);

    Msg->Header.u2.s2.Type = 0;
    Msg->Header.u1.s1.TotalLength = ulTotalSize;

    Body->Version = 0x20000000;
    Body->MsgId = 0x50000000;

    RtlCopyMemory(&Msg->Data, Body, sizeof(WERMSG));
}

VOID AllocSvcCollectMemInfo(PWERPORTMSG* WerMsg)
{
    *WerMsg = nullptr;

    PWERPORTMSG Msg = nullptr;
    PWERMSG Body = nullptr;
    Body = (PWERMSG)malloc(sizeof(WERMSG));
    Msg = (PWERPORTMSG)malloc(sizeof(WERPORTMSG));
    RtlZeroMemory(Body, sizeof(WERMSG));
    RtlZeroMemory(Msg, sizeof(WERPORTMSG));

    ULONG ulTotalSize = sizeof(PORT_MESSAGE) +
        sizeof(WERMSG);

    Msg->Header.u2.s2.Type = 0;
    Msg->Header.u1.s1.TotalLength = ulTotalSize;

    Body->Version = 0x20000000;
    Body->MsgId = 0xF0030002;

    RtlCopyMemory(&Msg->Data, Body, sizeof(WERMSG));
    *WerMsg = Msg;
}