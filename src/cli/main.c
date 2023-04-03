#include <stdbool.h>
#include <stdio.h>

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include "../../libs/nibblepoker-c-goodies/src/arguments/arguments.h"
#include "../../libs/nibblepoker-c-goodies/src/debug.h"
#include "../../libs/nibblepoker-c-goodies/src/text.h"

#include "./exitCodes.h"
#include "./handlers/iface.h"

static Verb *rootVerb, *ifaceRootVerb, *ifaceListVerb;

// Recursively shared options
static Option *helpOption;

// Partially shared options
static Option *textDelimiterOption;

// Root's options
static Option *buildInfoOption, *versionInfoOption, *versionOnlyInfoOption;

// Iface listing formatting options
static Option *ifaceListShowAllOption, *ifaceListShowIndexOption, *ifaceListShowGuidOption, *ifaceListShowDescriptionOption,
		*ifaceListShowStateOption, *ifaceListShowFormattedStateOption;

bool prepareLaunchArguments() {
	// Root verb
	rootVerb = args_createVerb("root", "Test");
	
	// Recursively shared options
	// TODO: Add a args_registerRecursiveOption !
	helpOption = args_createOption(
			'h', "help", "Shows this help text and exit.", FLAG_OPTION_STOPS_PARSING);
	
	// Partially shared options
	textDelimiterOption = args_createOption(
			'D', "delimiter",
			"Uses the given text as the delimiter in listings. (Defaults: \" - \" for generic listings, \";\" for selective ones)",
			FLAG_OPTION_HAS_VALUE);
	
	// Root's options
	buildInfoOption = args_createOption(
			'b', "build-info", "Shows the building info for this program and exit.", FLAG_OPTION_STOPS_PARSING);
	versionInfoOption = args_createOption(
			'v', "version", "Shows the program's version and exit.", FLAG_OPTION_STOPS_PARSING);
	versionOnlyInfoOption = args_createOption(
			'V', "version-only", "Shows the program's version in a basic numeric format and exit.",
			FLAG_OPTION_STOPS_PARSING);
	
	// Tier-1 "iface" verb.
	ifaceRootVerb = args_createVerb("iface", "Test 123");
	ifaceListVerb = args_createVerb("list", "Reeeee !");
	
	ifaceListShowAllOption = args_createOption(
			'a', "show-all", "Shows all the possible fields.  (Same as -igds)", FLAG_OPTION_NONE);
	ifaceListShowIndexOption = args_createOption(
			'i', "show-index", "Shows the interface's GUID.", FLAG_OPTION_NONE);
	ifaceListShowGuidOption = args_createOption(
			'g', "show-guid", "Shows the interface's index during the listing.", FLAG_OPTION_NONE);
	ifaceListShowDescriptionOption = args_createOption(
			'd', "show-description", "Shows the interface's description.", FLAG_OPTION_NONE);
	ifaceListShowStateOption = args_createOption(
			's', "show-state", "Shows the interface's state.", FLAG_OPTION_NONE);
	ifaceListShowFormattedStateOption = args_createOption(
			'S', "show-state-text", "Shows the interface's state in a readable format.", FLAG_OPTION_NONE);
	
	return rootVerb != NULL && helpOption != NULL && args_registerOption(buildInfoOption, rootVerb) &&
		   args_registerOption(versionInfoOption, rootVerb) &&
		   args_registerOption(versionOnlyInfoOption, rootVerb) &&
		   args_registerOption(helpOption, rootVerb) &&
		   args_registerVerb(ifaceRootVerb, rootVerb) &&
		   args_registerVerb(ifaceListVerb, ifaceRootVerb) &&
		   args_registerOption(ifaceListShowAllOption, ifaceListVerb) &&
		   args_registerOption(ifaceListShowIndexOption, ifaceListVerb) &&
		   args_registerOption(ifaceListShowGuidOption, ifaceListVerb) &&
		   args_registerOption(ifaceListShowDescriptionOption, ifaceListVerb) &&
		   args_registerOption(ifaceListShowStateOption, ifaceListVerb) &&
		   args_registerOption(ifaceListShowFormattedStateOption, ifaceListVerb) &&
		   args_registerOption(textDelimiterOption, ifaceListVerb);
}

int main(int argc, char **argv) {
	// ???
	// A pointer to this value will be passed around to make it easier to show error codes later on.
	enum wifi_exit_codes errorCode = WIFI_EXIT_CODE_NO_ERROR;
	
	// We prepare and attempt to parse the launch arguments.
	Verb *lastUsedVerb = NULL;
	
	if(!prepareLaunchArguments()) {
		fprintf(stderr, "Failed to prepare the launch arguments parser !");
		return 1;
	}
	if(argc > 1) {
		enum EArgumentParserErrors parserError = args_parseArguments(rootVerb, argv, 1, argc, &lastUsedVerb);
		if(parserError != ERROR_ARGUMENTS_NONE) {
			fprintf(stderr, "Unable to parse launch arguments !  (Error #%d) \n", parserError);
			return parserError;
		}
	}
	
	// Interpreting potentially exiting launch arguments.
	if(helpOption->occurrences) {
		printf("We should print the help text !\n");
		
		// TODO args_printHelpText(lastUsedVerb);
		
		return WIFI_EXIT_CODE_NO_ERROR;
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
		return WIFI_EXIT_CODE_NO_ERROR;
	} else {
		//printf("> %s\n", "123");
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
	// We can now process the launch parameters more thoroughly.
	
	// I couldn't use a switchcase, and I haven't implemented the "extra-data" parameter to the args yet so this will
	//  have to do, RIP the nice looking code...
	if(lastUsedVerb == ifaceRootVerb) {
		// wifi.exe iface [flags...]
		printf("Hello, World! 123\n");
		
	} else if(lastUsedVerb == ifaceListVerb) {
		// Preparing formatting info structure.
		// TODO: Optimize this piece of shit with binary flags or something. - Is it really worth it tho ?
		WifiInterfaceListingParameters formattingParams;
		formattingParams.showIndex =
				ifaceListShowIndexOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.showGuid =
				ifaceListShowGuidOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.showDescription =
				ifaceListShowDescriptionOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.showState =
				ifaceListShowStateOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.showStateText =
				ifaceListShowFormattedStateOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.doDefault =
				!formattingParams.showIndex && !formattingParams.showGuid && !formattingParams.showDescription &&
				!formattingParams.showState && !formattingParams.showStateText;
		formattingParams.separator = copyString(
				textDelimiterOption->occurrences > 0 ? dllist_selectFirstData(textDelimiterOption->arguments) :
				formattingParams.doDefault ? " - " : ";");
		
		wifi_handler_ifaceListing(hWlanClient, formattingParams);
		
		free(formattingParams.separator);
	}
	
	// Exiting procedure
	// The order is important here in order to make it simple to quit the app with "goto"s
	
	END_CLOSE_WLAN_HANDLE:
	WlanCloseHandle(hWlanClient, NULL);
	
	END_CLEAN_ROOT_VERB:
	args_freeVerb(rootVerb);
	
	return errorCode;
}
