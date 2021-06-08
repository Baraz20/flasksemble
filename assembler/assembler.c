/* this file continas the heart of the assembler aka the main function */

#include "include/stdheaders.h"
#include "include/stage_one.h"
#include "include/stage_two.h"
#include "include/error_handler.h"
#include "include/data_manager.h"
#include "include/list.h"

int main(int argc, char const *argv[])
{
	int i;
	if (argc <= 1)
	{
		call_error(FILE_NOT_GIVEN, NO_LINE);
	}
	data_manager_init();
	for(i = 1; i<argc; i++)
	{
		input_initiator(argv[i]);
		if(!is_err)
			output_initiator(argv[i]);
		free_images();
		free_list(symbols);
		symbols = NULL;
		free_list(symbols_TBD);
		symbols_TBD = NULL;
		free_list(entries);
		entries = NULL;
	}
	/* free all lists */
	/* //TODO improve this */
	/* it's a bit funcky didn't have much time */
	free_list(instructions);
	free_list(commands);
	return 0;
}
