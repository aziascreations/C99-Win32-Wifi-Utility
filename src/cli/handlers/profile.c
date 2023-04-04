#include "profile.h"

// Private prototypes

bool cb_listProfiles(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData);

// Public functions

void wifi_handler_ifaceProfilesListing(HANDLE hWlanClient, GUID *ifaceGuid,
									   WifiInterfaceProfileListingParameters formattingParams) {
	DWORD dwResult = wifi_profile_iterateAll(hWlanClient, ifaceGuid, &cb_listProfiles, &formattingParams);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Failed to list wlan interfaces' profiles !\n");
	}
}

// Private functions

bool cb_listProfiles(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData) {
	// "Should" be optimized when compiling to make direct calls to it instead.
	WifiInterfaceProfileListingParameters *params = extraCbData;
	
	if(params->doDefault) {
		// TODO: Doesn't print out emojis properly (Also, check the format so it is consistent !!!)
		wprintf(L"[%d]%hs%Ls%hs%ws\n", profileIndex, params->separator, pProfileInfo->strProfileName,
				params->separator, wifi_profile_formatInfoFlags(pProfileInfo->dwFlags));
	} else {
		bool hasPrintedSomethingYet = false;
		
		if(params->showIndex) {
			wprintf(L"%d", profileIndex);
			hasPrintedSomethingYet = true;
		}
		if(params->showName) {
			// TODO: Doesn't print out emojis properly (Also, check the format so it is consistent !!!)
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
