#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"


bool update_and_print_error(bool,bool,char*,int);
int DC(short int);
int IC(short int);
void convert_decimal_to_binarry(int, char*);
void convert_binarry_hexadecimal(char*,char*);
void insert_encoded_commend(int,char*,bool,int);
void update_table(int,char*,char*,int);
bool isnt_exist_on_table(char*);
void pending_address(int,int,char*);
int symbol_to_value(char*);
int find_row_by_ic(int);
int skip_to_end_commend(bool, char*);
bool continue_array(int);
void create_file_ob(char*);
void creat_file_ent(char*);
void creat_file_ext(char*);
bool is_external(char*);
void free_all_list(); 

/* this method update and print error */
bool update_and_print_error(bool intialize, bool print,char *error_type, int id_line)
{
   static bool dont_continue=false;    
   if(!print) /* define static bool variable if print is on dont_continue is true */
   {
      if(intialize)
         dont_continue=false;
      return dont_continue;   /* when call to method with !print get answer if error */
   }
   dont_continue=true;
   printf("line %d: error %s\n",id_line,error_type);
   return dont_continue;
}

/* this two mehtods that save ic and dc and rise in value which get and return the current calue */
int IC(short int i)
 {
   static int ic=100;
   ic+=i;
   return ic;
}
int DC(short int i)
{
   static int dc=0;
   dc+=i;
   return dc;
}

/*this method get a decinal number and convert to binary as string type */
void convert_decimal_to_binarry(int tmp, char data[])		
{
	int i; /* variable i use for date pointer */
   bool negative=false; /* this bool variable is mark if is nagative number for define as two comlement */
   data[size_row-1]='\0'; /* define end of string */
	if(tmp<0) /* check if is negative number and upfate negative var */
   {  
      negative=true;
      tmp=-tmp;  /* change for positive to calculate in binary as positive number */
   }
   for(i=23; tmp>0; i--)   /* put number '1' where is even number step by step */ 
   {    
	if(tmp%2)
		data[i]='1';
	else
		data[i]='0';
      tmp=tmp/2; 
   }
	while(i>=0)  /* intialzed the rest of bit to zero */  
   {    
	   data[i]='0';
		i--;
   }
   if(negative) /* if is nagative use in two complements method */
   {
      for(i=0; i<24; i++) /* turn the number */
      {
         if(data[i]=='0') 
         { 
            data[i]='1';
         }
         else 
         { 
	      	data[i]='0';
         }
      }
      i=23;
      while(data[i]=='1') /* intializ until the first bit with '1' on the last */
      {
       	data[i]='0';
         i--;
      }
      data[i]='1';
   }
}

/* this method get a string number in binary print it in heaxadecimal */
void convert_binarry_hexadecimal(char data[],char insert_to[])
{
    short int i,j,revers_k,k;  /* using with i to pointer num_of_hexadecimal j to point 4 bit as hexa num */
    char num_of_hexadecimal[16]="0123456789abcdef", hexa_bit[4];  /* define num of hexa and array to save 4 bit seperate  */
    for(i=0; i<23; i+=4)
    {   /* scan the array per 4 bit */
        strncpy(hexa_bit,data+i,4); 
        k=0,j=0,revers_k=8; /* update per for call becase its er 4 bit */
        while(k!=4)
        {   /* take a bit of hexa and convert for number in hexa to j */
            if(hexa_bit[k++]=='1')
                j+=revers_k;
            revers_k/=2;
        }
         insert_to[i/4]=num_of_hexadecimal[j]; /* update the custom bum of string matrix */
    }
    insert_to[6]='\0';
}

/* this method insert to database id number of encode commend and num of line in input file and boolean if is data type */
void insert_encoded_commend(int id, char data[],bool DC, int id_line)
{
   struct encoded_commend *p;
   struct encoded_commend *link= (struct encoded_commend*) malloc(sizeof(struct encoded_commend)); /* assign 1 place as encoded_commend at list */
   strcpy(link->data,data);   /* update all date on the list */
   link->key = id;
   link->DC=DC;
   link->id_line=id_line;
   p=head;  /* point by head of list */
   if(head) /* if list isn't empty  head!=null */
   {
      while(p->next && id>=p->next->key)  /* scan the list until place of id in num */
         p=p->next;
      link->next=p->next;  /* added current link to list in his place */
      p->next=link;
   }
   else
   {  /* if empty list update the link in head place */
      link->next=head;
      head=link;
   }
}

/* this method update symbol on symbol table and get two patameter ic or dc and symbol type */
void update_table(int value, char symbol[], char type[],int id_line) 
{
   if(isnt_exist_on_table(symbol)) /* check if this isn't exist on the table */
   {
      struct symbol_table *p=head_s; /* creat a pointer as symbol table for go to end list to insert next symbol */
      struct symbol_table *add_s = (struct symbol_table*) malloc(sizeof(struct symbol_table)); /* assign more place in size symbol_table */
      strcpy(add_s->symbol,symbol); /* update value symbol and type */
      strcpy(add_s->type,type);
      add_s->value=value;
      if(p) /* if list isn't empty go to end and insert symbol */
      {
         while (p->next)
            p=p->next;
         p->next=add_s;
         add_s->next=NULL;
      }
      else
      {  /* if list is empty insert symbol to head list */
         add_s->next = head_s;
         head_s = add_s;
      }
   }  /* copy all of the member */
   else /* symbol is exits update error */
      update_and_print_error(false,true,"symbol is already exist on symbol table",id_line);       
}

/* the method get word check if is exist on the table */
bool isnt_exist_on_table(char symbol[])
{
   struct symbol_table *p = head_s; /* pointer to first on the list */
   while(p!=NULL)
   {
      if(!strcmp(p->symbol,symbol))
      {
         return false;
      }
      p=p->next;   /* continue to next member */
   }
   return true;
}

/* this method insert label and operand which pending to get a address in step 2 */
void pending_address(int IC, int oprand_type, char data[])
{
   struct pending_for_address *link= (struct pending_for_address*) malloc(sizeof(struct pending_for_address)); /* add assign for 1 place */
   strcpy(link->label,data);  /* update label and opernad type and current ic at list */
   link->operand_type=oprand_type;
   link->IC=IC;
   link->next = head_address;
   head_address = link;
}

/* the method get a symbol and return ic or dc of symbol if symbol don't exist return 0 */
int symbol_to_value(char symbol[])
{
   struct symbol_table *p = head_s; /* creat pointer to first member in linked list */
   while(p!=NULL)
   {
      if(!strcmp(p->symbol,symbol))
      {
         return p->value;
         break;
      }
      p=p->next;
   }  /* check member step by step */
   return 0;   /* symbol dosn't exist */
}

/* thos method find row by key */
int find_row_by_ic(int IC)
{
   struct encoded_commend *ptr = head;	 /* creat pointer by linked list of encoded_commend*/
   while(ptr != NULL) /* scan from began until end list or find ic */
   {
      if(!ptr->DC && ptr->key==IC)
      {
         return ptr->id_line;
         break; 
      }           
      ptr = ptr->next;
   }
   return 0;
}

/* this method skip empty character until comma or end row by comma bool and return 0 when find other chracter bedor*/
int skip_to_end_commend(bool comma, char current_row[])
{
	int i_row=0; /* use for point to current row */
	while(i_row<strlen(current_row) && current_row[i_row]==' ') { i_row++; } /* skip empty character */ 
	if(comma  && i_row<strlen(current_row)  && current_row[i_row]==',') /* check the first char is , */
		return i_row+1;	   /* return the size between last char to , */
	if(!comma && i_row==strlen(current_row))
		return 1; 	/* check if just empty char until end row */ 
	else
		return 0;
}

/* this method warm do .data array if is finish or in action */
bool continue_array(int i)
{
	static bool to_continue=false;	/* define static variable if continue is legal 1 turn on -1 turn off */ 
	if(i==1)
		to_continue=true;
	if(i==-1)
		to_continue=false;
	return to_continue;
}

/* this method update label and ic that wating to make a file */
void pending_external_file(int id, char label[])
{
   struct external *link= (struct external*) malloc(sizeof(struct external)); /* assing more place in external size */
   strcpy(link->label,label);
   link->key = id;
   link->next = head_external;
   head_external = link;
}   /* update the data and go to the next member */

/* this method creat object file */
void create_file_ob(char file_name[])
{
   struct encoded_commend *p;
   char hexadecimal[7],*new_file; /* define data string and array string to change name of file */
   FILE *f; 
   new_file=(char*) malloc((strlen(file_name)+3)*sizeof(char)); /* assign array string for new name as object file */
   strcpy(new_file,file_name);
   strcat(new_file,".ob");    /* update the new name with object */
   f = fopen(new_file,"w");   /* creat and open new file */
   fprintf(f,"%7d %d\n",(IC(0)-100),DC(0));    /* insert num of IC and DC to file */
   p=head;  /* point by list of ic_dc */
   while (p)
   {  
      if(!p->DC)  /* search this ic from i at for loop */
      {
         convert_binarry_hexadecimal(p->data,hexadecimal);   /* convert for hexadecimal */
         fprintf(f,"%07d %s\n",p->key,hexadecimal);   /* insert this row to  file */
      }
      p=p->next;
   }  
   p=head;  /* point by list of ic_dc */
   while (p)
   {
      if(p->DC)     /* search this ic from i at for loop */
      {
         convert_binarry_hexadecimal(p->data,hexadecimal); /* convert for hexadecimal */   
         fprintf(f,"%07d %s\n",p->key+IC(0),hexadecimal);  /* insert this row to  file */
      }
      p=p->next;
   }  
   fclose(f);
}

/* this method creat entry file */
void creat_file_ent(char file_name[])
{
   FILE *f; 
   struct symbol_table *p;  /* crwat poiner of symbol table */
   char *new_file=(char*) malloc((strlen(file_name)+4)*sizeof(char)); /* assign string array for new name with .ent */
   strcpy(new_file,file_name);
   strcat(new_file,".ent");    /* update the new of file with .ent */
   f = fopen(new_file,"w");   /* reat and open new file */
   p = head_s;    /* point for head of list */
   while (p)
   {
      if(p->type[4]==',')  /* check if entry is exist on , indicat that have a 2 type */
         fprintf(f,"%s %07d\n",p->symbol,p->value);   /* insert this row to file */
      p=p->next;
   }
  fclose(f);
}

/* this method creat external file */
void creat_file_ext(char file_name[])
{
   FILE *f;
   struct external *p;  /* define a pointer of external list */
   char *new_file=(char*) malloc((strlen(file_name)+4)*sizeof(char));   /* assign string array for new name with .ext */
   strcpy(new_file,file_name);
   strcat(new_file,".ext");   /* update the new name */
   f = fopen(new_file,"w");   /* creat and open file */
   p=head_external;  /* p is point to head list of external */
   while (p)
   {
      fprintf(f,"%s %07d\n",p->label,p->key);   /* insert external label to file  */
      p=p->next;
   }
   fclose(f);
}

/* this mehtod check label if is external */
bool is_external(char label[])
{
   struct external *p;  /* define pointer as external list type */
   while (p)
   {  
      if(!strcmp(p->label,label))   /* check if exist on external list */
         return true;
      p=p->next;
   }
   return false;
}

void free_all_list()
{   /* creact variable as all of the type of linked list var current to point by head and p to scan and free */
    struct encoded_commend *current1=head;
    struct encoded_commend *p1;
    struct symbol_table *current2=head_s;
    struct symbol_table *p2;
    struct pending_for_address *current3=head_address;
    struct pending_for_address *p3;
    struct external *current4=head_external;
    struct external *p4;
    /* on linked 1 it's the same free of all the list */
    while (current1)
    {
        p1=current1->next;  /* p point to the next member */
        free(current1);     /* free the current member */
        current1 = p1;      /* update current to point on the next member */
    }
    head=NULL;  /* when all of member has free update head to null */ 

    while (current2)
    {
        p2=current2->next;
        free(current2); 
        current2 = p2; 
    }
    head_s=NULL;

    while (current3)
    {
        p3=current3->next;
        free(current3); 
        current3 = p3; 
    }
    head_address=NULL;

    while (current4)
    {
        p4=current4->next;
        free(current4); 
        current4 = p4; 
    }
    head_external=NULL;
}
