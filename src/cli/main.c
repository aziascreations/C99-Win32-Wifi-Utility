#include <stdbool.h>
#include <stdio.h>

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include "../../libs/nibblepoker-c-goodies/src/arguments/arguments.h"
#include "../../libs/nibblepoker-c-goodies/src/debug.h"

static Verb *rootVerb;
static Option *helpOption, *buildInfoOption, *versionInfoOption, *versionOnlyInfoOption;

bool prepareLaunchArguments() {
	rootVerb = args_createVerb("root", "Test");
	buildInfoOption = args_createOption('b', "build-info", "Shows the building info for this program and exit",
										FLAG_OPTION_STOPS_PARSING);
	versionInfoOption = args_createOption('v', "version", "Shows the program's version and exit",
										  FLAG_OPTION_STOPS_PARSING);
	versionOnlyInfoOption = args_createOption('V', "version-only",
											  "Shows the program's version in a basic numeric format and exit",
											  FLAG_OPTION_STOPS_PARSING);
	helpOption = args_createOption('h', "help", "Shows this help text and exit", FLAG_OPTION_STOPS_PARSING);
	
	return rootVerb != NULL && helpOption != NULL && args_registerOption(buildInfoOption, rootVerb) &&
		   args_registerOption(versionInfoOption, rootVerb) && args_registerOption(versionOnlyInfoOption, rootVerb) &&
		   args_registerOption(helpOption, rootVerb);
}

int main(int argc, char **argv) {
	// ???
	// A pointer to this value will be passed around to make it easier to show error codes later on.
	int errorCode = 0;
	
	// We prepare and attempt to parse the launch arguments.
	Verb *lastUsedVerb = NULL;
	
	if(!prepareLaunchArguments()) {
		fprintf(stderr, "Failed to prepare the launch arguments parser !");
		return 1;
	}
	if(argc > 1) {
		args_parseArguments(rootVerb, argv, 1, argc, &lastUsedVerb);
	}
	
	// Interpreting potentially exiting launch arguments.
	if(helpOption->occurrences) {
		printf("We should print the help text !\n");
		
		// TODO args_printHelpText(lastUsedVerb);
		
		return 0;
	}
	
	if(lastUsedVerb == rootVerb) {
		if(buildInfoOption->occurrences) {
			printf("%s\n", "TODO !");
		} else if(versionInfoOption->occurrences) {
			printf("%s v%s\n", NP_BUILD_GENERAL_PROJECT_NAME, NP_BUILD_WIFI_CLI_VERSION);
		} else if(versionOnlyInfoOption->occurrences) {
			// The last number is "modulo-ed" with 100 to prevent it form being 3 integers long.
			printf("%02d%02d%02d%02d\n", NP_BUILD_WIFI_CLI_VERSION_MAJOR, NP_BUILD_WIFI_CLI_VERSION_MINOR,
				   NP_BUILD_WIFI_CLI_VERSION_PATCH, NP_BUILD_WIFI_CLI_VERSION_EXTRA % 100);
		} else {
			printf("Usage: ...");
		}
		return 0;
	} else {
		printf("> %s\n", "123");
	}
	
	trace_println("Getting handle to Windows's Wlan Server...");
	
	HANDLE hWlanClient = NULL;
	DWORD wlanCurrentApiVersion = 0;
	DWORD dwResult = WlanOpenHandle(2, NULL, &wlanCurrentApiVersion, &hWlanClient);
	
	if(dwResult != ERROR_SUCCESS) {
		fprintf(stderr, "Unable to get a WLAN API handle !  (%lu) \n", dwResult);
		errorCode = 1;
		goto END_CLEAN_ROOT_VERB;
	}
	
	// TODO: Add option to enforce strict API version negotiation.
	if(wlanCurrentApiVersion < 2) {
		fprintf(stderr, "Unable to get access to the version 2 of the WLAN API !  (Got %lu)", wlanCurrentApiVersion);
		errorCode = 2;
		goto END_CLOSE_WLAN_HANDLE;
	}
	
	// Assuming we didn't jump to the end.
	// We can now process the launch parameters more thoroughly
	// TODO: This !
	
	printf("Hello, World!\n");
	
	
	
	// Exiting procedure
	// The order is important here in order to make it simple to quit the app with "goto"s
	
	END_CLOSE_WLAN_HANDLE:
	WlanCloseHandle(hWlanClient, NULL);
	
	END_CLEAN_ROOT_VERB:
	args_freeVerb(rootVerb);
	
	return errorCode;
}
