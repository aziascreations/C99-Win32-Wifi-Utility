#include "profiles.h"

// Private prototypes



// Public functions

DWORD wifi_profile_iterateAll(HANDLE hClient, GUID *pInterfaceGuid,
							bool (*callback)(int profileIndex, WLAN_PROFILE_INFO *pProfileInfo, void *extraCbData),
							void *extraCbData) {
	if(hClient == NULL || pInterfaceGuid == NULL || callback == NULL) {
		error_println("One of the parameters was NULL !  (hClient: %p, pInterfaceGuid: %p, callback: %p)", hClient,
					  pInterfaceGuid, callback);
		return ERROR_INVALID_PARAMETER;
	}
	
	// Preparing some variables.
	DWORD dwResult = ERROR_SUCCESS;
	PWLAN_PROFILE_INFO_LIST pProfileList = NULL;
	
	dwResult = WlanGetProfileList(hClient, pInterfaceGuid, NULL, &pProfileList);
	
	if(dwResult == ERROR_SUCCESS) {
		for(int i = 0; i < pProfileList->dwNumberOfItems; i++) {
			if(!callback(i, (WLAN_PROFILE_INFO *) &pProfileList->ProfileInfo[i], extraCbData)) {
				// If the callback tells us to stop iterating, we do.
				// The condition is more like `if(!shouldContinue)`, but its the same...
				i = (int) pProfileList->dwNumberOfItems;
			}
		}
		
		// TODO: Is this condition required since we check for errors ?
		if (pProfileList != NULL) {
			WlanFreeMemory(pProfileList);
		}
	}
	
	return dwResult;
}

PWCHAR wifi_profile_formatInfoFlags(DWORD dwWlanProfileInfoFlags) {
	switch(dwWlanProfileInfoFlags) {
		case WLAN_PROFILE_GROUP_POLICY | WLAN_PROFILE_USER:
			return L"Read-only group policy profile accessible only to current user only";
		case WLAN_PROFILE_GROUP_POLICY:
			return L"Read-only group policy profile accessible only to all users";
		case WLAN_PROFILE_USER:
			return L"User created profile accessible to current user only";
		case 0:
			return L"User created profile accessible to all users";
		default:
			return L"Unknown flags combination";
	}
}

//DWORD wifi_profile_deleteAllProfiles(HANDLE hClient, GUID *pInterfaceGuid, bool continueOnError, int **errorCount) {
//	if(hClient == NULL) {
//		error_println("The Windows Wlan Server handle was NULL !");
//		return ERROR_INVALID_PARAMETER;
//	}
//
//	DWORD dwLatestError = ERROR_SUCCESS;
//
//
//
//	return dwLatestError;
//}
