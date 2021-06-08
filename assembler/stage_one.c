/* this file handles all input related operations */

#include "include/stdheaders.h"
#include "include/data_manager.h"
#include "include/stage_one.h"
#include "include/error_handler.h"

/* this is input intiator acts like a constructor
@file_name - the assembly file name */
void input_initiator(const char *file_name)
{
	FILE *as;
	char line[MAX_LINE + 2]; /* content of line (+2 for new line and null byte) */
	size_t len = 0;			 /* length of line */
	int EC = SUCCESSFUL;	 /* ERROR CODE */
	char *fn;				 /* fn - file name */

	DC = INIT_DC; /* Data counter */
	IC = INIT_IC; /* Instruction counter */
	PC = INIT_PC; /* Program counter (line number) */
	is_err = false;

	/* Adding .as to the file */
	fn = add_extention(file_name, ASSEMBLY_EXTENTION);
	/* if problem opening file, return */
	if ((as = fopen(fn, "r")) == NULL)
	{
		call_error(FILE_UNABLE_TO_OPEN, NO_LINE);
		free(fn);
		return;
	}
	free(fn);
	/* read line until no more lines to read */
	while (fgets(line, MAX_LINE + 1, as) != NULL)
	{
		PC++;
		len = strlen(line);
		/* len of line not including new line */
		if (len - 1 > MAX_LINE)
		{
			call_error(LINE_EXCEEDS_LENGTH, PC);
		}

		/* making all line new-line terminated for easier handling */
		if (line[len - 1] != '\n')
		{
			line[len] = '\n';
			line[++len] = '\0';
		}

		/* if there is error call it */
		EC = line_analyzer(line);
		if (EC != SUCCESSFUL)
		{
			call_error(EC, PC);
		}
	}
	fclose(as);
}

/* this function anaylzes the line and breaks into appropriate parts
@line - the line you want to check
@return - error codes if there are error otherwise @SUCCESFUL (no err) is returned. */
int line_analyzer(char *line)
{
	/* make another pointer for data */
	/* char *line = data; */
	bool symbol = false;
	label lbl = {0};
	int EC = UNKNOWN;

	lbl.is_extern = false;
	lbl.is_entry = false;
	lbl.is_data = false;
	lbl.is_code = false;

	/* checking if first char is a comment (;) */
	if (*line == COMMENT_CHAR)
	{
		return SUCCESSFUL;
	}
	/* skiping whitespaces at the begining of line */
	line = skip_whitespace(line);
	/* if all the line is just white space */
	if (*line == '\0')
	{
		return SUCCESSFUL;
	}
	lbl.name = get_label(line);
	/* turning symbol flag */
	if (lbl.name != NULL)
	{
		EC = is_valid_label(lbl.name);
		if (EC != SUCCESSFUL)
		{
			return EC;
		}
		symbol = true;
		line = next_word(line);
	}
	if (*line == '\0')
	{
		return LINE_MISSING_BODY;
	}
	/* if instruction */
	if (*line == INSTRUCTION_CHAR)
	{
		EC = instruction_handler(line, &lbl);
		/* if an error occrued or if there no label or if there is an entry instruction go to next line */
		if (lbl.is_extern)
		{
			symbol = true;
		}
		if (EC != SUCCESSFUL || !symbol || lbl.is_entry)
		{
			return EC;
		}
	}
	/* if not instruction it's command */
	else
	{
		lbl.address = IC;
		EC = command_handler(line);
		if (EC != SUCCESSFUL)
		{
			return EC;
		}
		lbl.is_code = true;
	}
	/* adding label  */
	if (symbol)
	{
		if (is_label_exist(lbl.name))
		{
			return LABEL_ALREADY_EXISTS;
		}
		add_label(&lbl);
	}
	return SUCCESSFUL;
}

/* this function handles the command and the command's arguments
@param line the line which contains the command with or without the arguments
@param lbl if the @symbol flag is turned on @lbl will be add to the symbol table
@return the approprate error code.  
*/
int command_handler(char *line)
{
	command *cmd;
	code *item;
	/* for 2 args */
	char *argv[MAX_ARGS], *temp;
	char data_str[] = {DATA_CHAR, '\0'};
	int argc = 0;
	int EC = UNKNOWN;
	int taken_args; /* the number of arguments the command takes*/
	/* allocates memory */
	if ((cmd = malloc(sizeof(command))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	if ((item = malloc(sizeof(code))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	/* getting command name */
	temp = strdup(line);
	cmd->name = strtok(temp, SPACE_TOKEN);
	if (cmd->name == NULL)
	{
		EC = LINE_MISSING_BODY;
		goto EXIT;
	}
	/* checking if command exists */
	if (!is_saved_command(cmd->name))
	{
		EC = COMMAND_NOT_EXIST;
		goto EXIT;
	}

	cmd = (command *)(pointer_of(commands, cmd->name, (MATCH_CAST)match_command)->data);
	taken_args = (cmd->allowed_src != NULL) + (cmd->allowed_des != NULL);
	/* if first char is @DATA_CHAR (',') */
	line = next_word(line);
	if (*line == DATA_CHAR)
	{
		EC = COMMAND_INVALID_FORMAT_ARGS;
		goto EXIT;
	}
	if (taken_args != count_args(line))
	{
		/* // TODO add error for number of args */
		EC = COMMAND_INVALID_FORMAT_ARGS;
		goto EXIT;
	}
	/* if reached this point args are formated corectly i.e arg1,arg2 or arg1 or 'blank' */
	temp = strdup(line);
	argv[argc] = strtok(temp, data_str);
	while (argv[argc] != NULL)
	{
		/* if there are 2 or more words between each DATA_CHAR (,) */

		/* cleaning white space */
		argv[argc] = strtrim(argv[argc]);
		if (*next_word(argv[argc]) != '\0')
		{
			EC = COMMAND_INVALID_ARGS;
			goto EXIT;
		}
		/* next argument */
		argv[++argc] = strtok(NULL, data_str);
	}

	item->word.val = 0;
	item->word.encode.opcode = cmd->opcode;
	item->word.encode.func = cmd->func;
	switch (taken_args)
	{
	/* if the command takes 0 args */
	case 0:
		item->word.encode.des = 0;
		item->word.encode.src = 0;
		item->flag = FLAG_ABSOLUTE_CHAR;
		if (*line != '\0')
		{
			EC = COMMAND_EXPECT_ZERO_ARGS;
			goto EXIT;
		}
		add_code_image(item);
		/* adding item to code image */
		break;
	/* if the command takes 1 args */
	case 1:
		item->word.encode.des = get_arg_id(argv[0]);
		item->word.encode.src = 0;
		item->flag = FLAG_ABSOLUTE_CHAR;
		add_code_image(item);
		if ((EC = add_arg(argv[0]) != SUCCESSFUL))
		{
			goto EXIT;
		}
		if (!is_exist_array(item->word.encode.des, cmd->allowed_des))
		{
			EC = COMMAND_DES_ARG_INVALID;
			goto EXIT;
		}
		break;
	/* if the command takes 2 args */
	case 2:
		item->word.encode.des = get_arg_id(argv[1]);
		item->word.encode.src = get_arg_id(argv[0]);
		item->flag = FLAG_ABSOLUTE_CHAR;
		add_code_image(item);
		if ((EC = add_arg(argv[0]) != SUCCESSFUL) || (EC = add_arg(argv[1]) != SUCCESSFUL))
		{
			goto EXIT;
		}
		if (!is_exist_array(item->word.encode.src, cmd->allowed_src))
		{
			EC = COMMAND_SRC_ARG_INVALID;
			goto EXIT;
		}
		if (!is_exist_array(item->word.encode.des, cmd->allowed_des))
		{
			EC = COMMAND_DES_ARG_INVALID;
			goto EXIT;
		}
		break;
	}
	/* if reached this point no error  */
	EC = SUCCESSFUL;

EXIT: /* exit label */
	free(item);
	return EC;
}

/* this function takes a string and removes all padding whitespace from the end and start.
@param str the string you want trimed
@return - the trimed string with no padding whitespaces */
char *strtrim(char *str)
{
	int len;
	char *end;
	len = strlen(str);
	if (len == 0)
	{
		return str;
	}
	end = str + len - 1;
	while (end >= str && isspace(*end))
	{
		end--;
	}
	*(end + 1) = '\0';
	str = skip_whitespace(str);
	return str;
}

/* this function determines whether an int exist inside an array
@param val the value you want being checked inside @arr
@param arr the array you want to check in
@return true if @val exist inside @arr otherwise false */
bool is_exist_array(int val, int arr[])
{
	int i = 0;
	int len = arr[i++];
	for (; i <= len; i++)
	{
		if (arr[i] == val)
		{
			return true;
		}
	}
	return false;
}

/* this function take an argument and adds it to the code_image
@param arg this is the argument you want to encode and to code_image
@return the appropriate error code */
int add_arg(char *arg)
{
	code *item;
	label_tbd *lbl;
	int arg_id = get_arg_id(arg);
	if (arg_id == INVALID_ARG)
	{
		return COMMAND_INVALID_ARGS;
	}
	if (!(item = malloc(sizeof(code))))
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	if (!(lbl = malloc(sizeof(label_tbd))))
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	item = get_arg(arg, arg_id);
	/* adding TBD label for second stage */
	if (arg_id == DIRECT || arg_id == RELATIVE)
	{
		lbl->is_relative = arg_id == RELATIVE;
		lbl->address = IC;
		lbl->line_number = PC;
		if (arg_id == RELATIVE)
		{
			arg++;
		}
		lbl->name = arg;
		add_label_tbd(lbl);
	}
	add_code_image(item);

	free(item);
	return SUCCESSFUL;
}

/* this function get the code block with the given address function id
@arg - a string containing the arguemnt
@id - the address function id
@return - the appropriate code block*/
code *get_arg(char *arg, int id)
{
	code *block = malloc(sizeof(code));
	switch (id)
	{
	case IMMEDIATE:
		block->flag = FLAG_ABSOLUTE_CHAR;
		block->word.val = atoi(arg + 1); /* skiping the NUMBER_CHAR */
		break;
	case REGISTER:
		block->flag = FLAG_ABSOLUTE_CHAR;
		block->word.val = get_register(atoi(arg + 1));
		break;
	case RELATIVE:
	case DIRECT:
		block->flag = FLAG_TBD_CHAR;
		block->word.val = 0;
		break;
	default:
		call_error(UNKNOWN, PC);
		free(block);
		return NULL;
	}
	return block;
}

/* this function takes a register number returns register binary
@reg the number of register
@return - the binary number represnting the register */
int get_register(int reg)
{
	return 1 << (reg);
}

/* this function takes a given arguments and return the argument address function
@arg - the argument being checked.
@return - the appropriate address function. will return INVALID_ARG if @arg is invalid. */
int get_arg_id(char *arg)
{
	switch (*arg)
	{
	case NUMBER_CHAR:
		/* skiping number char ('#') */
		if (is_number(arg + 1))
		{
			return IMMEDIATE;
		}
		break;
	case REGISTER_CHAR:
		if (is_saved_register(arg))
		{
			return REGISTER;
		}
		break;
	case RELATIVE_CHAR:
		/* skipping relative char ('%') */
		if (is_valid_label(arg + 1))
		{
			return RELATIVE;
		}
		break;
	/* if not all other then label */
	default:
		if (is_valid_label(arg))
		{
			return DIRECT;
		}
	}
	return INVALID_ARG;
}

/* this function handles instruction on the first stage and inserts data and labels if needed
@line - the line that includes the instruction
@lbl - the label in the line if it had one.
@return - appropriate error code. */
int instruction_handler(char *line, label *lbl)
{
	instruction *instruct;
	char *temp;
	int EC = UNKNOWN;
	if ((instruct = malloc(sizeof(char))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	/* skiping @INSTRUCTION_CHAR (.) */
	line++;

	/* getting instruction */
	temp = strdup(line);
	instruct->name = strtok(temp, SPACE_TOKEN);
	/* checks whether instruction exists */
	if (!is_saved_instruction(instruct->name))
	{
		EC = INSTRUCTION_NOT_EXIST;
		goto EXIT;
	}
	/* data or string */
	if (match_string(instruct->name, STRING))
	{
		lbl->is_data = true;
		lbl->address = DC;
		EC = insert_string(line);
		goto EXIT;
	}
	if (match_string(instruct->name, DATA))
	{
		lbl->is_data = true;
		lbl->address = DC;
		EC = insert_data(line);
		goto EXIT;
	}
	/* entry or external */
	if (match_string(instruct->name, ENTRY))
	{
		line = next_word(line);
		temp = strdup(line);
		lbl->name = strtok(temp, SPACE_TOKEN);

		if (lbl->name == NULL) /* if no label is given */
		{
			EC = INSTRUCTION_EXTERN_MISSING_LABEL;
			goto EXIT;
		}
		else if (*next_word(line) != '\0' || is_valid_label(lbl->name) != SUCCESSFUL)
		{
			EC = INSTRUCTION_EXTERN_INVALID_LABEL;
			goto EXIT;
		}

		lbl->is_entry = true;

		/* gets handled at the second stage */
		add_first(&entries, lbl, sizeof(label));

		EC = SUCCESSFUL;
		goto EXIT;
	}
	if (match_string(instruct->name, EXTERNAL))
	{
		line = next_word(line);
		temp = strdup(line);
		lbl->name = strtok(temp, SPACE_TOKEN);

		if (lbl->name == NULL) /* if no label is given */
		{
			EC = INSTRUCTION_EXTERN_MISSING_LABEL;
			goto EXIT;
		}
		else if (*next_word(line) != '\0' || is_valid_label(lbl->name) != SUCCESSFUL)
		{
			EC = INSTRUCTION_EXTERN_INVALID_LABEL;
			goto EXIT;
		}
		lbl->is_extern = true;
		lbl->address = PLACEHOLDER_ADDRESS;
		EC = SUCCESSFUL;
		goto EXIT;
	}
	EXIT:
	free(instruct);
	return EC;
}

/* this function inserts data from the .data instruction
@line - the line where the data instruction is in
@return - error code if there is an error otherwise @SUCCESSFUL IS RETURNED */
int insert_data(char *line)
{
	int i, EC = SUCCESSFUL;
	/* for adding to data image */
	word *item;
	/* for strtok */
	char data_str[2] = {DATA_CHAR, '\0'};
	char *str, *temp;
	int argc = 0;
	/* storing args */
	int argv[MAX_LINE] = {0};

	line = next_word(line);

	if (*line == '\0')
	{
		return INSTRUCTION_MISSING_ARGS;
	}
	if (*line == DATA_CHAR)
	{
		return INSTRUCTION_INVALID_DATA;
	}

	temp = strdup(line);
	str = strtok(temp, data_str);
	while (str != NULL)
	{
		str = skip_whitespace(str);
		if (!is_number(str))
		{
			EC = INSTRUCTION_INVALID_DATA;
			goto EXIT;
		}
		argv[argc++] = atoi(str);
		if (str != NULL && *skip_whitespace(str) == '\0')
		{
			EC = INSTRUCTION_INVALID_DATA_SYNTAX;
			goto EXIT;
		}
		str = strtok(NULL, data_str);
	}
	/* another more secure check but with less detail */
	if (count_args(line) != argc)
	{
		EC = INSTRUCTION_INVALID_DATA_SYNTAX;
		goto EXIT;
	}
	if ((item = (word *)malloc(sizeof(word))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	for (i = 0; i < argc; i++)
	{
		item->val = argv[i];
		add_data_image(item);
	}
EXIT:
	free(temp);
	return EC;
}

/* this function gets a line and if there is a valid string it puts it inside the data image
@line - the line where the string is contianed
@return - Error code is there is error other wise @SUCCESSFUL is returned. */
int insert_string(char *line)
{
	char *str = NULL;
	char string_str[2] = {STRING_CHAR, '\0'};
	/* first and last pointers to the string */
	char *first = strchr(line, STRING_CHAR);
	char *last = strrchr(line, STRING_CHAR);
	/* insert item */
	word *item;
	/* checks validity of string */
	if (first == NULL)
	{
		return INSTRUCTION_MISSING_ARGS;
	}
	if (last == NULL || first == last)
	{
		return INSTRUCTION_INVALID_STRING;
	}
	str = strtok(strdup(first), string_str);
	/* if string is empty */
	if ((item = (word *)malloc(sizeof(word))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	if (str == NULL)
	{
		item->val = '\0';
		add_data_image(item);
		free(item);
		return SUCCESSFUL;
	}
	while (true)
	{
		item->val = (int)(*str);
		add_data_image(item);
		if (*str == '\0')
		{
			break;
		}
		str++;
	}
	free(item);
	return SUCCESSFUL;
}

/* determines where a given string is a number of not */
bool is_number(char *number)
{
	if (number == NULL)
	{
		return false;
	}
	number = skip_whitespace(number);
	if (*number == POSITIVE_CHAR || *number == NEGETIVE_CHAR)
	{
		number++;
	}
	while (!isspace(*number) && *number != '\0')
	{
		if (!isdigit(*number))
		{
			return false;
		}
		number++;
	}
	return *skip_whitespace(number) == '\0';
}

/* this function counts the number of time a given char is inside a given string.
@str - the string you want to search in
@find - the char you want counted
@return - the number of times @find is inside @str */
int count_char(char *str, const char find)
{
	char *temp = str;
	int counter = 0;
	temp = strchr(temp, find);
	while (temp != NULL)
	{
		temp = strchr(temp + 1, find);
		counter++;
	}
	return counter;
}

/* This function get the word before the @LABEL_CHAR which is not inside a string
NOTE @get_label doesn't check if the label is valid or not.
@line - the line which the label is extracted from
@return - the string up to the @LABEL_CHAR not including the @LABEL_CHAR */
char *get_label(char *line)
{
	char label_str[2] = {LABEL_CHAR, '\0'};
	/* +1 for null byte and another +1 incase label is larger then @MAX_LABEL so it can be checked. */
	char *label = NULL;
	/* getting address of both @LABEL_CHAR and @STRING_CHAR */
	const char *label_chr = strchr(line, LABEL_CHAR);
	const char *string_chr = strchr(line, STRING_CHAR);

	if (label_chr == line)
	{
		call_error(LABEL_EMPTY, PC);
		return NULL;
	}

	/* if @label_char doesn't exist in line
	 or if the @string_char exist before the @label_char */
	if (label_chr == NULL || (string_chr != NULL && label_chr > string_chr))
	{
		return NULL;
	}
	label = strtok(strdup(line), label_str);
	if (!isalpha(*label))
	{
		call_error(LABEL_FIRST_CHAR_NOT_ALPHA, PC);
		return NULL;
	}
	return label;
}

/* this function checks if a given string is a saved command
@cmd - the command to be checked.
@return - true if @cmd is a valid command otherwise false is returned. */
bool is_saved_command(char *cmd)
{
	return index_of(commands, cmd, (MATCH_CAST)match_command) != -1;
}

/* this function check if a given string is a saved register
@reg - the string to be checked.
@return - true if @reg is a valid register otherwise false is returned. */
bool is_saved_register(char *reg)
{
	int num;
	/* checking if first char is 'r' and reset is digits.
	(reg + 1) skiping the letter 'r' */
	if (reg[0] == REGISTER_CHAR && is_all_rule(reg + 1, &isdigit))
	{
		num = atoi(reg + 1);
		return num >= 0 && num < NUMBER_OF_REGISTERS;
	}
	/* not register */
	return false;
}

/* this function check if a given string is a saved instruction
@instruct - the string to be checked.
@return - true if @instruct is a valid instruction otherwise false is returned. */
bool is_saved_instruction(char *instruct)
{
	return index_of(instructions, instruct, (MATCH_CAST)match_instruction) != -1;
}

/*this functin determines if a given word is a is a label
@word - the word that is being checked
@return - @SUCCESFUL if @word is a labal otherwise ERROR CODE is returned*/
int is_valid_label(char *label)
{
	if (strlen(label) > MAX_LABEL) /* checks length of label */
	{
		return LABEL_EXCEEDS_LENGTH;
	}
	else if (!isalpha(*label)) /* checks is first char is alpha */
	{
		return LABEL_FIRST_CHAR_NOT_ALPHA;
	}
	else if (is_saved_command(label)) /* checks if label is command */
	{
		return LABEL_IS_COMMAND;
	}
	else if (is_saved_register(label)) /* checks if label is register */
	{
		return LABEL_IS_REGISTER;
	}
	else if (is_saved_instruction(label))
	{
		return LABEL_IS_INSTRUCTION;
	}
	else if (!is_all_rule(label + 1, isalnum)) /* checks if body is alpha or num */
	{
		return LABEL_INVALID_FORMAT;
	}
	return SUCCESSFUL;
}

bool is_label_exist(char *lbl)
{
	return index_of(symbols, lbl, (MATCH_CAST)match_label) != -1;
}

/*this function gets a rule and check if all chars of a string follow the rule.
@word - the string to be check if follows the @rule.
@rule - the rule function.
@return - true if all chars follow the @rule otherwise flase is returned.*/
bool is_all_rule(char *word, int (*rule)(int))
{
	int i = 0;
	char ch = word[i];
	while (ch != '\0')
	{
		if (!rule(ch))
		{
			return false;
		}
		ch = word[i];
		i++;
	}
	return true;
}

/*this function change the pointer of a string to point at the next word skipping all white characters
@data - the pointer to the string*/
char *skip_whitespace(char *str)
{
	int i = 0;
	if (str == NULL)
	{
		return NULL;
	}
	while (isspace(str[i]))
	{
		i++;
	}
	return str + i;
}

/* counts the number of elements seperated by DATA_CHAR
@line - the line which includes the arguments
@return - the number of fields seperated by DATA_CHAR */
int count_args(char *line)
{
	int counter = count_char(line, DATA_CHAR);
	if (counter == 0)
	{
		/* if there is a "next_word" there is 1 arg */
		return *skip_whitespace(line) != '\0';
	}
	return counter + 1;
}

/* this function goes to start of the next word
@line - the line which you want to point to the next word
@return - pointer to the start of next word */
char *next_word(char *line)
{
	line = skip_word(line);
	line = skip_whitespace(line);
	return line;
}

/* this function skip to the end of next word */
char *skip_word(char *line)
{
	int i = 0;
	if (line == NULL)
		return NULL;
	while (!isspace(*(line + i)) && *(line + i) != '\0')
	{
		i++;
	}
	return line + i;
}

/* this function add an extention to a file name
@param file_name  */
char *add_extention(const char *file_name, const char *extention)
{
	char *fn;
	if((fn = malloc(strlen(file_name) + strlen(extention) + 1)) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY,NO_LINE);
	}
	strcpy(fn, file_name);
	return strcat(fn, extention);
}
