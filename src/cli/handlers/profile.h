#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <windows.h>
#include <wlanapi.h>
#include <wtypes.h>

#include "../../commons/profiles.h"

struct wifi_cli_iface_profile_formatting_params {
	bool showIndex;
	bool showName;
	bool showFlags;
	bool showFlagsText;
	/** If set to `true`, only show the default human readable formatting and ignore other variables. */
	bool doDefault;
	char *separator;
};
#define WifiInterfaceProfileListingParameters struct wifi_cli_iface_profile_formatting_params

void wifi_handler_ifaceProfilesListing(HANDLE hWlanClient, GUID *ifaceGuid,
									   WifiInterfaceProfileListingParameters formattingParams);

wchar_t *wifi_handler_getProfileNameFromInput(HANDLE hWlanClient, GUID *ifaceGuid, int desiredIndex,
											  wchar_t *desiredName);