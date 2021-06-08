#include "include/stdheaders.h"
#include "include/error_handler.h"
#include "include/data_manager.h"

void call_error(int ec, int pc)
{
	is_err = true;
	printf("\n------------------------------------------\n");
	printf("ERROR: ");
	if (pc != NO_LINE)
	{
		printf("at line %d\n", pc);
	}
	switch (ec)
	{
	/* FILE ERROR */
	case FILE_NOT_GIVEN:
		printf("At least 1 file name must be given as an argument.\n");
		exiting_msg();
		break;
	case FILE_NAME_EXCEEDED:
		printf("The file name is too long.\n");
		break;
	case FILE_UNABLE_TO_OPEN:
		printf("Unable to open file.\n");
		break;
	case FILE_UNABLE_TO_CREATE:
		printf("Unable to create output files.\n");
		break;
	case FILE_NOT_EXIST:
		printf("The specified file doesn't seem to exist.\n");
		break;

	/* INSTRUCTION ERRORS */
	case INSTRUCTION_INVALID_STRING:
		printf("The provided string is in invalid format.\n");
		break;
	case INSTRUCTION_NOT_EXIST:
		printf("The given instruction doesn't apear to exist.\n");
		break;
	case INSTRUCTION_MISSING_ARGS:
		printf("An instruction was given but it is missing arguments.\n");
		break;
	case INSTRUCTION_INVALID_DATA:
		printf("A %c%s instruction was given but the data type is not a valid number\n", INSTRUCTION_CHAR, DATA);
		break;
	case INSTRUCTION_INVALID_DATA_SYNTAX:
		printf("A %c%s instrction was given but the data syntax is incorect.\n", INSTRUCTION_CHAR, DATA);
		break;
	case INSTRUCTION_EXTERN_MISSING_LABEL:
		printf("An %c%s was given but a label is missing.\n", INSTRUCTION_CHAR, EXTERNAL);
		break;
	case INSTRUCTION_EXTERN_INVALID_LABEL:
		printf("An %c%s was given with an invalid label.\n", INSTRUCTION_CHAR, EXTERNAL);
		break;
	case INSTRUCTION_ENTRY_MISSING_LABEL:
		printf("An %c%s was given but a label is missing.\n", INSTRUCTION_CHAR, ENTRY);
		break;
	case INSTRUCTION_ENTRY_INVALID_LABEL:
		printf("An %c%s was given with an invalid label.\n", INSTRUCTION_CHAR, ENTRY);
		break;

	/* COMMAND ERRORS */
	case COMMAND_NOT_EXIST:
		printf("The given command doesn't seem to exist.\n");
		break;
	case COMMAND_INVALID_ARGS:
		printf("Invalid arguments with given command.\n");
		break;
	case COMMAND_EXCEEDS_ARGS:
		printf("Too many arguments were given for the command. You can provide at most 2 arguments.\n");
		break;
	case COMMAND_EXPECT_ZERO_ARGS:
		printf("The given command expects no arguments.\n");
		break;
	case COMMAND_EXPECT_ONE_ARGS:
		printf("The given command takes exacly 1 argument.\n");
		break;
	case COMMAND_EXPECT_TWO_ARGS:
		printf("The given command takes exacly 2 arguments.\n");
		break;
	case COMMAND_INVALID_FORMAT_ARGS:
		printf("The arguments are formated incorrectly.\n");
		break;
	case COMMAND_SRC_ARG_INVALID:
		printf("The given source (second) argument type is invalid.\n");
		break;
	case COMMAND_DES_ARG_INVALID:
		printf("The given destination (first) argument type is invalid.\n");
		break;

	/* REGISTER ERRORS */
	case REGISTER_NOT_EXIST:
		printf("Register doesn't exist.\n");
		printf("There are %d register r0 ... r%d\n", NUMBER_OF_REGISTERS, NUMBER_OF_REGISTERS);
		break;

	/* LABEL ERRORS */
	case LABEL_IS_INSTRUCTION:
		printf("The name of the label is a saved instruction.\n");
		break;
	case LABEL_IS_COMMAND:
		printf("The name of the label is a saved command.\n");
		break;
	case LABEL_IS_REGISTER:
		printf("The name of the label is a saved register.\n");
		break;
	case LABEL_ALREADY_EXISTS:
		printf("The label already exist.\n");
		break;
	case LABEL_EXCEEDS_LENGTH:
		printf("The name of the label exceeds the maximum length of: %d\n", MAX_LABEL);
		break;
	case LABEL_FIRST_CHAR_NOT_ALPHA:
		printf("the first character of a label must be an alphabetic character.\n");
		break;
	case LABEL_INVALID_FORMAT:
		printf("The label name must contain only alphanumeric characters.\n");
		break;
	case LABEL_UNRECOGNIZED:
		printf("The refered label is not recognize by the program.\n");
		break;
	case LABEL_EMPTY:
		printf("The label must contain at least 1 character.\n");
		break;

	/* MEMORY ALLOCATION ERROR */
	case UNABLE_TO_ALLOCATE_MEMORY:
		printf("Memory allocation failed.\n exiting...");
		exiting_msg();
		break;
	case RUN_OUT_MEMORY:
		printf("Run out out of memory... which is: %d\n", MAX_IMAGE_SIZE);
		exiting_msg();
		break;

	/* EXTRA */
	case LINE_MISSING_BODY:
		printf("Line missing command/instruction\n");
		break;
	case LINE_EXCEEDS_LENGTH:
		printf("The line exceeds the maximun length of a line which is: %d\n", MAX_LINE);
		break;

	default:
		printf("UNDEFINED ERROR\n");
		break;
	}
	printf("------------------------------------------\n");
}

void exiting_msg()
{
	printf("\nEXITING...\n");
	printf("------------------------------------------\n");
	exit(1);
}