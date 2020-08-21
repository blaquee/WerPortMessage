#pragma once

VOID AllocElevatedLaunchMsg(PWERPORTMSG* WerMsg);
VOID AllocSvcCollectMemInfo(PWERPORTMSG* WerMsg);
VOID AllocReplyMsg(PWERPORTMSG* WerMsg);

VOID DeAllocMsg(PWERPORTMSG* Msg);