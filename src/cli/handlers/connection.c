#include "connection.h"

// Private structures

struct wifi_cli_iface_disconnect {
	HANDLE hWlanClient;
	bool continueOnError;
	bool reportErrorsInStdout;
	DWORD lastError;
};

// Private prototypes

bool cb_disconnect(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData);

// Public functions

DWORD wifi_handler_disconnectFromAll(HANDLE hWlanClient, bool continueOnError, bool reportErrorsInStdout) {
	trace_println("Disconnecting from all interfaces...");
	
	struct wifi_cli_iface_disconnect disconnectParams;
	disconnectParams.hWlanClient = hWlanClient;
	disconnectParams.continueOnError = continueOnError;
	disconnectParams.reportErrorsInStdout = reportErrorsInStdout;
	disconnectParams.lastError = ERROR_SUCCESS;
	
	DWORD dwResult = wifi_iface_iterateAll(hWlanClient, &cb_disconnect, &disconnectParams);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(reportErrorsInStdout ? stdout : stderr,
				"Failed to iterate over wlan interfaces !  (Win32 Error %lu)\n", dwResult);
		return dwResult;
	}
	
	return disconnectParams.lastError;
}

// Private functions

bool cb_disconnect(int ifaceIndex, PWLAN_INTERFACE_INFO pIfInfo, void *extraCbData) {
	// Should be optimized during compilation.
	struct wifi_cli_iface_disconnect *params = extraCbData;
	
	DWORD dwDisconnectResult = WlanDisconnect(params->hWlanClient, &pIfInfo->InterfaceGuid, NULL);
	
	if(dwDisconnectResult != ERROR_SUCCESS) {
		// Printing the error to the appropriate output.
		fprintf(params->reportErrorsInStdout ? stdout : stderr,
				"Unable to disconnect from the \"%ws\" interface !  (Win32 Error %lu)\n", pIfInfo->strInterfaceDescription, dwDisconnectResult);
		
		// Relaying back the error.
		params->lastError = dwDisconnectResult;
		
		// Checking if we should continue.
		return params->continueOnError;
	}
	
	// Normal execution's return value.
	return true;
}
