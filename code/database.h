#include <math.h>
#include <stdbool.h>
#define max_label 32
#define size_row 25


/* this stucrt is a database for binary encode with paramater ic or dc and */
struct encoded_commend {
   char data[size_row];
   int key;
   int id_line;
   bool DC;
   struct encoded_commend *next;
};
struct encoded_commend *head = NULL; 

/* this struct is a linked list of symbol table that contain symbol value of symbol and symbol type */
struct symbol_table {
   char symbol[max_label];
   int value;
   char type[12];
   struct symbol_table *next;
};
struct symbol_table *head_s = NULL;

/* this struct use as linked list for save operand that unknow thier address with parameter ic_dc, operand_tyoe */
struct pending_for_address
{
   char label[max_label-1];
   int IC;
   int operand_type;
   struct pending_for_address *next;   
};
struct pending_for_address *head_address = NULL;


/* this linked list save external symbol for insert to file ext */
struct external {
   char label[max_label];
   int key;
   struct external *next;
};
struct external *head_external = NULL;
