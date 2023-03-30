#include <stdbool.h>
#include <stdio.h>

#include "libs/nibblepoker-c-goodies/src/arguments/arguments.h"

static Verb *rootVerb;
static Option *helpOption;

bool prepareLaunchArguments() {
	rootVerb = args_createVerb("root", "Test");
	helpOption = args_createOption('h', "help", "Shows this help text and exit", FLAG_OPTION_STOPS_PARSING);
	
	return rootVerb != NULL && helpOption != NULL && args_registerOption(helpOption, rootVerb);
}

int main(int argc, char **argv) {
	// We prepare and attempt to parse the launch arguments.
	if(!prepareLaunchArguments()) {
		fprintf(stderr, "Failed to prepare the launch arguments parser !");
		return 1;
	}
	if(argc > 1) {
		args_parseArguments(rootVerb, argv, 1, argc);
	}
	
	// Interpreting the launch arguments.
	if(helpOption->occurrences) {
		printf("We should print the help text !\n");
	}
	
	printf("Hello, World!\n");
	return 0;
}
