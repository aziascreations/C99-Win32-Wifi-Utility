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
