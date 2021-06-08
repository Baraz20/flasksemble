#ifndef _INPUT_
#define _INPUT_

#include "data_manager.h"

void input_initiator(const char *file_name);
bool is_all_rule(char *word, int (*rule)(int));
bool is_saved_register(char *reg);
bool is_saved_command(char *cmd);
bool is_saved_instruction(char *instruct);
bool is_label_exist(char *lbl);
bool is_number(char *number);
bool is_exist_array(int val, int *arr);
int is_valid_label(char *);
int line_analyzer(char *data);
int count_char(char *str, const char find);
int count_args(char *line);
int insert_string(char *line);
int insert_data(char *line);
int instruction_handler(char *line, label *lbl);
int command_handler(char *line);
int get_register(int reg);
int get_arg_id(char *arg);
int add_arg(char *arg);
char *get_label(char *line);
char *next_word(char *line);
char *skip_word(char *line);
char *skip_whitespace(char *line);
char *strtrim(char *str);
char *add_extention(const char *file_name, const char *extention);
code *get_arg(char *arg, int id);

#endif /* _INPUT_ */