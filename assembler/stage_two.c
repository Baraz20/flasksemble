/* THIS IS THE STAGE 2 OF THE ASSEMBLER */

#include "include/stdheaders.h"
#include "include/stage_one.h"
#include "include/stage_two.h"
#include "include/error_handler.h"
#include "include/data_manager.h"
#include "include/list.h"

/* this acts as the main funtion for stage two of the assembler */
void output_initiator(const char *file_name)
{
    /* taking final values of IC and DC */
    const int ICF = IC;
    const int DCF = DC;
    update_symbols(ICF);
    add_missing_labels();

    if (!is_err)
    {
        make_object_file(file_name, ICF, DCF);
        make_entry_file(file_name);
        make_external_file(file_name);
    }
}

/* this function updates the addresses of the labels marked as data with ICF
@param ICF - the final instruction counter for the address calculation. */
void update_symbols(const int ICF)
{
    node *head = symbols;
    label *lbl;
    while (head != NULL)
    {
        lbl = (label *)(head->data);
        if (lbl->is_data)
        {
            lbl->address += ICF;
        }
        head = head->next;
    }
}

/* add missing labels in code image from symbols_TBD (to be determine) 
@ICF - the final instruction counter is needed for correct address calculation*/
void add_missing_labels()
{
    node *head = symbols_TBD;
    node *temp;
    label *lbl;
    label_tbd *tbd;
    code *block; /* the code block */
    int addr;    /* the address */
    while (head != NULL)
    {
        tbd = head->data;
        temp = pointer_of(symbols, tbd->name, (MATCH_CAST)match_label);
        if (temp == NULL)
        {
            call_error(LABEL_UNRECOGNIZED, tbd->line_number);
            head = head->next;
            continue;
        }
        lbl = temp->data;
        if ((block = malloc(sizeof(code))) == NULL)
        {
            call_error(UNABLE_TO_ALLOCATE_MEMORY, NO_LINE);
        }
        /* putting correct label address */
        if (tbd->is_relative == true)
        {
            /* (labael_addr) - (call_addr + 1)  */
            addr = lbl->address - (tbd->address);
        }
        else
        {
            addr = lbl->address;
        }
        /* setting adress and flag */
        block->word.val = addr;
        block->flag = lbl->is_extern ? FLAG_EXTERNAL_CHAR : FLAG_RELOCATABLE_CHAR;
        /* adding to code image */
        add_code_image_at(block, tbd->address);
        /* next to be determine label */
        head = head->next;
    }
}

/* this function create the entry file
@file - the original inputed filename without the extention */
void make_entry_file(const char *file_name)
{
    node *head = entries;
    bool entry_exist = false;
    char *fn; /* the file name with the extention */
    FILE *ent;
    label ent_label;
    label lbl;
    node *temp;

    /* creating file */
    fn = add_extention(file_name, ENTRY_EXTENTION);
    if ((ent = fopen(fn, "w")) == NULL)
    {
        call_error(FILE_UNABLE_TO_CREATE, NO_LINE);
    }

    fprintf(ent, "ENTRIES");
    while (head != NULL)
    {
        ent_label = *(label *)head->data;
        temp = pointer_of(symbols, ent_label.name, (MATCH_CAST)match_label);
        /* if there isn't any label with the entry label */
        if (temp == NULL)
        {
            head = head->next;
            continue;
        }
        lbl = *(label *)temp->data;
        entry_exist = true;
        fprintf(ent, "%s %04d\n", lbl.name, lbl.address);
        head = head->next;
    }
    /* if there isn't any entry file remove entry file */
    if (!entry_exist)
    {
        remove(fn);
    }
    free(fn);
}

/* this function creates the external file if there is any external labels
@file_name - the original file name inputed as argument */
void make_external_file(const char *file_name)
{
    node *head = symbols_TBD;
    char *fn; /* file name */
    FILE *ext;
    node *temp;
    label_tbd tbd_label;
    label ext_label;
    bool extern_exist = false;

    /* creating file */
    fn = add_extention(file_name, EXTERNAL_EXTENTION);
    if ((ext = fopen(fn, "w")) == NULL)
    {
        call_error(FILE_UNABLE_TO_CREATE, NO_LINE);
    }

    /* checking all label that were used as arguments aka (symbols_tbd)
    if there are externals add them to the file */
    fprintf(ext, "EXTERNALS");
    while (head != NULL)
    {
        tbd_label = *(label_tbd *)head->data;
        temp = pointer_of(symbols, tbd_label.name, (MATCH_CAST)match_label);
        if (temp == NULL)
        {
            head = head->next;
            continue;
        }
        ext_label = *(label *)temp->data;
        /* if label is ext add it to the list */
        if (ext_label.is_extern)
        {
            /* marking that there is an extern label */
            extern_exist = true;
            fprintf(ext, "%s %04d\n", ext_label.name, tbd_label.address);
        }
        head = head->next;
    }
    /* if there is no extern label */
    if (!extern_exist)
    {
        remove(fn);
    }
    free(fn);
}

/* this function creates the object file
@file_name - the object file  */
void make_object_file(const char *file_name, const int ICF, const int DCF)
{
    char *fn; /* file name */
    FILE *obj;
    int addr;
    code *block;
    word *data;
    fn = add_extention(file_name, OBJECT_EXTENTION);
    if ((obj = fopen(fn, "w")) == NULL)
    {
        call_error(FILE_UNABLE_TO_CREATE, NO_LINE);
    }
    free(fn);

    fprintf(obj, "OBJECT");
    /* header values*/
    fprintf(obj, "%d %d\n", ICF - INIT_IC, DCF - INIT_DC);
    /* CODE IMAGE FIRST */
    for (addr = INIT_IC; addr < ICF; addr++)
    {
        block = get_code_at(addr);
        fprintf(obj, "%04d %03X %c\n", addr, block->word.val, block->flag);
    }
    /* DATA IMAGE SECOND */
    for (addr = INIT_DC; addr < DCF; addr++)
    {
        data = get_data_at(addr);
        fprintf(obj, "%04d %03X %c\n", addr + ICF, data->val, FLAG_ABSOLUTE_CHAR);
    }
}
