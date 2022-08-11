#include <stdio.h>
#include "step1.c"

int main(int argc, char *argv[])
{
    bool external,entry;    /* this is mark if creat external or entry file */
    int file_input_num;     /* point by num of file in main argv */
    for(file_input_num=1; file_input_num<argc; file_input_num++)    /* excute compilation by all file which input */
    {
        if(insert_rows_and_compile_step1(argv[file_input_num]))    /* send file to compile step 1 and return true if file exist */
        {
            if(!update_and_print_error(false,false,"",0))   /* check if step 1 executed withput error */
            {
                entry=search_label_of_entry(argv[file_input_num]);  /* update all of label */
                external=update_label_without_address();    /* update label without address */
            }
            if(!update_and_print_error(false,false,"",0)) /* if no error in step 2 */
            {
                if(entry)	/* if entry is exist creat file .ent */
                    creat_file_ent(argv[file_input_num]); 
                if(external)	 /* if external is exist creat file .ext */
                    creat_file_ext(argv[file_input_num]);
                create_file_ob(argv[file_input_num]);   /* creat object file */
            }
            /* intialize error method dc,ic and free all of the list */
            update_and_print_error(true,false,"",0);
            DC(-DC(0));
            IC(-IC(0)+100);
            free_all_list();
        }
        else    /* print warming because this file so not exist */
            printf("error %s.as file isn't exist\n",argv[file_input_num]);
    }    
    return 0;   
}