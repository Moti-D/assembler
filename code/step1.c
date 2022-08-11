#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "matrix_of_legal_commend.h"
#include "step2.c"


bool insert_rows_and_compile_step1(char*); 
void input_symbol_and_commend_type(char*,int,bool);
void idenfity_date_type(char*,char*,bool,int);
void update_array_date(char [],int);
void update_table(int, char*, char[],int);
void insert_encoded_commend(int,char[],bool,int);
void update_string(char [],int);
void commend_step(int,char*,bool,int);
void encoding_commend(int, char*, int, bool, int);
void update_extern(char*, int);

/* the method input row from file step by step and call validation_row_from_begin method for every row */
bool insert_rows_and_compile_step1(char file_input[]) 
{
	FILE *f;
	bool end_file=false,file_exist=true; /* variable mark when finish to read the file and file_exist mark if exist this file */
	int id_line=0;  /* variable for identify row on the file */
	char *file_input_as=(char*) malloc((strlen(file_input)+3)*sizeof(char));   /* assign string array for new name with .as */
   	strcpy(file_input_as,file_input);
   	strcat(file_input_as,".as");   /* update name.as */
	if((f=fopen(file_input_as,"r")))
	{ 
		while(!end_file)	
		{
			int i=0,j=0; 	/* i is for refrance charcter j is for character of notice sentance */
			char c=' '; 
			char current_row[81];	 /* array for input the row */
			/* while inpute the data until end row without notice sentance and update current row var */
			while(fread(&c,sizeof(char),1,f) && c!='\n' && c!=';' && i<80) 
			{
				current_row[i++]=c;
			}
			current_row[i]='\0'; 
			id_line++; 	/* update the first or next row on the file */
			/* check why finish the the privios while */
			if(c==';')  /* if notice sentance skip to the next row */
			{
				while(fread(&c,sizeof(char),1,f) && c!='\n' && ++j && j!=80); 
			}
			if(c!='\n' && c!=';' && i<80)  /*	check if end file by two pararamter these */
			{
				end_file=true;
			}
			if(i) /* check if readed character in this row */
			{
				if(i<80 && j<80)  /* check integrity of length row*/
				{
					input_symbol_and_commend_type(current_row,id_line,false); /* call to work about this row */
				}
				else /* update and print error */
				{
					while(fread(&c,sizeof(char),1,f) && c!='\n'); /* skip to the next row */
					update_and_print_error(false,true,"the row is too long",id_line);	
				}
			}
		}
		fclose(f);
	}
	else 
		file_exist=false;
	return file_exist;	
}

/* the method read the row from begin, update symbol on table and check commend type */
void input_symbol_and_commend_type(char current_row[], int id_line, bool symbol)
{
	int j=0,i=0,i_row=0; 	 /* j for identify commdend, i for array of symbol name i_row id of current row */
	static char label[max_label]; 	 /* var of lable  */
	while(i_row<strlen(current_row) && current_row[i_row]==' ') { i_row++; } /* skip empty chracter */
	if(symbol) 
	{	/* update the symbol on table by data or commend by code */
		if(current_row[i_row]!='.')
			update_table(IC(0)+DC(0),label,"code",id_line);	
	}
	if(isalpha(current_row[i_row]))  /* check option that is commend or label */ 
	{
		continue_array(-1); /* update non conyinue array of data type */
		while(i_row<strlen(current_row) && (isalpha(current_row[i_row]) || isdigit(current_row[i_row])) && i<31) /* sequance of work checking with 32 max*/
		{
			label[i++]=current_row[i_row++];  /* while update in label var sequence of digit or alpha */
		}
		label[i]='\0';  /* update end string */
		if(i==2 && label[0]=='r' && isdigit(label[1]) && label[1]<'8') /* check that label dosn't as hamster name */
		{
			update_and_print_error(false,true,"label or commend show as memory hamster",id_line); /* update and print error */
		}
		else if(i==31)  /* check exception on label */
		{
			update_and_print_error(false,true,"label or commend is too long",id_line);  /* update and print error */
		}
		else  /* no error update commend or label */
		{
			while(j<16)
			{	/* check on matrix if is commend */
				if(!strncmp(commends[j],label,i))
				{	
					if(j>13)	/* check if it's commend wihtout word or character after */
					{
						if(skip_to_end_commend(false,current_row+i_row)) /* check if no charcter until end row */
						{
							encoding_commend(j,"",0,false,id_line); /* encoding this commend */
							break;
						}
						else
						{		/* update and print error */
							update_and_print_error(false,true,"ilegal character after commend",id_line);
							break;
						}
					}
					else if(current_row[i_row]==' ') /* check if space between commend and who contain in */
					{
						commend_step(j,current_row+i_row,false,id_line); 
						break;  /* call to the method the check data of commend */
					}
					else
					{		/* update and print error */
						update_and_print_error(false,true,"no space between commend to data commend",id_line);
						break;
					}
				}
				j++;
			}
			if(j==16) /* exist when the word isnt commend */
			{
				if (!symbol && current_row[i_row]==':') 
				{
					input_symbol_and_commend_type(current_row+i_row+1,id_line,true);
				}	/* call the method again for with symbol on memory */
				else
				{	/* update and print error */
					update_and_print_error(false,true,"ilegal word isnt label and not commend",id_line);
				}
			}
		}
	}
	else if(current_row[i_row]=='.') /* check if is start of data commend */ 	
		idenfity_date_type((current_row+i_row+1),label,symbol,id_line); /* call for this method to identify this commend*/
	else	/* its not begin of symbol and commend update and print error */
		update_and_print_error(false,true,"ilegal chracter",id_line);
}

/* the method idenfity_date_type identify commend type of data  */
void idenfity_date_type(char current_row[],char label[], bool symbol, int id_line) 
{
	int j=0,i_row=0;  /* j parameter for commends data type  i_row for current_row pointer */
	enum {data,string,exter,entry};  /* define commends type by 0-3 */
	while(i_row<strlen(current_row))  
	{	/* compar via matrix and i_row+1 to calculat relative until to end word */
		while(strncmp(current_row,creat_array_cype[j],i_row+1) && j<4) { j++; } 
		if(j==4) 
		{		/* when j=4 the previos strncmp can't find commend in commend matrix */
			update_and_print_error(false,true,"show . but not found commend of data type",id_line); /* update and print error */
			break;
		}
		if(i_row==4)
		{	/* check special case of extern and string is name of commend in c language and execute menual calculate */ 
			if(i_row+1<strlen(current_row) && ((j==2 && current_row[i_row+1]!='n') || (j==1 && current_row[i_row+1]!='g')))
			{
				update_and_print_error(false,true,"show . but not found commend of data type",id_line); 
				break;      /* update and print error */
			}
		}
		if(i_row+1==strlen(creat_array_cype[j])) /* if found commend until the end word */
		{ 
			if(j)
				continue_array(-1); /* update that no continue of array data for the next step */
			switch(j)	/* check which commedns in parameter j by enum numbering */
			{	
				case data:
					if(symbol)
					{   /* check if befor the commend have a symbol or this is continue of array */
						update_table(IC(0)+DC(0),label,"data",id_line);
						update_array_date(current_row+i_row+1,id_line); /* update symbol if symbol is on */
					}
					else if(continue_array(0)) 
						update_array_date(current_row+i_row+1,id_line);
					else	/* array wihtout label update and print error */
						update_and_print_error(false,true,".data without symbol",id_line);  
					break;
				case string:
					if(symbol) /* check if befor commend readed symbol and update the symbol on table */
					{
						update_table(IC(0)+DC(0),label,"data",id_line);
						update_string(current_row+i_row+2,id_line);
					}
					else	/* update and print error */
						update_and_print_error(false,true,"string without symbol",id_line);
					break;
				case exter:
					if(!symbol)	/* check if befor commend readed symbol */
						update_extern(current_row+i_row+2,id_line);
					else	/* print  warming notice */
						printf("line %d warming extern with symbol",id_line); 
					break;
				case entry:
					if(symbol)	/* print  warming notice */
						printf("line %d warming entry with symbol",id_line);
					break;		
			}
			break;	
		}	
		i_row++;	
	}
	if(i_row==strlen(current_row)) /*check if the while ended becasue end row so do not found commend */
		update_and_print_error(false,true,"show . but not found commend of data type",id_line); /* update and print error */
}  

/* this method update array date to database */ 
void update_array_date(char current_row[], int id_line)
{	/* define i_row to use in cuurent_row pointer, castemp is var to keep convert char to int, limit name to use pointer number */ 
	int i_row=0,castemp,limit_num; 
	bool isnt_empty=false;  /* marker if inserted min 1 number after data commend */
	char number[8],binary_to_insert[size_row]; /* defind number array to insert num and binary_to_insert to save converting in binary encoding */ 
	if(i_row<strlen(current_row) && current_row[i_row++]==' ') /* check if have a space between data->number */
	{
		while(i_row<strlen(current_row))
		{
			limit_num=0;  /* intialized for mark begin of num */
			if((current_row[i_row]=='-' || current_row[i_row]=='+') && i_row+1<strlen(current_row) && isdigit(current_row[i_row+1])) 
			{ 
				number[limit_num++]=current_row[i_row++]; /* update - or + to first charcter of number */
			}
			if(isdigit(current_row[i_row]))
			{
				while(i_row<strlen(current_row)  && isdigit(current_row[i_row]))
				{
					if(limit_num==8)
					{	/* check if size digit of number bigger than 8 it's to longer update and print error */
						update_and_print_error(false,true,"the number is too long",id_line);
						break;
					}
					number[limit_num++]=current_row[i_row++]; /* update number character step by step */
				}
				if(limit_num<7)
					number[limit_num]='\0'; /* update that end_row */
				sscanf(number, "%d", &castemp);  /* convert pointer number to int and save it in castemp */
				if((castemp<0 && -castemp>max_num_data) || (castemp>0 && castemp>=max_num_data))
				{    /* check exception in size of number 2^23 update and print error */
					update_and_print_error(false,true,"the number is too long",id_line); 
					break;
				}
				convert_decimal_to_binarry(castemp,binary_to_insert); /* convert and insert number to binary_to_insert by var castemp and method dedicated */
				insert_encoded_commend(DC(1)-1,binary_to_insert,true,id_line); /* update number on datebase */
				isnt_empty=true; /* mark that commend date has input the number and not empty */
				if(skip_to_end_commend(false,current_row+i_row))
				{	/* check if end row by skip_to_tnd method */
					continue_array(1);  /* update that this array could be continue on the next line */
					break;
				}
				if(skip_to_end_commend(true,current_row+i_row))	/* when dont end file check that have a , */
				{
					i_row+=skip_to_end_commend(true,current_row+i_row)-1; /* skip to  1 character after , */
					isnt_empty=false;  /* update that method much input more 1 number */
				}
				else
				{		/* that ilegal character after number that inserted */
					update_and_print_error(false,true,"ilegal charceter after number that inputed",id_line);
					break;
				}
			}
			else
			{	
				if(current_row[i_row]!=' ')
				{	/* if ilegal chracter update and print error and exit from the method */
					update_and_print_error(false,true,"ilegal chracter",id_line);
					break;
				}
			}
			i_row++;
		}
	}	
	if(!isnt_empty)
	{     
		update_and_print_error(false,true,"no inpute number after data or ,",id_line);
	}
}

/* this method update data as string func */
void update_string(char current_row[], int id_line)
{
	int i_row=0;  /* i_row use to current_row pointer */
	char binary_to_insert[size_row]; /* binary_to_insert use to get char as asciiz num */
	if(i_row<strlen(current_row) && current_row[i_row++]==' ')	/* chaeck if have a space between commend */
	{
		while(i_row<strlen(current_row) && current_row[i_row]==' ') { i_row++; } /* skip empty character */
		if(i_row<strlen(current_row) && current_row[i_row++]=='"')  /* check if have open character " of string */
		{
			while(i_row<strlen(current_row) && current_row[i_row]!='"')
			{    /* read the character of string step by step */
				convert_decimal_to_binarry(current_row[i_row++],binary_to_insert); /* convert to binary in binary_to_insert */
				insert_encoded_commend(DC(1)-1,binary_to_insert,true,id_line);  /* update in database as DC */
			}
			convert_decimal_to_binarry(0,binary_to_insert); /* insert "\0" to at word */
			insert_encoded_commend(DC(1)-1,binary_to_insert,true,id_line);
			if(i_row<strlen(current_row) && current_row[i_row++]=='"') /* check if have a " to end string */
			{
				if(!skip_to_end_commend(false,current_row+i_row)) /* check if row isn't empty after this */
				{		/* update and print error */
					update_and_print_error(false,true,"ilegal character after this string",id_line); 	
				}
			}
			else	/* update and print error becase have not closing character */		
				update_and_print_error(false,true,"no closing character of string",id_line);
		}
		else	/* update and print error becase have not opening character */
			update_and_print_error(false,true,"no opening character of string",id_line);
	}
	else	/*error no space between commend to the next character */
		update_and_print_error(false,true,"no space between commend to the next character",id_line);			
}

/* the method identifier the data of commends and send to encoding by other method */
void commend_step(int commend_id, char current_row[], bool input_second, int id_line)
{
	int i_row=0,i=0,c=-1,new_i_row; /* i_row use for a pointer current_row, i for pointer contents_commend c for a contens type and new_i_row when skip to empty return new i_row or zero  */
	char contents_commend[max_label]; /* use to save contents_commend */
	while(i_row<strlen(current_row))
	{
		switch(current_row[i_row]) /* check content commend type */
		{
			case '#':
				c=0;
				break;
			case '&':
				c=2;
				break;
		}
		if(isalpha(current_row[i_row]))
		{	/* check the other 2 types r or label */
			c=1;
			if(current_row[i_row]=='r' && i_row+1<strlen(current_row) && isdigit(current_row[i_row+1]) && current_row[i_row+1]<'8' && (i_row+2==strlen(current_row) || (i_row+2<strlen(current_row) && (current_row[i_row+2]==' ' || current_row[i_row+2]==',')))) 
				c=3;  /*  if r and digit after until 7 like memory hamser */
		}
		if(c!=-1)	
		{		/* have a legal commends and c has update */
			while(operand_source[commend_id][i]!=c && i<=2) { i++; } /* search the operand source type in matrix */
			if(!input_second && i<=2)
			{
				i=0; /* intialized to zero to use i again */
				if(!c)
				{		/* if c=0 much to input imidiate */
					i_row++; /* input the next chararcter */
					while(i_row<strlen(current_row) && (isdigit(current_row[i_row]) || (i==0 && (current_row[i_row]=='-' || current_row[i_row]=='+')))) /* update operand source by this condition */ 
					{	/* check if size digit of number bigger than 8 it's to longer update and print error */
						if(i==8)
						{	
							update_and_print_error(false,true,"the number is too long",id_line);
							break;
						}
						contents_commend[i++]=current_row[i_row++];
					}
					contents_commend[i]='\0'; /* update the end array */
					if(i==0)
					{	/* when i=0 no inpute number after # then update error */
						update_and_print_error(false,true,"no number after #",id_line);	
						break; 
					}
					if((new_i_row=skip_to_end_commend(true,current_row+i_row)))
					{	/* skip until , and if no , return 0 to new_i_row */
						encoding_commend(commend_id,contents_commend,c,true,id_line); /* encoding this contents */
						commend_step(commend_id,current_row+i_row+new_i_row,true,id_line); /* call to the method agian to find operand target */
						break;
					}
					else	/* error no , after operand source update error */
					{
						update_and_print_error(false,true,"no , after operand source",id_line);
						break;
					}	
				}
				else
				{		/* operand type is & r ot label update operand source */
					while(i_row<strlen(current_row)  && (isalpha(current_row[i_row]) || isdigit(current_row[i_row]))) /* read and update operand traget */ 
					{	
						if(i==31)
						{	/* update error if operand source is a bigger than 31 */
							update_and_print_error(false,true,"operand source is a bigger than 31",id_line);
							break;
						}
						contents_commend[i++]=current_row[i_row++];
					}
					contents_commend[i]='\0';
					if(i==0)
					{	/* upfate error if havn't character alpha or digit so i dont rise */
						update_and_print_error(false,true,"ilegal operand source",id_line);
						break; 
					}
					if((new_i_row=skip_to_end_commend(true,current_row+i_row)))
					{	/* skip until , and if no , return 0 to new_i_row */
						encoding_commend(commend_id,contents_commend,c,true,id_line); /* encoding this contents */
						commend_step(commend_id,current_row+i_row+new_i_row,true,id_line); /* call to the method agian to find operand target */
						break;
					}
					else	/* error no , after operand source update error */
					{
						update_and_print_error(false,true,"no , after operand source",id_line);
						break;
					}
				}
			}
			else
			{
				i=0; /* intialized to zero to use again */
				while(operand_target[commend_id][i]!=c && i<=2 && i++); /* search the operand target type in matrix */
				if(i<=2)
				{
					i=0;
					if(!c)
					{		/* if c=0 much to input imidiate */
					i_row++;	 /* input the next chararcter */
						while(i_row<strlen(current_row) && (isdigit(current_row[i_row]) || (i==0 && (current_row[i_row]=='-' || current_row[i_row]=='+')))) /* update operand target by this condition */
						{	/* check if size digit of number bigger than 8 it's to longer update and print error */
							if(i==8)
							{	
								update_and_print_error(false,true,"the number is too long",id_line);
								break;
							}
							contents_commend[i++]=current_row[i_row++];
						}
						contents_commend[i]='\0'; /* update this is end row */
						if(i==0)
						{	/* when i=0 no inpute number after # then update error */
							update_and_print_error(false,true,"no number after #",id_line);	
							break; 
						}
						if(skip_to_end_commend(false,current_row+i_row))
						{	/* skip until end row return 0 to new_i_row */
							encoding_commend(commend_id,contents_commend,c,false,id_line); /* encoding this contents */
							break;
						}
						else	/* error ilegal character after operand target update error */
						{
							update_and_print_error(false,true,"ilegal chracter after operand target",id_line);
							break;
						}
					}
					else
					{
						if(c==2) /* if operand target is & could begin to read from the next character */
							i_row++;
						while(i_row<strlen(current_row) && (isdigit(current_row[i_row]) || isalpha(current_row[i_row]))) /* update operand target by this condition */
						{
							if(i==31)
							{	/* update error if operand source is a bigger than 31 */
								update_and_print_error(false,true,"operand targert is a bigger than 31",id_line);
								break;
							}
							contents_commend[i++]=current_row[i_row++];
						}
						contents_commend[i]='\0'; /* mark end row */
						if(i==0)
						{	/* upfate error if havn't character alpha or digit so i dont rise */
							update_and_print_error(false,true,"ilegal operand target",id_line);
							break; 
						}
						if(skip_to_end_commend(false,current_row+i_row))
						{	/* skip until end row return 0 to new_i_row */
							encoding_commend(commend_id,contents_commend,c,false,id_line); /* send contents to encod */
							break;
						}
						else	/* error ilegal character after operand target update error */
						{
							update_and_print_error(false,true,"ilegal chracter after operand target",id_line);
							break;
						}
					}
				}
				else	/* error ilegal operand target type */
				{
					update_and_print_error(false,true,"ilegal operand target type by this commend",id_line);
					break;
				}		
			}
		}
		else if(current_row[i_row]!=' ') /* ilegal operand type */
		{
			update_and_print_error(false,true,"ilegal operand type",id_line);
			break;
		}
		i_row++;
	}	
}
			
/* this method encod commend and update in database */
void encoding_commend(int commend_id, char data[], int operand_type, bool source, int id_line)
{
	int to_int,i;	/* to_int is save integer from char format and i for encoding_row pointer */
	static char encoding_row[size_row-1]; 	/* update encoding to encoding_row */
	static bool add_row_from_source=false;  /* mark when method with operand source need to add row */
	char binary[size_row-1]; /* define array to save encoding binary row */
	if(encoding_row[0]!='0') /* if the first time that i call to function intilized arrat to '0 for update just 1 */
	{
		for(i=0; i<24; i++)
			encoding_row[i]='0';
	}
	if(commend_id>13)  /* if commend id is 14,15 so stop or rts without operand */
	{
		convert_decimal_to_binarry(commend_id,binary); /* convert commend num to binary in 6 bit */
		strncpy(encoding_row,binary+18,6);  /* update in ecoding row opcode */
		encoding_row[21]='1';	/* upddate Absolut in 22 */
		insert_encoded_commend(IC(1)-1,encoding_row,false,id_line); /* update in database */
	}
	else
	{
		if(source)	/* if found operand source on the process */
		{
			convert_decimal_to_binarry(operand_type,binary);	/* convert commend num to binary in 2 bit */
			strncpy(encoding_row+6,binary+22,2); /* update operand type in binarty 2 bit */
			if(operand_type==3)	/* if operand type is memory hamster */
			{	
				sscanf(data+1,"%d",&to_int); /* inpute the number to to_int wothout r */
				convert_decimal_to_binarry(to_int,binary);  /* convert hamster num to binary */  
				strncpy(encoding_row+8,binary+21,3);  /* update binary hamster num to encoding row */ 
			}
			else
			{
				add_row_from_source=true; /* update add row in ic becase imidiate or label or & take mor encoding row */
				if(operand_type==0) /* this is imidiate type */
				{
					sscanf(data,"%d",&to_int);  /* input imidiate from date char to int */
					if(!(to_int<0 && -to_int>max_num_imdiate) || (to_int>0 && to_int>=max_num_imdiate)) /* check legaliztion size */
					{
						convert_decimal_to_binarry(to_int,binary);	/* convert to_int to binary and insert to binary array */
						strcpy(binary,binary+3);  /* move <- in 3 because ARE */
						strcpy(binary+21,"10"); 
						binary[23]='0';			/* updata Absolute and 00 after */			
						insert_encoded_commend(IC(0)+1,binary,false,id_line); /* update imidiate in database */
					}
					else	/* error imidiate number is too long */
						update_and_print_error(false,true,"imidiate number is too long",id_line);
				}
				else	/* if operand type is label or & update in database as unknow address to identify on the second step */
					pending_address(IC(0)+1,operand_type,data);	
			}
		}
		else 
		{
			convert_decimal_to_binarry(commends_code[commend_id][0],binary); /* convert opcode to binary */
			strncpy(encoding_row,binary+18,6);	/* save the opcode in encoding row */
			convert_decimal_to_binarry(operand_type,binary);  /* convert operand target to binary */
			strncpy(encoding_row+11,binary+22,2);	/* save operand target in encoding row */
			if(operand_type==3)
			{	/* if operand is memory hamster update num of hamster in encoding row */
				sscanf(data+1,"%d",&to_int);
				convert_decimal_to_binarry(to_int,binary);
				strncpy(encoding_row+13,binary+21,3);
			}
			convert_decimal_to_binarry(commends_code[commend_id][1],binary); /* convert and update func */
			strncpy(encoding_row+16,binary+19,5);
			encoding_row[21]='1';	/* update absolute */
			insert_encoded_commend(IC(1)-1,encoding_row,false,id_line);
			if(add_row_from_source)	  /* if in the previos call added operand source with more row */
			{
				IC(1);		/* update ic+1 and cancel add send */
				add_row_from_source=false;
			}		
			if(operand_type<3) /* if operand type ask add row */
			{
				if(operand_type==0)
				{	/* update imidiate */
					sscanf(data,"%d",&to_int); 
					if(!(to_int<0 && -to_int>max_num_imdiate) || (to_int>0 && to_int>=max_num_imdiate))
					{	/* inpute char number to_int and check legal size */
						convert_decimal_to_binarry(to_int,binary);
						strcpy(binary,binary+3); /* move <- 3 bit because ARE */
						strcpy(binary+21,"10"); /* update Absolute */
						binary[23]='0';
						insert_encoded_commend(IC(1)-1,binary,false,id_line);
					}
					else	/* error imidiate number is too long */
					{
						update_and_print_error(false,true,"imidiate number is too long",id_line);
						add_row_from_source=false;
					}
				}
				else/* if operand type is label or & update in database as unknow address to identify on the second step */
					pending_address(IC(1)-1,operand_type,data); 
			}
			else
				add_row_from_source=false;	
		}
	}
	if(!source)	/* if finish to update encoding row intialized array to next call */
	{	
		for(i=0; i<24; i++)
			encoding_row[i]='0';	
	}
}			

/* this method update date in extern commend */		
void update_extern(char current_row[], int id_line)
{
	int i_row=0,i=0; /* i_row to use current_row pointer an i for label pointer */ 
	char label[max_label];	/* array to save label */
	if(i_row<strlen(current_row) && current_row[i_row]==' ') /* check if have a space between commend */
	{
		while(++i_row<strlen(current_row) && current_row[i_row]==' '); /* skip empty character */
		if (i_row<strlen(current_row) && isalpha(current_row[i_row])) /* check if the first character is alpha */
		{
			while(i_row<strlen(current_row) && (isalpha(current_row[i_row]) || isdigit(current_row[i_row]))) 
			{	/* update the label step by step */
				if(i==31)
				{	/* update error if label is a bigger than 31 */
					update_and_print_error(false,true,"label is a bigger than 31",id_line);
					break;
				}
				label[i++]=current_row[i_row++];	/* update label step by step*/
			}
			label[i]='\0'; /* mark end label */
			if(skip_to_end_commend(false,current_row+i_row))  /* check if no chractr after label */
				update_table(0,label,"external",id_line);
			else	/* update and error ilegal character after label */
				update_and_print_error(false,true,"ilegal character after label",id_line);
		}
		else	/* update error ilegal character after extern */
			update_and_print_error(false,true,"ilegal character after extern",id_line);
	}
	else	/* error no space between commend to lable */
		update_and_print_error(false,true,"no space between commend to lable",id_line);	
}
