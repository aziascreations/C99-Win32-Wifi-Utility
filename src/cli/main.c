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

static Verb *rootVerb, *ifaceRootVerb, *ifacesRootVerb, *ifacesListVerb, *ifaceProfileVerb;

// Recursively shared options
static Option *helpOption;

// Partially shared options
static Option *textDelimiterOption, *ifaceGuidOption;

// Root's options
static Option *buildInfoOption, *versionInfoOption, *versionOnlyInfoOption;

// Iface listing formatting options
static Option *ifaceListShowAllOption, *ifaceListShowIndexOption, *ifaceListShowGuidOption, *ifaceListShowDescriptionOption,
		*ifaceListShowStateOption, *ifaceListShowFormattedStateOption;
static Option *ifaceProfileShowNameOption, *ifaceProfileShowFlagsOption, *ifaceProfileShowFormattedFlagsOption;

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
	ifaceGuidOption = args_createOption(
			'G', "guid",
			"GUID associated with the Wi-Fi interface that is being interacted with.  (Can be given as the internal listing index)",
			FLAG_OPTION_HAS_VALUE | FLAG_OPTION_DEFAULT | FLAG_OPTION_REQUIRED);
	
	// Root's options
	buildInfoOption = args_createOption(
			'b', "build-info", "Shows the building info for this program and exit.", FLAG_OPTION_STOPS_PARSING);
	versionInfoOption = args_createOption(
			'v', "version", "Shows the program's version and exit.", FLAG_OPTION_STOPS_PARSING);
	versionOnlyInfoOption = args_createOption(
			'V', "version-only", "Shows the program's version in a basic numeric format and exit.",
			FLAG_OPTION_STOPS_PARSING);
	
	// Tier-1 "iface" verb.
	ifacesRootVerb = args_createVerb("ifaces", "Operations that affect all interfaces.");
	ifacesListVerb = args_createVerb("list", "Lists the interface in a human or machine readable format.");
	ifaceRootVerb = args_createVerb("iface", "Operations that affect a specific interface.");
	ifaceProfileVerb = args_createVerb("profile", "Interacts with the profiles associated with an interface.");
	// TODO: Add "profiles" to handle all with additional parameters shared with "iface list" 's ones.
	
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
	
	/*ifaceProfileShowNameOption = args_createOption(
			'n', "show-name", "Shows the profile info's name.", FLAG_OPTION_NONE);
	// TODO: Check if it can have some control chars like SSIDs, don't be like wmic once it encounters emojis.
	//ifaceProfileShowNameOption = args_createOption(
	//		'N', "show-name-hex", "Shows the profile info's name.", FLAG_OPTION_NONE);
	ifaceProfileShowFlagsOption = args_createOption(
			'f', "show-flags", "Shows the profile info's flags.  (As an unsigned 32bit integer)", FLAG_OPTION_NONE);
	ifaceProfileShowFormattedFlagsOption = args_createOption(
			'F', "show-flags-text", "Shows the profile info's flags in a readable format.", FLAG_OPTION_NONE);*/
	
	return rootVerb != NULL && helpOption != NULL &&args_registerOption(buildInfoOption, rootVerb) &&
		   args_registerOption(versionInfoOption, rootVerb) &&
		   args_registerOption(versionOnlyInfoOption, rootVerb) &&
		   args_registerOption(helpOption, rootVerb) &&
		   // wifi ifaces [...]
		   args_registerVerb(ifacesRootVerb, rootVerb) &&
		   // wifi ifaces list [...]
		   args_registerVerb(ifacesListVerb, ifacesRootVerb) &&
		   args_registerOption(ifaceListShowAllOption, ifacesListVerb) &&
		   args_registerOption(ifaceListShowIndexOption, ifacesListVerb) &&
		   args_registerOption(ifaceListShowGuidOption, ifacesListVerb) &&
		   args_registerOption(ifaceListShowDescriptionOption, ifacesListVerb) &&
		   args_registerOption(ifaceListShowStateOption, ifacesListVerb) &&
		   args_registerOption(ifaceListShowFormattedStateOption, ifacesListVerb) &&
		   args_registerOption(textDelimiterOption, ifacesListVerb) &&
		   // wifi iface <GUID/Index> [...]
		   args_registerVerb(ifaceRootVerb, rootVerb) &&
		   args_registerOption(ifaceGuidOption, ifaceProfileVerb) &&
		   // wifi iface <GUID/Index> profile [...]
		   args_registerVerb(ifaceProfileVerb, ifaceRootVerb);
}

/**
 * Check if the application was ran through double-clicking it or via a command prompt.
 * @return `true` if was ran through double-clicking, `false` otherwise.
 */
bool isProgramRunDirectly() {
	DWORD dwProcessList;
	return GetConsoleProcessList(&dwProcessList, 2) <= 1;
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
	if(argc <= 1 || helpOption->occurrences) {
		printf("We should print the help text !\n");
		
		// TODO args_printHelpText(lastUsedVerb);
		
		goto END_CLEAN_ROOT_VERB;
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
	
	// If the '-G|--guid' option was used, it means it will be required later on, so we need to verify it now.
	// If the option was required but not given, the parsing process will fail earlier and this code won't be reached.
	GUID ifaceGuid;
	if(args_wasOptionUsed(ifaceGuidOption)) {
		trace_println("We need to verify the iface GUID !");
		trace_println("> [%lu, %hu, %hu, [%d, %d, %d, %d, %d, %d, %d, %d]]", ifaceGuid.Data1, ifaceGuid.Data2,
					  ifaceGuid.Data3, ifaceGuid.Data4[0], ifaceGuid.Data4[1], ifaceGuid.Data4[2], ifaceGuid.Data4[3],
					  ifaceGuid.Data4[4], ifaceGuid.Data4[5], ifaceGuid.Data4[6], ifaceGuid.Data4[7]);
		
		if(!isInterfaceGuid(ifaceGuidOption->arguments->first->data)) {
			trace_println("We have the index");
			
			errno = 0;
			char *nbrConversionTmp;
			int desiredIndex = strtol(ifaceGuidOption->arguments->first->data, &nbrConversionTmp, 10);
			
			if(nbrConversionTmp == ifaceGuidOption->arguments->first->data || *nbrConversionTmp != '\0' ||
			   ((desiredIndex == LONG_MIN || desiredIndex == LONG_MAX) && errno == ERANGE)) {
				fprintf(stderr, "Unable to convert the given index into a valid number !\n");
				errorCode = 97;
				goto END_CLOSE_WLAN_HANDLE;
			}
			
			trace_println("> %d", desiredIndex);
			
			if(!wifi_handler_getGuidFromIndex(hWlanClient, &ifaceGuid, desiredIndex)) {
				fprintf(stderr, "Unable to find an interface with the given index !\n");
				errorCode = 98;
				goto END_CLOSE_WLAN_HANDLE;
			}
		} else {
			trace_println("We have the text-based GUID");
			if(!wifi_handler_getGuidFromGuid(hWlanClient, &ifaceGuid, ifaceGuidOption->arguments->first->data)) {
				fprintf(stderr, "Unable to find an interface with the given GUID !\n");
				errorCode = 99;
				goto END_CLOSE_WLAN_HANDLE;
			}
		}
		
		trace_println("> [%lu, %hu, %hu, [%d, %d, %d, %d, %d, %d, %d, %d]]", ifaceGuid.Data1, ifaceGuid.Data2,
					  ifaceGuid.Data3, ifaceGuid.Data4[0], ifaceGuid.Data4[1], ifaceGuid.Data4[2], ifaceGuid.Data4[3],
					  ifaceGuid.Data4[4], ifaceGuid.Data4[5], ifaceGuid.Data4[6], ifaceGuid.Data4[7]);
	}
	
	// I couldn't use a switchcase, and I haven't implemented the "extra-data" parameter to the args yet so this will
	//  have to do, RIP the nice looking code...
	if(lastUsedVerb == ifaceRootVerb) {
		// wifi.exe iface [flags...]
		printf("Hello, World! 123\n");
		
	} else if(lastUsedVerb == ifacesListVerb) {
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
	
	// FIXME: Check if it triggers when called from another language or a batch file !
	if(isProgramRunDirectly()) {
		printf("Press any key to continue...\n");
		getchar();
	}
	
	return errorCode;
}
