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

/**
 * Deletes all profiles from all interface in the given client.
 * May report back errors depending on some parameters.
 * @param hWlanClient
 * @param continueOnError Continue the deletion process even when an error occurs.
 * @param reportErrorsInStdout Report each individual error in stdout instead of stderr.
 * @return
 */
DWORD wifi_handler_deleteAllProfilesFromAll(HANDLE hWlanClient, bool continueOnError, bool reportErrorsInStdout);

/**
 * Deletes all profiles from a given interface.
 * May report back errors depending on some parameters.
 * @param hWlanClient
 * @param ifaceGuid relevant interface's GUID.
 * @param continueOnError Continue the deletion process even when an error occurs.
 * @param reportErrorsInStdout Report each individual error in stdout instead of stderr.
 * @return
 */
DWORD wifi_handler_deleteAllProfiles(HANDLE hWlanClient, GUID *ifaceGuid, bool continueOnError, bool reportErrorsInStdout);
