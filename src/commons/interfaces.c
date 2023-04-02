#include "interfaces.h"

DWORD wifi_iface_iterateAll(HANDLE hClient, void (*callback)(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData), void *extraCbData) {
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
			callback(i, (WLAN_INTERFACE_INFO *) &pIfList->InterfaceInfo[i], extraCbData);
		}
		
		// FIXME: Is this condition required since we check for errors ?
		if(pIfList != NULL) {
			WlanFreeMemory(pIfList);
		}
	}
	
	return dwResult;
}
