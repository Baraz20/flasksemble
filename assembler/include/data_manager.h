#ifndef _DATA_MANAGER_
#define _DATA_MANAGER_

#include "list.h"

#define INIT_IC 100
#define INIT_DC 0
#define INIT_PC 0

#define ASSEMBLY_EXTENTION ".as"
#define OBJECT_EXTENTION ".ob"
#define ENTRY_EXTENTION ".ent"
#define EXTERNAL_EXTENTION ".ext"

#define INSTRUCTION_CHAR '.'
#define DATA_CHAR ','
#define COMMENT_CHAR ';'
#define STRING_CHAR '\"'
#define LABEL_CHAR ':'
#define NUMBER_CHAR '#'
#define REGISTER_CHAR 'r'
#define RELATIVE_CHAR '%'
#define POSITIVE_CHAR '+'
#define NEGETIVE_CHAR '-'
#define SPACE_TOKEN " \t\n" /* string conting space,tab,newline for tokenizing strings */

#define FLAG_ABSOLUTE_CHAR 'A'
#define FLAG_RELOCATABLE_CHAR 'R'
#define FLAG_EXTERNAL_CHAR 'E'
#define FLAG_TBD_CHAR '?'	/* TBD - to be determine */

#define DATA "data"
#define STRING "string"
#define ENTRY "entry"
#define EXTERNAL "extern"

#define PLACEHOLDER_ADDRESS 0
#define NUMBER_OF_COMMANDS 16
#define NUMBER_OF_INSTRUCTIONS 4
#define NUMBER_OF_REGISTERS 8 /* r0 r1 .. r7*/
#define MAX_LINE 80
#define MAX_LABEL 30
#define MAX_IMAGE_SIZE 4096
#define MAX_FILE_NAME 256 /* this is based on the book */
#define MAX_ARGS 2

#define SOURCE_FUNCTION_SIZE 2
#define DESTINATION_FUNCTION_SIZE 2
#define OPCODE_SIZE 4
#define FUNC_SIZE 4

#define WORD_SIZE (SOURCE_FUNCTION_SIZE + DESTINATION_FUNCTION_SIZE + OPCODE_SIZE + FUNC_SIZE)

#define MATCH_CAST bool (*)(void *,void *) /* this is for casting all the match_xxx functions */

enum address_function
{
	IMMEDIATE = 0,
	DIRECT,
	RELATIVE,
	REGISTER,
	INVALID_ARG
};

typedef union word
{
	unsigned int val : WORD_SIZE;

	struct encode
	{
		unsigned int des : DESTINATION_FUNCTION_SIZE;
		unsigned int src : SOURCE_FUNCTION_SIZE;
		unsigned int func : FUNC_SIZE;
		unsigned int opcode : OPCODE_SIZE;
	} encode;
	
} word;

typedef struct code
{
	word word;
	char flag;
} code;

typedef struct command
{
	char *name;
	int opcode;
	int func;
	int *allowed_src;
	int *allowed_des;
} command;

typedef struct label
{
	char *name;
	unsigned address;
	bool is_data;
	bool is_code;
	bool is_entry;
	bool is_extern;
} label;

typedef struct label_tbd
{
	char *name;
	unsigned address;
	int line_number;
	bool is_relative;
}label_tbd;


typedef struct instruction
{
	char *name;
} instruction;

void data_manager_init();
command *mkcmd(char *, int, int, int *, int *);
instruction *mkinstruct(char *name);
int *mkarr(int, ...);
void add_label(label *lbl);
void add_label_tbd(label_tbd *lbl);
void add_data_image(word *item);
void add_code_image(code *item);
void add_code_image_at(code *item, int index);
void free_images();

/* util for special matching (index_of) */
bool match_command(char *target, command *cmd);
bool match_instruction(char *target, instruction *cmd);
bool match_label(char *target,label *lbl);

char *strdup(char * src);

/* getters for images O(1) */
code * get_code_at(int index);
word * get_data_at(int index);

/* could combine alot of the lists togther for ex @instructions and @commands
but for readability decided to not  */

/* EXTERNS */
extern node *commands;
extern node *instructions;
extern node *symbols_TBD;
extern node *symbols;
extern node *entries;		
extern bool is_err; /* if error accoured this is set to true */
extern int IC;		/* instruction counter */
extern int DC;		/* data counter */
extern int PC;

#endif /* _DATA_MANAGER_ */