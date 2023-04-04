#include "interfaces.h"

DWORD wifi_iface_iterateAll(HANDLE hClient, bool (*callback)(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData), void *extraCbData) {
	// Making sure we can properly and safely go forward.
	if(hClient == NULL || callback == NULL) {
		error_println("One of the parameters was NULL !  (hClient: %p, callback: %p)", hClient, callback);
		return ERROR_INVALID_PARAMETER;
	}
	
	// Preparing some variables.
	DWORD dwResult = ERROR_SUCCESS;
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	
	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if(dwResult == ERROR_SUCCESS) {
		for(int i = 0; i < (int) pIfList->dwNumberOfItems; i++) {
			if(!callback(i, (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i], extraCbData)) {
				// If the callback tells us to stop iterating, we do.
				// The condition is more like `if(!shouldContinue)`, but its the same...
				i = (int) pIfList->dwNumberOfItems;
			}
		}
		
		// TODO: Is this condition required since we check for errors ?
		if(pIfList != NULL) {
			WlanFreeMemory(pIfList);
		}
	}
	
	return dwResult;
}

PWCHAR wifi_iface_formatState(WLAN_INTERFACE_STATE isState) {
	switch(isState) {
		case wlan_interface_state_not_ready:
			return L"Not ready";
		case wlan_interface_state_connected:
			return L"Connected";
		case wlan_interface_state_ad_hoc_network_formed:
			return L"First node in a ad hoc network";
		case wlan_interface_state_disconnecting:
			return L"Disconnecting";
		case wlan_interface_state_disconnected:
			return L"Not connected";
		case wlan_interface_state_associating:
			return L"Attempting to associate with a network";
		case wlan_interface_state_discovering:
			return L"Auto configuration is discovering settings for the network";
		case wlan_interface_state_authenticating:
			return L"In process of authenticating";
		default:
			return L"Unknown state";
	}
}
