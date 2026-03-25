#ifndef TADDATATYPE_H
#define TADDATATYPE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR 1
#define SET 2
#define LIST 3

struct dataType{
	int nodeType;
	union{
		char *dataStr;
		struct{
			struct dataType *data;
			struct dataType *next; 
		};
	};
};

typedef struct dataType* DataType;

DataType iniciaDT();
void agrega_elem(DataType C, DataType nuevo);
DataType copiar(DataType c);	
int iguales(DataType A, DataType B);
	
/*OPERACIONES GENERALES*/
DataType createDT(char *s); //completo
void freeDT(DataType);	//completo
void printDT(DataType d);	//completo
int typeDT(DataType d);		//completo

/*operaciones con STR*/
char* strDT(DataType d);	//completo
char* concatSTR(const char* str1, const char* str2);

/*operaciones con SET y LIST*/
DataType elemDT(DataType CL, int pos);//completo

/*operaciones con LIST*/
void push(DataType L, DataType elem); //completo
DataType pop(DataType* L); 	//completo
int sizel(DataType L); 	//completo
DataType concatenar(DataType A, DataType B);

/*operaciones con SET*/
int card(DataType S);	//completo
int in(DataType S, DataType elem);	//completo
DataType unionn(DataType A, DataType B);
DataType inter(DataType A, DataType B);
DataType diff(DataType A, DataType B);
int incluido(DataType A, DataType B);

//Aleph
DataType limpiar(DataType);
void asignaTipo(DataType);

#endif

