#include "profile.h"

// Private structures
struct wifi_cli_profile_getter_params {
	int desiredIndex;
	wchar_t *desiredName;
	wchar_t *foundName;
};

// Private prototypes

bool cb_listProfiles(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData);

bool cb_getProfileName(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData);

// Public functions

void wifi_handler_ifaceProfilesListing(HANDLE hWlanClient, GUID *ifaceGuid,
									   WifiInterfaceProfileListingParameters formattingParams) {
	DWORD dwResult = wifi_profile_iterateAll(hWlanClient, ifaceGuid, &cb_listProfiles, &formattingParams);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Failed to list wlan interfaces' profiles !\n");
	}
}

wchar_t *wifi_handler_getProfileNameFromInput(HANDLE hWlanClient, GUID *ifaceGuid, int desiredIndex, wchar_t *desiredName) {
	struct wifi_cli_profile_getter_params searchParams;
	searchParams.desiredIndex = desiredIndex;
	searchParams.desiredName = desiredName;
	searchParams.foundName = NULL;
	
	DWORD dwResult = wifi_profile_iterateAll(hWlanClient, ifaceGuid, &cb_getProfileName, &searchParams);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Failed to list wlan interfaces' profiles !\n");
		
		// Cleaning up, just in case.
		if(searchParams.foundName != NULL) {
			free(searchParams.foundName);
			searchParams.foundName = NULL;
		}
	}
	
	return searchParams.foundName;
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
