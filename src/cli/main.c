#include <stdbool.h>
#include <stdio.h>

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#ifndef NP_WIFI_NO_UTF8_CODE_PAGE
#include <locale.h>
#endif

#include "../../libs/nibblepoker-c-goodies/src/arguments/arguments.h"
#include "../../libs/nibblepoker-c-goodies/src/arguments/help.h"
#include "../../libs/nibblepoker-c-goodies/src/debug.h"
#include "../../libs/nibblepoker-c-goodies/src/text.h"

#include "./exitCodes.h"
#include "./handlers/iface.h"
#include "./handlers/profile.h"

static Verb *rootVerb, *ifaceRootVerb, *ifacesRootVerb, *ifacesListVerb, *ifaceProfilesVerb, *ifaceProfilesListVerb, *ifaceProfileVerb;
static Verb *ifaceProfileDelete, *ifaceProfilesDelete, *ifacesProfilesDelete;

// Recursively shared options
static Option *helpOption;

// Partially shared options
static Option *textDelimiterOption, *ifaceGuidOption, *ifaceProfileNameOption;
static Option *ifaceProfileNameAsNameOption, *ifaceProfileNameAsIndexOption;

// Root's options
static Option *buildInfoOption, *versionInfoOption, *versionOnlyInfoOption;

// Iface listing formatting options
static Option *ifaceListShowAllOption, *ifaceListShowIndexOption, *ifaceListShowGuidOption, *ifaceListShowDescriptionOption,
		*ifaceListShowStateOption, *ifaceListShowFormattedStateOption;
static Option *ifaceProfileShowNameOption, *ifaceProfileShowFlagsOption, *ifaceProfileShowFormattedFlagsOption;

bool prepareLaunchArguments() {
	// Root verb
	rootVerb = args_createVerb(L"root", L"Test");
	
	// Recursively shared options
	// TODO: Add a args_registerRecursiveOption !
	helpOption = args_createOption(
			'h', L"help", L"Shows this help text and exit.", FLAG_OPTION_STOPS_PARSING);
	
	// Partially shared options
	textDelimiterOption = args_createOption(
			'D', L"delimiter",
			L"Uses the given text as the delimiter in listings. (Defaults: \" - \" for generic listings, \";\" for selective ones)",
			FLAG_OPTION_HAS_VALUE);
	
	ifaceGuidOption = args_createOption(
			'G', L"guid",
			L"GUID associated with the Wi-Fi interface that is being interacted with.  (Can be given as the internal listing index)",
			FLAG_OPTION_HAS_VALUE | FLAG_OPTION_DEFAULT | FLAG_OPTION_REQUIRED | FLAG_OPTION_ALLOW_VERBS_AFTER);
	
	ifaceProfileNameOption = args_createOption(
			'P', L"profile-name",
			L"Wi-Fi interface's profile name associated that is being interacted with.  (Can be given as the internal listing index)",
			FLAG_OPTION_HAS_VALUE | FLAG_OPTION_DEFAULT | FLAG_OPTION_REQUIRED | FLAG_OPTION_ALLOW_VERBS_AFTER);
	ifaceProfileNameAsNameOption = args_createOption(
			'\0', L"as-name",
			L"Forces interpretation of the given profile's \"name/index\" as a name only.",
			FLAG_OPTION_ALLOW_VERBS_AFTER);
	ifaceProfileNameAsIndexOption = args_createOption(
			'\0', L"as-index",
			L"Forces interpretation of the given profile's \"name/index\" as an index only.",
			FLAG_OPTION_ALLOW_VERBS_AFTER);
	
	// Root's options
	buildInfoOption = args_createOption(
			'b', L"build-info", L"Shows the building info for this program and exit.", FLAG_OPTION_STOPS_PARSING);
	versionInfoOption = args_createOption(
			'v', L"version", L"Shows the program's version and exit.", FLAG_OPTION_STOPS_PARSING);
	versionOnlyInfoOption = args_createOption(
			'V', L"version-only", L"Shows the program's version in a basic numeric format and exit.",
			FLAG_OPTION_STOPS_PARSING);
	
	// "iface" and "ifaces" verb and sub-verbs.
	ifacesRootVerb = args_createVerb(L"ifaces", L"Operations that affect all interfaces.");
	ifacesListVerb = args_createVerb(L"list", L"Lists the interface in a human or machine readable format.");
	
	ifaceRootVerb = args_createVerb(L"iface", L"Operations that affect a specific interface.");
	ifaceProfileVerb = args_createVerb(L"profile", L"Interacts with a profile associated with an interface.");
	ifaceProfilesVerb = args_createVerb(L"profiles", L"Interacts with all the profiles associated with an interface.");
	ifaceProfilesListVerb = args_createVerb(L"list", L"Lists the interface in a human or machine readable format.");
	
	ifaceProfileDelete = args_createVerb(L"delete", L"Deletes a given profile from the given interface");
	ifaceProfilesDelete = args_createVerb(L"delete", L"Deletes all the profiles from the given interface");
	
	// TODO: Implement it !
	//ifacesProfilesDelete = args_createVerb(L"delete", L"Deletes all the profiles from all the interfaces");
	
	// Somewhat shared
	ifaceListShowAllOption = args_createOption(
			'a', L"show-all", L"Shows all the possible fields.", FLAG_OPTION_NONE);
	ifaceListShowIndexOption = args_createOption(
			'i', L"show-index", L"Shows the interface's GUID.", FLAG_OPTION_NONE);
	
	// ifaces only
	ifaceListShowGuidOption = args_createOption(
			'g', L"show-guid", L"Shows the interface's index during the listing.", FLAG_OPTION_NONE);
	ifaceListShowDescriptionOption = args_createOption(
			'd', L"show-description", L"Shows the interface's description.", FLAG_OPTION_NONE);
	ifaceListShowStateOption = args_createOption(
			's', L"show-state", L"Shows the interface's state.", FLAG_OPTION_NONE);
	ifaceListShowFormattedStateOption = args_createOption(
			'S', L"show-state-text", L"Shows the interface's state in a readable format.", FLAG_OPTION_NONE);
	
	// profiles only
	ifaceProfileShowFlagsOption = args_createOption(
			'f', L"show-flags", L"Shows the profile info's flags.  (As an unsigned 32bit integer)", FLAG_OPTION_NONE);
	ifaceProfileShowFormattedFlagsOption = args_createOption(
			'F', L"show-flags-text", L"Shows the profile info's flags in a readable format.", FLAG_OPTION_NONE);
	ifaceProfileShowNameOption = args_createOption(
			'n', L"show-name", L"Shows the profile info's name.", FLAG_OPTION_NONE);
	
	return rootVerb != NULL && helpOption != NULL && args_registerOption(buildInfoOption, rootVerb) &&
		   args_registerOption(versionInfoOption, rootVerb) &&
		   args_registerOption(versionOnlyInfoOption, rootVerb) &&
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
		   args_registerOption(ifaceGuidOption, ifaceRootVerb) &&
		   // wifi iface <GUID/Index> profile <ProfileName/Index> [--as-name|--as-index] [...]
		   args_registerVerb(ifaceProfileVerb, ifaceRootVerb) &&
		   args_registerOption(ifaceProfileNameOption, ifaceProfileVerb) &&
		   args_registerOption(ifaceProfileNameAsNameOption, ifaceProfileVerb) &&
		   args_registerOption(ifaceProfileNameAsIndexOption, ifaceProfileVerb) &&
		   // wifi iface <GUID/Index> profile <ProfileName/Index> [--as-name|--as-index] delete
		   args_registerVerb(ifaceProfileDelete, ifaceProfileVerb) &&
		   // wifi iface <GUID/Index> profiles [...]
		   args_registerVerb(ifaceProfilesVerb, ifaceRootVerb) &&
		   // wifi iface <GUID/Index> profiles list [...]
		   args_registerVerb(ifaceProfilesListVerb, ifaceProfilesVerb) &&
		   args_registerOption(ifaceListShowAllOption, ifaceProfilesListVerb) &&
		   args_registerOption(textDelimiterOption, ifaceProfilesListVerb) &&
		   args_registerOption(ifaceProfileShowFlagsOption, ifaceProfilesListVerb) &&
		   args_registerOption(ifaceProfileShowFormattedFlagsOption, ifaceProfilesListVerb) &&
		   args_registerOption(ifaceListShowIndexOption, ifaceProfilesListVerb) &&
		   args_registerOption(ifaceProfileShowNameOption, ifaceProfilesListVerb) &&
		   // wifi iface <GUID/Index> profiles delete
		   args_registerVerb(ifaceProfilesDelete, ifaceProfilesVerb) &&
		   // The -h, --help
		   args_registerOptionRecursively(helpOption, rootVerb);
}

/**
 * Check if the application was ran through double-clicking it or via a command prompt.
 * @return `true` if was ran through double-clicking, `false` otherwise.
 */
bool isProgramRunDirectly() {
	DWORD dwProcessList;
	return GetConsoleProcessList(&dwProcessList, 2) <= 1;
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
#ifndef NP_WIFI_NO_UTF8_CODE_PAGE
	// Allows emojis in SSIDs and profiles' names to be properly shown later on.
	// It also allows the original code page to be restored when exiting.
	setlocale(LC_ALL, "en_US.UTF-8");
	
	UINT originalCodePage = GetConsoleCP();
	UINT originalOutputCodePage = GetConsoleOutputCP();
	
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif
	
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
			errorCode = (enum wifi_exit_codes) parserError;
			goto END_CLEAN_ROOT_VERB;
		}
		
		trace_wprintln("> lastUsedVerb: %s", lastUsedVerb->name);
	}
	
	// Interpreting potentially exiting launch arguments.
	if(argc <= 1 || helpOption->occurrences) {
		trace_println("Printing the help text...");
		
		args_printHelpText(lastUsedVerb, L"ligma.exe", 80);
		
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
				   5 /*NP_BUILD_WIFI_CLI_VERSION_PATCH*/, NP_BUILD_WIFI_CLI_VERSION_EXTRA % 100);
		} else {
			printf("No options or action given, please use -h or --help for more info.");
		}
		goto END_CLEAN_ROOT_VERB;
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
		trace_println("We need to verify the given iface GUID !");
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
				fprintf(stderr, "Unable to convert the given interface index into a valid number !\n");
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
	
	// If the '-P|--profile-name' option was used, it means it will be required later on, so we need to verify it now.
	// If the option was required but not given, the parsing process will fail earlier and this code won't be reached.
	wchar_t *ifaceProfileName = NULL;
	if(args_wasOptionUsed(ifaceProfileNameOption)) {
		trace_println("We need to verify the given iface profile name !");
		trace_wprintln("> Input as wchar_t: '%wc'", ((wchar_t*) ifaceProfileNameOption->arguments->first->data)[0]);
		
		// This is a special case, we cannot be sure the user has given us the index or name unless they gave us a
		//  special hint flag.
		// If none were given, we'll do the following things:
		//  * Attempt to parse the profile's name as a number
		//  * If it didn't work, we'll only consider it as a name
		//  * If it worked, we'll check for both
		//  * If a profile was found only by their name or index, we continue.
		//  * If a profile was found by their name and index, we stop due to potential ambiguity.
		
		// Checking if we don't have both hint flags, I don't want yet another "X takes precedence over Y" scenario...
		if(args_wasOptionUsed(ifaceProfileNameAsNameOption) && args_wasOptionUsed(ifaceProfileNameAsIndexOption)) {
			fprintf(stderr, "Both '--%ws' and '--%ws' were given, only one should be at most !\n",
					ifaceProfileNameAsNameOption->name, ifaceProfileNameAsIndexOption->name);
			errorCode = 110;
			goto END_CLOSE_WLAN_HANDLE;
		}
		
		// If not told otherwise, we attempt to parse the supposed number fully.
		int desiredIndex = -1;
		if(!args_wasOptionUsed(ifaceProfileNameAsNameOption)) {
			errno = 0;
			char *nbrConversionTmp;
			desiredIndex = strtol(ifaceProfileNameOption->arguments->first->data, &nbrConversionTmp, 10);
			
			if(nbrConversionTmp == ifaceProfileNameOption->arguments->first->data || *nbrConversionTmp != '\0' ||
			   ((desiredIndex == LONG_MIN || desiredIndex == LONG_MAX) && errno == ERANGE)) {
				trace_println("Unable to convert the given profile name into a number");
				
				// In the event we could only use the index, we report the issue to the user.
				if(args_wasOptionUsed(ifaceProfileNameAsIndexOption)) {
					fprintf(stderr, "Unable to convert the given profile index into a valid number !\n");
					errorCode = 111;
					goto END_CLOSE_WLAN_HANDLE;
				}
				
				// We reset the number to say it won't be used.
				desiredIndex = -1;
			}
			trace_println("> Input as int: '%d'", desiredIndex);
		}
		
		// If not told otherwise, we convert the given name into a wchar_t string to make later comparisons easier.
		if(!args_wasOptionUsed(ifaceProfileNameAsIndexOption)) {
			ifaceProfileName = copyWCharString(ifaceProfileNameOption->arguments->first->data);
			
			// If it failed, we just assume that a critical error occurred.
			// We don't really care about the '--as-name' flag since we fail in all cases here.
			if(ifaceProfileName == NULL) {
				fprintf(stderr, "Unable to convert and copy the given profile name for internal uses !\n");
				errorCode = 112;
				goto END_CLOSE_WLAN_HANDLE;
			}
			
			trace_wprintln("> Input as wchar_t*: '%ws'", ifaceProfileName);
			
			// TODO: The rest
		}
		
		// We now attempt to get the profile name from the input.
		// If it ain't NULL, we're good and just need to free the old value in `ifaceProfileName` and move this new one.
		wchar_t *foundProfileName = wifi_handler_getProfileNameFromInput(hWlanClient, &ifaceGuid, desiredIndex, ifaceProfileName);
		
		if(foundProfileName == NULL) {
			fprintf(stderr, "Unable to find a profile with the given index or name !\n");
			errorCode = 113;
			goto END_CLOSE_WLAN_HANDLE;
		}
		
		// We're in the clear.
		free(ifaceProfileName);
		ifaceProfileName = foundProfileName;
		
		trace_wprintln("Found the profile named '%ws' !", ifaceProfileName);
	}
	
	// I couldn't use a switchcase, and I haven't implemented the "extra-data" parameter to the args yet so this will
	//  have to do, RIP the nice looking code...
	if(lastUsedVerb == ifaceRootVerb) {
		// wifi.exe iface [flags...]
		printf("Hello, World! 123\n");
		
	} else if(lastUsedVerb == ifacesListVerb) {
		// Preparing formatting info structure.
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
		
		trace_println("Calling 'wifi_handler_ifaceListing'...");
		wifi_handler_ifaceListing(hWlanClient, formattingParams);
		
		free(formattingParams.separator);
	} else if(lastUsedVerb == ifaceProfilesListVerb) {
		// Preparing formatting info structure.
		WifiInterfaceProfileListingParameters formattingParams;
		formattingParams.showIndex =
				ifaceListShowIndexOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.showName =
				ifaceProfileShowNameOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.showFlags =
				ifaceProfileShowFlagsOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.showFlagsText =
				ifaceProfileShowFormattedFlagsOption->occurrences > 0 || ifaceListShowAllOption->occurrences > 0;
		formattingParams.doDefault =
				!formattingParams.showIndex && !formattingParams.showName && !formattingParams.showFlags &&
				!formattingParams.showFlagsText;
		formattingParams.separator = copyString(
				textDelimiterOption->occurrences > 0 ? dllist_selectFirstData(textDelimiterOption->arguments) :
				formattingParams.doDefault ? " - " : ";");
		
		trace_println("Calling 'wifi_handler_ifaceProfilesListing'...");
		wifi_handler_ifaceProfilesListing(hWlanClient, &ifaceGuid, formattingParams);
		
		free(formattingParams.separator);
	} else if(lastUsedVerb == ifaceProfileDelete) {
		// Deleting a single profile from an interface
		DWORD dwProfileDeletionResult = WlanDeleteProfile(hWlanClient, &ifaceGuid, ifaceProfileName, NULL);
		if(dwProfileDeletionResult != ERROR_SUCCESS) {
			if(dwProfileDeletionResult == ERROR_ACCESS_DENIED) {
				fprintf(stderr, "Unable to delete '%ws' profile due to access restrictions !", ifaceProfileName);
				errorCode = 200;
			} else {
				fprintf(stderr, "Unable to delete '%ws' profile !", ifaceProfileName);
				errorCode = 201;
			}
		} else {
			wprintf(L"Successfully deleted the '%ws' profile !", ifaceProfileName);
		}
	} else if(lastUsedVerb == ifaceProfilesDelete) {
		// Deleting all profiles from an interface
		
		// Unable to delete one or more ...
	} else {
		fprintf(stderr, "The requested action wasn't processed in any way !\n");
	}
	
	// Exiting procedure
	// The order is important here in order to make it simple to quit the app with "goto"s
	
	END_CLOSE_WLAN_HANDLE:
	trace_println("Closing Windows's Wlan Server handle...");
	WlanCloseHandle(hWlanClient, NULL);
	
	END_CLEAN_ROOT_VERB:
	trace_println("Cleaning verbs and options...");
	args_freeVerb(rootVerb);
	
	// Cleaning other variables that may have memory allocated to them.
	trace_println("Cleaning other variables...");
	//if(ifaceProfileName != NULL) {
	//	free(ifaceProfileName);
	//}
	
	// FIXME: Check if it triggers when called from another language or a batch file !
	// FIXME: Appears to silently crash the program once out of CLion...
	// This might be caused by the function detecting the console since CLion appeared to trigger this one :/
	// And it worked nicely when double clicked since it was the same "course" as Clion...
	//if(isProgramRunDirectly()) {
	//	printf("Press any key to continue...\n");
	//	//getchar();
	//}

#ifndef NP_WIFI_NO_UTF8_CODE_PAGE
	trace_println("Setting console's code page back to %d & %d...", originalCodePage, originalOutputCodePage);
	SetConsoleCP(originalCodePage);
	SetConsoleOutputCP(originalOutputCodePage);
#endif
	
	trace_println("End of main(...) !");
	return errorCode;
}
