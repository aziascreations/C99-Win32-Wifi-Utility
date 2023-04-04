#include "iface.h"

// Private structures
struct wifi_cli_iface_guid_getter_params {
	bool wasFound;
	GUID *pOutputGuid;
	wchar_t *desiredGuid;
	int desiredIndex;
};

// Private prototypes

bool cb_listInterface(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData);

bool cb_getInterfaceGuid(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData);

// Public functions

void wifi_handler_ifaceListing(HANDLE hWlanClient, WifiInterfaceListingParameters params) {
	DWORD dwResult = wifi_iface_iterateAll(hWlanClient, &cb_listInterface, &params);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Failed to list wlan interfaces !\n");
	}
}

bool wifi_handler_getGuidFromListing(HANDLE hWlanClient, GUID *pOutputGuid, char* desiredGuid, int desiredIndex) {
	if(hWlanClient == NULL || pOutputGuid == NULL) {
		return false;
	}
	
	// Preparing the parameters.
	struct wifi_cli_iface_guid_getter_params guidGetterParams;
	guidGetterParams.wasFound = false;
	guidGetterParams.pOutputGuid = pOutputGuid;
	if(desiredGuid != NULL) {
		// We need to convert the string for the comparison later.
		guidGetterParams.desiredGuid = charStringToWChar(desiredGuid);
		if(guidGetterParams.desiredGuid == NULL) {
			fprintf(stderr, "Unable to convert the desired GUID's string !  (Error #%d)\n", errno);
			return false;
		}
		
		trace_println("We parsed the GUID: %ws", guidGetterParams.desiredGuid);
	} else {
		guidGetterParams.desiredGuid = NULL;
		trace_println("We didn't parse a GUID, we are index-boys up in here, safety is off baby !");
	}
	guidGetterParams.desiredIndex = desiredIndex;
	
	DWORD dwResult = wifi_iface_iterateAll(hWlanClient, &cb_getInterfaceGuid, &guidGetterParams);
	
	if(desiredGuid != NULL) {
		free(guidGetterParams.desiredGuid);
	}
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Failed to list wlan interfaces !\n");
		return false;
	}
	
	return guidGetterParams.wasFound;
}

// Private functions

bool cb_listInterface(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData) {
	int iRet = 1;
	WCHAR GuidString[40] = {0};
	
	// "Should" be optimized when compiling to make direct calls to it instead.
	WifiInterfaceListingParameters *params = extraCbData;
	
	// We only grab the GUID if needed later.
	if(params->showGuid || params->doDefault) {
		iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39);
	}
	
	if (iRet == 0) {
		fprintf(stderr, "Unable to grab GUID for interface #%d", ifaceIndex);
	} else {
		if(params->doDefault) {
			wprintf(L"[%d]%hs%ws%hs%ws\n", ifaceIndex, params->separator, GuidString, params->separator,
					pIfInfo->strInterfaceDescription);
		} else {
			bool hasPrintedSomethingYet = false;
			
			if(params->showIndex) {
				wprintf(L"%d", ifaceIndex);
				hasPrintedSomethingYet = true;
			}
			if(params->showGuid) {
				wprintf(L"%hs%ws", hasPrintedSomethingYet ? params->separator : "", GuidString);
				hasPrintedSomethingYet = true;
			}
			if(params->showDescription) {
				wprintf(L"%hs%ws", hasPrintedSomethingYet ? params->separator : "", pIfInfo->strInterfaceDescription);
				hasPrintedSomethingYet = true;
			}
			if(params->showState) {
				wprintf(L"%hs%d", hasPrintedSomethingYet ? params->separator : "", pIfInfo->isState);
				hasPrintedSomethingYet = true;
			} else if(params->showStateText) {
				wprintf(L"%hs%ws", hasPrintedSomethingYet ? params->separator : "", wifi_iface_formatState(pIfInfo->isState));
			}
			
			printf("\n");
		}
	}
	
	return true;
}

bool cb_getInterfaceGuid(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData) {
	struct wifi_cli_iface_guid_getter_params *guidGetterParams = extraCbData;
	
	// We check if we are working with an index to skip early.
	if(guidGetterParams->desiredGuid == NULL && guidGetterParams->desiredIndex != ifaceIndex) {
		return true;
	}
	
	// We are either working with an un-compared string or the right index.
	WCHAR GuidString[40] = {0};
	int iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39);
	
	if (iRet == 0) {
		fprintf(stderr, "Unable to grab GUID for interface #%d\n", ifaceIndex);
		return false;
	}
	
	bool isCorrespondingGuid = false;
	
	if(guidGetterParams->desiredGuid == NULL) {
		// We just need to return the GUID
		isCorrespondingGuid = true;
	} else {
		// We need to compare the string.
		trace_println("Comparing '%ws' and '%ws'...", GuidString, guidGetterParams->desiredGuid);
		isCorrespondingGuid = wcscmp(GuidString, guidGetterParams->desiredGuid) == 0;
		trace_println("Result: %d", isCorrespondingGuid);
	}
	
	if(isCorrespondingGuid) {
		guidGetterParams->wasFound = true;
		
		// We just copy the GUID's fields to prevent from having a null pointer later on, and so we don't have to
		//  deal with more pointers to free in order to reduce the spaghetti.
		memcpy(guidGetterParams->pOutputGuid, &pIfInfo->InterfaceGuid, sizeof(GUID));
	}
	
	return !isCorrespondingGuid;
}
