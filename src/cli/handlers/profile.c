#include "profile.h"

// Private structures
struct wifi_cli_profile_getter_params {
	int desiredIndex;
	wchar_t *desiredName;
	wchar_t *foundName;
};

struct wifi_cli_profile_deleter_params {
	HANDLE hWlanClient;
	GUID *ifaceGuid;
	bool continueOnError;
	bool reportErrorsInStdout;
	DWORD lastError;
};

// Private prototypes

bool cb_listProfiles(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData);

bool cb_getProfileName(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData);

bool cb_deleteProfile(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData);

// Public functions

void wifi_handler_ifaceProfilesListing(HANDLE hWlanClient, GUID *ifaceGuid,
									   WifiInterfaceProfileListingParameters formattingParams) {
	DWORD dwResult = wifi_profile_iterateAll(hWlanClient, ifaceGuid, &cb_listProfiles, &formattingParams);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Failed to list wlan interfaces' profiles !  (Win32 Error %lu)\n", dwResult);
	}
}

wchar_t *wifi_handler_getProfileNameFromInput(HANDLE hWlanClient, GUID *ifaceGuid, int desiredIndex, wchar_t *desiredName) {
	struct wifi_cli_profile_getter_params searchParams;
	searchParams.desiredIndex = desiredIndex;
	searchParams.desiredName = desiredName;
	searchParams.foundName = NULL;
	
	DWORD dwResult = wifi_profile_iterateAll(hWlanClient, ifaceGuid, &cb_getProfileName, &searchParams);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Failed to list wlan interfaces' profiles !  (Win32 Error %lu)\n", dwResult);
		
		// Cleaning up, just in case.
		if(searchParams.foundName != NULL) {
			free(searchParams.foundName);
			searchParams.foundName = NULL;
		}
	}
	
	return searchParams.foundName;
}

DWORD wifi_handler_deleteAllProfilesFromAll(HANDLE hWlanClient, bool continueOnError, bool reportErrorsInStdout) {
	DWORD returnedError = ERROR_SUCCESS;
	
	// Preparing some variables.
	DWORD dwResult = ERROR_SUCCESS;
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	
	dwResult = WlanEnumInterfaces(hWlanClient, NULL, &pIfList);
	if(dwResult == ERROR_SUCCESS) {
		for(int i = 0; i < (int) pIfList->dwNumberOfItems; i++) {
			// We re-use "dwResult" for the errors returned for each interface
			dwResult = wifi_handler_deleteAllProfiles(hWlanClient, &pIfList->InterfaceInfo[i].InterfaceGuid, continueOnError, reportErrorsInStdout);
			
			if(dwResult != ERROR_SUCCESS) {
				returnedError = dwResult;
				
				if(!continueOnError) {
					// We force the end of the loop if we shouldn't continue.
					// The rest is handled like normal.
					i = (int) pIfList->dwNumberOfItems;
				}
			}
		}
		
		if(pIfList != NULL) {
			WlanFreeMemory(pIfList);
		}
	}
	
	return returnedError;
}

DWORD wifi_handler_deleteAllProfiles(HANDLE hWlanClient, GUID *ifaceGuid, bool continueOnError, bool reportErrorsInStdout) {
	// Preparing the parameters that will be passed to the callback.
	// This isn't "optimized" for recursive call, but I won't spaghettify the code for the ONE in a billion guy
	//  that has 100+ Wi-Fi interfaces and cannot wait a couple of cycles to save his own life.
	// It's fast enough as-is.
	struct wifi_cli_profile_deleter_params deletionParams;
	deletionParams.hWlanClient = hWlanClient;
	deletionParams.ifaceGuid = ifaceGuid;
	deletionParams.continueOnError = continueOnError;
	deletionParams.reportErrorsInStdout = reportErrorsInStdout;
	deletionParams.lastError = ERROR_SUCCESS;
	
	// The official documentation isn't 100% clear on whether the iterating function will skip entries if we delete
	//  them as we go, but we can assume it won't since we allocate a list for it.
	DWORD dwResult = wifi_profile_iterateAll(hWlanClient, ifaceGuid, &cb_deleteProfile, &deletionParams);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(reportErrorsInStdout ? stdout : stderr,
				"Failed to iterate over wlan interfaces' profiles !  (Win32 Error %lu)\n", dwResult);
		return dwResult;
	}
	
	return deletionParams.lastError;
}

// Private functions

bool cb_listProfiles(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData) {
	// "Should" be optimized when compiling to make direct calls to it instead.
	WifiInterfaceProfileListingParameters *params = extraCbData;
	
	if(params->doDefault) {
		// Remark: If emojis aren't printed out properly, you need to use the command `chcp 65001`.
		wprintf(L"[%d]%hs%Ls%hs%ws\n", profileIndex, params->separator, pProfileInfo->strProfileName,
				params->separator, wifi_profile_formatInfoFlags(pProfileInfo->dwFlags));
	} else {
		bool hasPrintedSomethingYet = false;
		
		if(params->showIndex) {
			wprintf(L"%d", profileIndex);
			hasPrintedSomethingYet = true;
		}
		if(params->showName) {
			// Remark: If emojis aren't printed out properly, you need to use the command `chcp 65001`.
			wprintf(L"%hs%lls", hasPrintedSomethingYet ? params->separator : "", pProfileInfo->strProfileName);
			hasPrintedSomethingYet = true;
		}
		if(params->showFlags) {
			wprintf(L"%hs%d", hasPrintedSomethingYet ? params->separator : "", pProfileInfo->dwFlags);
			hasPrintedSomethingYet = true;
		} else if(params->showFlagsText) {
			wprintf(L"%hs%ws", hasPrintedSomethingYet ? params->separator : "",
					wifi_profile_formatInfoFlags(pProfileInfo->dwFlags));
		}
		
		printf("\n");
	}
	
	return true;
}

bool cb_getProfileName(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData) {
	// "Should" be optimized when compiling to make direct calls to it instead.
	struct wifi_cli_profile_getter_params *params = extraCbData;
	
	if(params->desiredIndex != -1 && params->desiredIndex == profileIndex) {
		params->foundName = copyWCharString(pProfileInfo->strProfileName);
		return false;
	}
	
	if(params->desiredName != NULL) {
		if(wcscmp(params->desiredName, pProfileInfo->strProfileName) == 0) {
			// Copying the name here is redundant, but it simplifies later operations by always having a copy if found.
			params->foundName = copyWCharString(pProfileInfo->strProfileName);
			return false;
		}
	}
	
	return true;
}

bool cb_deleteProfile(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData) {
	// "Should" be optimized when compiling to make direct calls to it instead.
	struct wifi_cli_profile_deleter_params *params = extraCbData;
	
	debug_println("Deleting profile \"%ws\" from iface @%p", pProfileInfo->strProfileName, params->ifaceGuid);
	DWORD dwProfileDeletionResult = WlanDeleteProfile(params->hWlanClient, params->ifaceGuid, pProfileInfo->strProfileName, NULL);
	
	if(dwProfileDeletionResult != ERROR_SUCCESS) {
		// Printing the error to the appropriate output.
		fprintf(params->reportErrorsInStdout ? stdout : stderr,
				"Unable to delete profile \"%ws\" !  (Win32 Error %lu)\n",
				pProfileInfo->strProfileName, dwProfileDeletionResult);
		
		// Relaying back the error.
		params->lastError = dwProfileDeletionResult;
		
		// Checking if we should continue.
		return params->continueOnError;
	}
	
	// Normal execution's return value.
	return true;
}
