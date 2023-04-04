#pragma once

#include <stdbool.h>

#include <windows.h>
#include <wlanapi.h>
#include <wtypes.h>

#include "../../libs/nibblepoker-c-goodies/src/debug.h"
#include "../../libs/nibblepoker-c-goodies/src/text.h"

/**
 * Loops over all available profiles in a designated Wi-Fi interface, and calls a given callback for each.
 * The data is freed once the function returns, it should be copied properly if needed afterward !
 * @param hClient
 * @param pInterfaceGuid
 * @param callback
 * @param extraCbData Extra data passed as-is to the callback.
 * @return
 */
DWORD wifi_profile_iterateAll(HANDLE hClient, GUID *pInterfaceGuid,
							  bool (*callback)(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData),
							  void *extraCbData);

/**
 * Formats a given set of profile info flags into a human-readable string.
 * @param dwWlanProfileInfoFlags The profile info flags as given in `WLAN_PROFILE_INFO->dwFlags`.
 * @return The human-readable text.
 */
PWCHAR wifi_profile_formatInfoFlags(DWORD dwWlanProfileInfoFlags);
