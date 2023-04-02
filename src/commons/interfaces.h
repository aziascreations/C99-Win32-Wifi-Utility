#pragma once

#include <stdbool.h>

#include <windows.h>
#include <wlanapi.h>
#include <wtypes.h>

#include "../../libs/nibblepoker-c-goodies/src/debug.h"

/**
 * Loops over all available Wi-Fi interfaces and calls a given callback for each.
 * The data is freed once the function returns, it should be copied properly if needed afterward !
 * @param hClient
 * @param callback
 * @return
 */
DWORD wifi_iface_iterateAll(HANDLE hClient, void (*callback)(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData), void *extraCbData);
