#include "iface.h"

// Private prototypes

void cb_listInterface(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData);

void wifi_handler_ifaceListing(HANDLE hWlanClient, WifiInterfaceListingParameters params) {
	DWORD dwResult = wifi_iface_iterateAll(hWlanClient, &cb_listInterface, &params);
	
	if(dwResult != ERROR_SUCCESS) {
		printf("Failed to list wlan interfaces !");
	}
}

// Private functions

void cb_listInterface(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData) {
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
			wprintf(L"%ws%hs%ws\n", GuidString, params->separator, pIfInfo->strInterfaceDescription);
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
				// TODO: Add a text switch case !
			}
			
			printf("\n");
		}
	}
}
