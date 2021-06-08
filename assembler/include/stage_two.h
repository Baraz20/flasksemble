#ifndef _STAGE_TWO_
#define _STAGE_TWO_

#include <stdio.h>

void output_initiator(const char * file_name);
void add_missing_labels();
void make_object_file(const char * file_name,const int ICF,const int DCF);
void update_symbols(const int ICF);
void make_external_file(const char *file_name);
void make_entry_file(const char *file_name);

#endif
