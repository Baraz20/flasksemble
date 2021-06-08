#include "include/stdheaders.h"
#include "include/data_manager.h"
#include "include/list.h"
#include "include/error_handler.h"
#include <stdarg.h>

/* //TODO: CHANGE THIS NODES (not a must according to instructions but recommanded) */
code *code_image[MAX_IMAGE_SIZE];
word *data_image[MAX_IMAGE_SIZE];

/* tbd - "to be determine" is a list of symbols used as arguments 
that need to be replaced on the second stage */

/* could combine alot of the lists togther for ex @entries and @externals
but for readability decided to not  */

node *entries;		/* list of entry symbols */
node *symbols_TBD;	/* list of symbols that are to be determine */
node *symbols;		/* list of symbols (symbol table) */
node *commands;		/* list of commands */
node *instructions; /* list of instruction .string .data */
int IC = INIT_IC;	/* instuction counter */
int DC = INIT_DC;	/* data counter */
int PC = 0;			/* program counter */
bool is_err = false;

/*
this function is an initalizer for data_manager
*/
void data_manager_init()
{
	/* //TODO: Make this more efficent or hash_tables, and mkcmd to work with all structs? */
	/* creating all commands*/
	int cmd_size = sizeof(command);
	int instruct_size = sizeof(instruction);
	add_first(&commands, mkcmd("mov", 0, 0, mkarr(3, 0, 1, 3), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("cmp", 1, 0, mkarr(3, 0, 1, 3), mkarr(3, 0, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("add", 2, 10, mkarr(3, 0, 1, 3), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("sub", 2, 11, mkarr(3, 0, 1, 3), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("lea", 4, 0, mkarr(1, 1), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("clr", 5, 10, mkarr(0), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("not", 5, 11, mkarr(0), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("inc", 5, 12, mkarr(0), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("dec", 5, 13, mkarr(0), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("jmp", 9, 10, mkarr(0), mkarr(2, 1, 2)), cmd_size);
	add_first(&commands, mkcmd("bne", 9, 11, mkarr(0), mkarr(2, 1, 2)), cmd_size);
	add_first(&commands, mkcmd("jsr", 9, 12, mkarr(0), mkarr(2, 1, 2)), cmd_size);
	add_first(&commands, mkcmd("red", 12, 0, mkarr(0), mkarr(2, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("prn", 13, 0, mkarr(0), mkarr(3, 0, 1, 3)), cmd_size);
	add_first(&commands, mkcmd("rts", 14, 0, mkarr(0), mkarr(0)), cmd_size);
	add_first(&commands, mkcmd("stop", 15, 0, mkarr(0), mkarr(0)), cmd_size);

	/* creating all instruction */
	add_first(&instructions, mkinstruct(STRING), instruct_size);
	add_first(&instructions, mkinstruct(ENTRY), instruct_size);
	add_first(&instructions, mkinstruct(DATA), instruct_size);
	add_first(&instructions, mkinstruct(EXTERNAL), instruct_size);
}

/* this function duplicates to a new memory location a given string.
@param src the string you want to duplicate.
@return - the duplicated string at the new memory location. */
char *strdup(char *src)
{
	char *str;
	char *tmp;
	int len = strlen(src);

	if ((str = malloc(len + 1)) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	tmp = str;
	while (*src != '\0')
	{
		*tmp++ = *src++;
	}
	*tmp = '\0';
	return str;
}

/* this funtion creates an integer array with a given lenght
@len - the length of the array
@... - the elements of the array
@return - the array with length @len + 1 and the elements of @... (first element of array has @len)
if @len is zero or below NULL is returned.*/
int *mkarr(int len, ...)
{
	/* //TODO make this function more relable */
	int *arr;
	int index = 0;
	va_list ap;
	if (len <= 0)
	{
		return NULL;
	}
	if ((arr = (int *)malloc(sizeof(int) * (len + 1))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	va_start(ap, NULL);
	/* puts length of array at first element */
	arr[index++] = len;
	while (index != (len + 1))
	{
		arr[index++] = va_arg(ap, int);
	}
	va_end(ap);
	return arr;
}

/*this function creates a function with a name ,opcode and func
@name - this is the name of the command
@opcode - this is the opcode of the command
@func - this is the func of the command  
@return - a struct command with the given properties
*/
command *mkcmd(char *name, int opcode, int func, int *allowed_src, int *allowed_des)
{
	command *cmd;
	if ((cmd = malloc(sizeof(command))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	cmd->name = name;
	cmd->opcode = opcode;
	cmd->func = func;
	cmd->allowed_src = allowed_src;
	cmd->allowed_des = allowed_des;
	return cmd;
}

/* this function creates an instruction with a given name
@param name - the name you want to give instruction
@return a instruction struct with the value of @name  */
instruction *mkinstruct(char *name)
{
	instruction *item;
	if ((item = malloc(sizeof(instruction))) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	item->name = name;
	return item;
}

void add_code_image(code *item)
{
	if ((DC + IC) >= MAX_IMAGE_SIZE)
	{
		call_error(RUN_OUT_MEMORY, PC);
		return;
	}
	add_code_image_at(item, IC);
	IC++;
}

/* this function adds a code struct to the code image at a specific index
@item - the code block you want to add
@index - the index at which you want to store */
void add_code_image_at(code *item, int index)
{
	size_t code_size;
	code_size = sizeof(code);
	if (index >= MAX_IMAGE_SIZE)
	{
		printf("Warning index out of range. returning NULL\n");
		return;
	}
	if ((code_image[index] = malloc(code_size)) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	memcpy(code_image[index], item, code_size);
}

/* this function adds data to the next available spot on the data image
@item - the data you want to add */
void add_data_image(word *item)
{
	size_t word_size;
	if ((DC + IC) >= MAX_IMAGE_SIZE)
	{
		call_error(RUN_OUT_MEMORY, PC);
		return;
	}
	word_size = sizeof(word);

	if ((data_image[DC] = malloc(word_size)) == NULL)
	{
		call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
	}
	memcpy(data_image[DC], item, word_size);
	DC++;
}

/* this function adds a label to the "to-be-determine symbol table" 
@param lbl the label you wanted added to the table*/
void add_label_tbd(label_tbd *lbl)
{
	add_first(&symbols_TBD, lbl, sizeof(label_tbd));
}

/* this is a simple function to add a label to the symbol table 
@param lbl the label you want added to the table*/
void add_label(label *lbl)
{
	add_first(&symbols, lbl, sizeof(label));
}

/* this function gets the cotents of the data image at @index  */
word *get_data_at(int index)
{
	return data_image[index];
}

/* this function gets the cotents of the code image at @index */
code *get_code_at(int index)
{
	return code_image[index];
}

/* this function matchs command name with a given target
@target - the target command
@cmd - the data which is the command pointer
@return - true if @target match with @cmd */
bool match_command(char *target, command *cmd)
{
	return match_string(target, cmd->name);
}

/* this function matchs instruction name with a given target
@target - the target instruction
@data - the data which is the command pointer
@return - true if @target match with @instruct */
bool match_instruction(char *target, instruction *instruct)
{
	return match_string(target, instruct->name);
}

/* this function matchs labels name with a given target
@target - the target label
@lbl - the label which is the label pointer
@return - true if @target match with @lbl */
bool match_label(char *target, label *lbl)
{	
	return match_string(target, lbl->name);
}

/* this function free the EVERY cell of the images */
void free_images()
{
	int i = 0;
	/* free all cells of images */
	/* this is a safer freeing process then freeing only up to IC or DC */
	for(i = 0; i < MAX_IMAGE_SIZE; i++)
	{
		free(code_image[i]);
		free(data_image[i]);
	}	
}