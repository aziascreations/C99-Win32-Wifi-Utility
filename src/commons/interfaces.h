#pragma once

#include <stdbool.h>

#include <windows.h>
#include <wlanapi.h>
#include <wtypes.h>

#include "../../libs/nibblepoker-c-goodies/src/debug.h"
#include "../../libs/nibblepoker-c-goodies/src/text.h"

#define isInterfaceGuid(potentialGuid) stringStartsWith(potentialGuid, "{")

/**
 * Loops over all available Wi-Fi interfaces and calls a given callback for each.
 * The data is freed once the function returns, it should be copied properly if needed afterward !
 * @param hClient
 * @param callback
 * @param extraCbData Extra data passed as-is to the callback.
 * @return
 */
DWORD wifi_iface_iterateAll(HANDLE hClient,
							bool (*callback)(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData),
							void *extraCbData);

PWCHAR wifi_iface_formatState(WLAN_INTERFACE_STATE isState);
