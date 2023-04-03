#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <windows.h>
#include <wlanapi.h>
#include <wtypes.h>

#include "../../commons/interfaces.h"

struct wifi_cli_iface_formatting_params {
	/** If set to `true`, only show the default human readable formatting and ignore other variables. */
	bool showIndex;
	bool showGuid;
	bool showDescription;
	bool showState;
	bool showStateText;
	bool doDefault;
	char *separator;
};
#define WifiInterfaceListingParameters struct wifi_cli_iface_formatting_params

void wifi_handler_ifaceListing(HANDLE hWlanClient, WifiInterfaceListingParameters params);

#define wifi_handler_getGuidFromGuid(hWlanClient, pOutputGuid, desiredGuid) wifi_handler_getGuidFromListing(hWlanClient, pOutputGuid, desiredGuid, 0)

#define wifi_handler_getGuidFromIndex(hWlanClient, pOutputGuid, index) wifi_handler_getGuidFromListing(hWlanClient, pOutputGuid, NULL, index)

bool wifi_handler_getGuidFromListing(HANDLE hWlanClient, GUID *pOutputGuid, char* desiredGuid, int desiredIndex);
