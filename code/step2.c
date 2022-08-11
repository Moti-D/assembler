#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "additional_methods.c"


bool insert_entry_to_table(char*);
bool update_label_without_address();
bool search_label_of_entry(char*);

/* this method update address of unknow label in step 1 and retrun true if label external is exist */
bool update_label_without_address()
{
   int row_number;   /* define variable to get number of ic label */
   char to_binary[25];  /* define a number for encoding row */
   bool external=false; /* define variable to mark if external in on */
   struct pending_for_address *p=head_address;  /* define a pointer to head of pndding assress list */
   while(p!=NULL)
   {
      row_number=symbol_to_value(p->label); /* get number IC */
      if(!row_number) /* if not found symbol */
      {
         if(p->operand_type==1 && is_external(p->label)) /* is extern update extern */
         {
            insert_encoded_commend(p->IC,"000000000000000000000001",false,find_row_by_ic(p->IC-1));  /* update as esternal in ic_dc list */
            external=true; /* update that external is exist */
            pending_external_file(p->IC,p->label); /* update this label to list of external */
         }
         else /* update and print error not found symbol */
            update_and_print_error(false,true,"not found symbol",find_row_by_ic(p->IC-1));
      }
      else
      {  /* if label if found in symbol */
         if(p->operand_type==1)
         {  
            convert_decimal_to_binarry(row_number,to_binary);  /* convert the ic addrress to binary address */
            strcpy(to_binary,to_binary+3);   /* move as 3 bit for ARE */
            strcpy(to_binary+21,"010");   /* update R relocat */
            insert_encoded_commend(p->IC,to_binary,false,find_row_by_ic(p->IC-1));  /* insert to ic_dc list */
         }
         else
         {   /* is a & opernad type */
            convert_decimal_to_binarry(row_number-(p->IC-1),to_binary); /* convert number of space row in label ic to binary */ 
            strcpy(to_binary,to_binary+3);   /* move as 3 bit for ARE */
            strcpy(to_binary+21,"100");   /* update absolute */
            insert_encoded_commend(p->IC,to_binary,false,find_row_by_ic(p->IC-1));  /* insert to ic_dc list */
         }       
      }
      p=p->next;  /* update members step by step */ 
   }
   return external;  
}

/* this method find statement of entry and add to symbol table as type */
bool search_label_of_entry(char file_name[])
{
   FILE *f;
	char current_row[81],insert_to_entry[max_label];   /* define currrent_row to insert row in file and insert_to_entry to insert a label */
	bool end_file=false,entry=false; /* variable 1 mark if end file ans var 2 mark if entry is exist */
   int id_line=0; /* id_line mark line number in file */
   char *file_input_as=(char*) malloc((strlen(file_name)+3)*sizeof(char));   /* assign string array for new name with .as */
   strcpy(file_input_as,file_name);
   strcat(file_input_as,".as");   /* update name.as */
	f=fopen(file_input_as,"r");  
	while(!end_file)	
	{
		int i=0,i_row=0; /* i to point by current_row when is insert from file j point when notice ; i_row for use in curren_row validation */
		char c=' ';
		while(fread(&c,sizeof(char),1,f) && c!='\n' && c!=';' && i<80) /* while inpute the data until end row without notice sentance and update current row var */
		{
			current_row[i++]=c;
		}
		current_row[i]='\0';
      id_line++; 	/* update the first or next row on the file */
		/* check why finish the the privios while */
		if(c==';')  /* if notice sentance skip to the next row */
		{
			while(fread(&c,sizeof(char),1,f) && c!='\n');
		}
		if(c!='\n' && c!=';' && i<80)    /*	check if end file by two pararamter these */
		{
			end_file=true;
		}
		if(i) /* check if readed character in this row */
		{  
			while(i_row<strlen(current_row) && current_row[i_row]==' ') { i_row++; }   /* skip by empty chracter */
         if(i_row+6<strlen(current_row) && !strncmp(current_row+i_row,".entry ",7))  /* check .entry is show */
         {
            i_row+=7;   /* update point by row to move 7 becase . and commend and 1 space after */
            while(i_row<strlen(current_row) && current_row[i_row]==' ') { i_row++; }   /* skip by empty chracter */
            if(isalpha(current_row[i_row]))  /* check that first character is alpha */
            { 
               i=0;  /* update the point by insert_to_entry to begin */
               while(i_row<strlen(current_row) && (isalpha(current_row[i_row]) || isdigit(current_row[i_row])))
               {  /* insert character as word to array */
                  insert_to_entry[i++]=current_row[i_row++];
               }
               insert_to_entry[i]='\0';
               if(skip_to_end_commend(false,current_row+i_row))   /* no ilegal character agter .entry and "label" */
               {
                  if(insert_entry_to_table(insert_to_entry))  /* this label dosnt exist symbol table */
                  {
                     entry=true; /* update that entry is exist */
                  }
                  else   /* update and print error because symbol dont show on the table */
                     update_and_print_error(false,true,"this label dosn't exist on symbol table",id_line);
               }
               else  /*update and print error because ilegal character after .entry and 'label'"*/
                  update_and_print_error(false,true,"ilegal character after .entry and 'label'",id_line);
            }
         }
		}
	}
	fclose(f);
    return entry;	
}

/* this method get a label of entry type and copy this label to table in type */
bool insert_entry_to_table(char *insert_to_entry)
{
	struct symbol_table *search = head_s;  /* define point as symbol table to head of list */
	while (search)
	{
		if(!strcmp(search->symbol,insert_to_entry))  /* search label in table */
		{
			strcpy(search->type+4,", entry");   /* copy this label is also entry type */
			return true;   /* return and update that entry is exist */
			break;
		}
		search=search->next;
	}
	return false;
}