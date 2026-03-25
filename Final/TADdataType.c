#include "TADdataType.h"
void asingaTipo(DataType a);
char* clean(char* s){
	if(strcmp(s, "{}")!=0 && strcmp(s, "[]")!=0){ //agregue el strcmp en caso de que se ingrese un conjunto sin nada
		int i=1;
		char* aux;
		aux=(char*) malloc(sizeof(char)*255); 
		while (s[i]!='\0'){
			aux[i-1]=s[i];
			i++;
		}
		aux[i-2]='\0';
		return aux;
	}else{
		return '\0';
	}	
}
	
	
void getElem(char**s, char**aux){
	char* s_aux;
	int i=0;
	int cont_abre_l=0;
	int cont_cierra_l=0;
	int cont_abre_c=0;
	int cont_cierra_c=0;
	int j,salir;
	
	*aux=(char*) malloc(sizeof(char)*255); 
	s_aux=(char*) malloc(sizeof(char)*255);
	if ((*s)[0]!='{' && (*s)[0]!='[') {
		while ((*s)[i]!='\0' && (*s)[i]!=',') {
			(*aux)[i]=(*s)[i];
			i++;
		}
		(*aux)[i]='\0';
	} else {
		salir=0;
		while ((*s)[i]!='\0' && !salir) {
			(*aux)[i]=(*s)[i];
			switch ((*s)[i]) {
			case '{':
			cont_abre_l++;
				break;
			case '[':
				cont_abre_c++;
				break;	
			case '}':
				cont_cierra_l++;
				break;
			case ']':
				cont_cierra_c++;
				break;
			}
			if (cont_abre_l-cont_cierra_l==0 && cont_abre_c-cont_cierra_c==0) 
				salir=1;
			i++;
		}
		(*aux)[i]='\0';
	}
	i++;
	if (i<strlen(*s)) {
		j=0;
		while ((*s)[i]!='\0'){
			s_aux[j]=(*s)[i];
			j++;
			i++;
		}
		s_aux[j]='\0';
		*s=s_aux;
	} else *s=NULL;
}
	
//Operaciones Generales
DataType iniciaDT(){
	DataType nvo = NULL;
	return nvo;
}	

void agrega_elem(DataType C, DataType nuevo){
	DataType aux=NULL;
	
	if(C->data != NULL)
	{
		aux = C;
		while(aux->next != NULL)
		{
		aux=aux->next;
		}
		aux->next = (struct dataType*)malloc(sizeof(struct dataType));
		aux->next->nodeType = aux->nodeType;
		aux->next->data = nuevo;
		aux->next->next = NULL;
	}
	else
	{
		C->data = nuevo;
	}
}

int iguales(DataType elem, DataType compara){
	DataType aux2= compara;
	DataType aux = elem;
	
	switch(elem->nodeType){
	case SET:
		if(compara->nodeType == SET){
			if(card(elem) == card(compara)){
				if(aux->data == NULL && aux2->data == NULL){
					return 1;
				}
				while(compara!=NULL && in(aux, compara->data)){
					compara=compara->next;
				}
				if(compara==NULL) return 1;
				else return 0;
			}
		}
		else return 0;
		break;
	case LIST:
		if(compara->nodeType == LIST){
			if(sizel(elem) == sizel(compara)){
				if(elem->data == NULL && compara->data == NULL){
					return 1;
				}
				while(aux!=NULL && iguales(aux->data, aux2->data)){
					aux=aux->next;
					aux2=aux2->next;
				}
				if(aux!=NULL)return 0;
				else return 1;
			}
		}
		else return 0;
		break;
	default :
		if(compara->nodeType == STR){
			if(strcmp(elem->dataStr, compara->dataStr)==0){
				return 1;
			}
			else return 0;
		}
		else return 0;
	}

	return 0;
}

int in(struct dataType* d, struct dataType* elem){
	struct dataType* aux = d;
	struct dataType* buscado = elem;
	
	if(aux != NULL){
		//if(aux->nodeType == SET){
			while(aux != NULL && aux->data != NULL){
				if(iguales(aux->data, buscado)){
					return 1;
				}
				aux = aux->next;
			}
		//}
	}
	return 0;
}
	
void createDT2(DataType C, char* cade){
	
	char *elem = NULL;
	DataType nuevo=NULL;
	cade=clean(cade);
	while(cade != NULL){
		getElem(&cade, &elem);
		nuevo = createDT(elem);
		if(in(C, nuevo) == 0){
			agrega_elem(C, nuevo);
		}
		printf("\nSi");
	}	
}

DataType createDT(char* cade){
	
	DataType nodo;
	nodo = NULL;
	nodo = (DataType)malloc(sizeof(struct dataType));
	
	if(nodo != NULL){
		switch(cade[0]){
		case '{':
			nodo->nodeType = SET;
			nodo->data=NULL;
			nodo->next=NULL;
			createDT2(nodo, cade);
			break;
		case '[':
			nodo->nodeType = LIST;
			nodo->data = NULL;
			nodo->next = NULL;
			createDT2(nodo, cade);
			break;
		default:
			nodo->nodeType = STR;
			nodo->dataStr=strdup(cade);
			break;
		}
	}
	return nodo;
}

void muestraDato(DataType C){
	
	if(C != NULL){
		if(C->next!=NULL){
			printDT(C->data);
			printf(",");
			muestraDato(C->next);	
		}
		else{
			printDT(C->data);
		}
	}
}

void printDT(DataType C){
	if(C!=NULL){
		if(C->data != NULL)
		{
			switch(C->nodeType){
			case SET:
				printf("{");
				muestraDato(C);
				printf("}");
				break;
			case LIST:
				printf("[");
				muestraDato(C);
				printf("]");
				break;
			default:
				printf("%s", C->dataStr);
				break;
			}
		}else{
			if(C->nodeType == SET){
				printf("{}");
			}
			else{
				printf("[]");
			}
		}
	}
	else{
		printf("elemento nulo");
	}
}

void freeDT(DataType C){
	
	if(C!=NULL){
		switch(C->nodeType){
		case SET:
			freeDT(C->next);	
			freeDT(C->data);
			C->data=NULL;
			C=NULL;
			break;

		case LIST:
			freeDT(C->next);
			freeDT(C->data);
			C->data=NULL;
			C=NULL;
			break;

		default:
			free(C->dataStr);
			C->dataStr=NULL;
			free(C);
			C=NULL;
		}
	}
}

char* strDT(DataType d){
	if(d != NULL && d->nodeType == STR){
		return d->dataStr;
	}
	else{
		return NULL;
	}
}

DataType copiar(DataType c){	
	
	if(c!=NULL){
		DataType copia=NULL;
		DataType aux=NULL;
		DataType copia2=NULL;
		
		aux=c;
		copia=(struct dataType*)malloc(sizeof(struct dataType));
		copia->nodeType=c->nodeType;
		copia2=copia;
		switch(c->nodeType){
		case STR:
			copia->dataStr=(char*)malloc(sizeof(char)*255);
			strcpy(copia->dataStr,c->dataStr);
			break;
		default:
			
			copia->data=copiar(aux->data);
			while(aux->next!=NULL){
				
				copia->next=(struct dataType*)malloc(sizeof(struct dataType));
				copia->next->nodeType=aux->nodeType;
				copia->next->data=copiar(aux->next->data);
				aux=aux->next;
				copia=copia->next;
			}
			copia->next=NULL;
			
		}
		return copia2;
	}
	else{
		return NULL;
	}
}


int card(DataType C){	
	int cont = 0;
	if(C!=NULL){
		if(C->nodeType == SET){
			if(C->data != NULL){
				while(C != NULL){
					C = C->next;
					cont++;
				}
			}
			return cont;
		}
	}
	
	return -1;	
}

DataType unionn(DataType A ,DataType B){

	if(A != NULL && B != NULL){
		if(A->nodeType == SET && B->nodeType == SET){
			DataType U = createDT("{}");
			DataType aux = A;
			while(aux!=NULL){
				agrega_elem(U, copiar(aux->data));
				aux=aux->next;
			}
		
			if(U->data != NULL){
				while(B != NULL && B->data != NULL){
					if(in(U, B->data)==0){
						agrega_elem(U, copiar(B->data));
					}
					B = B->next;
				}
				return U;
			}
			return copiar(B);
		}
		printf("\nError: un operando no es un conjunto.");
	}
	return NULL;
}

DataType inter(DataType A,DataType B){
	DataType C=createDT("{}");
	
	if(A!=NULL && B!=NULL){
		if(A->nodeType == SET && B->nodeType == SET){
			if(card(B) != 0){
				while(B != NULL){
					if(in(A, B->data)){
						agrega_elem(C, copiar(B->data));
					}
					B = B->next;
				}
			}
			return C;
		}
		printf("\nError: Error: un operando no es un conjunto.");
	}
	return NULL;
}

DataType diff(DataType A, DataType B){
	DataType C=createDT("{}");
	
	if(A->nodeType == SET && B->nodeType == SET){
		if(A->data!=NULL){
			if(card(B) != 0){
				while(A != NULL){
					if(in(B, A->data)==0){
						agrega_elem(C, copiar(A->data));
					}
					A = A->next;
				}
				return C;
			}
		}
		return A;
	}
	
	return NULL;
}

DataType pop(DataType* L){
	DataType ant = NULL;
	
	if((*L)!=NULL){
		if((*L)->nodeType == LIST){
			if(sizel(*L)>1){
				ant = (*L);
				(*L)=(*L)->next;
				ant->next=NULL;
				return ant->data;
			}
			else{
				ant = (*L);
				(*L)=(*L)->next;
				*L=createDT("[]");
				ant->next=NULL;
				return ant->data;
			}
		}
	}
	return NULL;
}

void push(DataType L, DataType elem){
	
	if(L != NULL){	
		if(L->nodeType == LIST){
			if(sizel(L)>0){
				while(L->next != NULL){
					L = L->next;
				}
				L->next = (struct dataType*)malloc(sizeof(struct dataType));
				L->next->nodeType=LIST;
				L->next->data=copiar(elem);
				L->next->next=NULL;
			}else{
				L->data=copiar(elem);
			}
		}
	}
}

int sizel(DataType L){
	int cont = 0;
	
	if(L!=NULL){
		if(L->nodeType == LIST){
			if(L->data!=NULL){
				while(L != NULL){
					L = L->next;
					cont++;
				}
			}
			return cont;
		}
	}
	return -1;
}

DataType elemDT(DataType CL, int pos){//retorna la posicion del elemento(que puede ser un STR, un conjunto o una lista) desde la pérspectiva de conjunto
	DataType aux = CL;
	int posaux=1;
	
	if(aux->data!=NULL){
		while(aux->next!=NULL && posaux<pos){
			aux = aux->next;
			posaux++;
		}
		if(aux!=NULL){
			return aux->data;
		}else{
			return NULL;
		}
	}else{
		printf("Conjunto Vacio\n");
		return NULL;
	}
}

DataType limpiar(DataType c){
	DataType copia = createDT("{}");
	while(c != NULL){
	
		if(in(copia, c->data) == 0){
			agrega_elem(copia, copiar(c->data));
		}
		c = c->next;
	}
	return copia;
}

void asignaTipo(DataType a){
	switch (a->nodeType)
	{
	case SET:
		a = a->next;
		while (a!=NULL){
			a->nodeType = SET;
			a = a->next;
		}
		break;
	case LIST:
		a = a->next;
		while (a!=NULL){
			a->nodeType = LIST;
			a = a->next;
		}
	}
}


/*Operaciones extra*/
int incluido(DataType A, DataType B){
	if(A != NULL && B != NULL){
		if(A -> data != NULL){
			while(A != NULL){
				if(in(B, A -> data)){
					A = A -> next;
				}
				else{
					return 0;
				}
			}
		}
		return 1;
	}
	return 0;
}

DataType concatenar(DataType A, DataType B){
	if(A != NULL && B != NULL){
		if(A -> nodeType == LIST && B-> nodeType == LIST){
			DataType aux = copiar(A);
			while(B != NULL){
				agrega_elem(aux, copiar(B->data));
				B = B -> next;
			}
			return aux;
		}
		printf("Error: un operando no es una lista.");
	}
	return NULL;
}

char* concatSTR(const char* str1, const char* str2) {
    // Calcular la longitud total necesaria (+1 para el carácter nulo '\0')
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t totalLen = len1 + len2 + 1;

    // Reservar memoria para la nueva cadena
    char* resultado = (char*)malloc(totalLen * sizeof(char));

    // Verificar si la asignación de memoria fue exitosa
    if (resultado == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        exit(EXIT_FAILURE);
    }

    // Copiar la primera cadena y luego concatenar la segunda
    strcpy(resultado, str1);
    strcat(resultado, str2);

    return resultado;
}
