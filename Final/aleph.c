#include "aleph.h"
#include "tpt.tab.h"
#include <stdlib.h>
#include <stdio.h>

struct ast* newast(int type, struct ast* l, struct ast* r){
    struct ast* A = (struct ast*)malloc(sizeof(struct ast));
    if(A!=NULL){
        A-> nodetype = type;
        A-> l = l;
        A-> r = r;
    }
    else{
        yyerror("sin memoria");
        exit(0);
    }
    return A;
}

struct ast* newelem(int nodetype, char* s){
    struct elemval * A = (struct elemval*)malloc(sizeof(struct elemval));
    if(A!=NULL){
        A-> nodetype = nodetype;
        A-> elem = s;
    }
    else{
        exit(0);
    }
    return (struct ast*)A;
}

struct ast* newif(int nodetype, struct ast *cond, struct ast *thn, struct ast *els){
    struct if_struct* aux = (struct if_struct*)malloc(sizeof(struct if_struct));  

    if(!aux){
        ("\nno hay espacio");
        exit(0);
    }
    aux -> nodetype = nodetype;
    aux -> cond = cond;
    aux -> thn = thn;
    aux -> els = els;
    
    return (struct ast*)aux;
}

struct ast *newfor(int nodetype, struct ast *var, struct ast *exp, struct ast *stmts){
    struct for_struct* aux = (struct for_struct*)malloc(sizeof(struct for_struct));

    if(!aux){
        yyerror("\nno hay espacio");
        exit(0);
    }
    aux -> nodetype = nodetype;
    aux -> var = var;
    aux -> exp = exp;
    aux -> stmts = stmts;

    return (struct ast*)aux;
}

void newfn(struct symbol *func, struct symlist *param, struct ast *body){
    if(param != NULL){
        func -> syms = param;
        func -> func = body;
    }
    else{
        func -> syms = NULL;
        func -> func = body;
    }
    printf("Funcion \"%s\" definida.\n", func -> name);
   
}

void *newasgnmult(struct symlist* vars, struct ast *vals){
   while(vars != NULL && vals != NULL){
        vars -> sym -> value = eval(vals->l);
        vars = vars -> next;
        vals = vals -> r;
   }
   
   if(vals == NULL && vars != NULL){
        yyerror("algunas variables no se asignaron.");
   }
}

struct ast *newfncall(struct symbol *f, struct ast *param){
    struct fncall *aux = (struct fncall *)malloc(sizeof(struct fncall));

    if(!aux){
        yyerror("\nno hay espacio");
        exit(0);
    }
    aux -> nodetype = FUNC;
    aux -> f = f;
    aux -> param = param;

    return (struct ast*)aux;
}

/* ******TRANSFORMACION DEL AST A DATATYPE****** */
DataType transforma(struct ast *b){
    struct dataType* aux = (struct dataType*)malloc(sizeof(struct dataType));
if(b!=NULL){
    switch (b -> nodetype){
    case SET:
        if(b->l != NULL){
            b = b->l;
            aux -> nodeType = SET;
            aux -> next = transforma(b->r);
            aux -> data = transforma(b->l);
            asignaTipo(aux);
            aux = limpiar(aux);
        }else{
            aux -> nodeType = SET;
            aux -> next = NULL;
            aux -> data = NULL;
        }
        break;
    
    case LIST:
        if(b->l != NULL){
            b = b->l;
            aux -> nodeType = LIST;
            aux -> next = transforma(b->r);
            aux -> data = transforma(b->l);
            asignaTipo(aux);
        }else{
            aux -> nodeType = LIST;
            aux -> next = NULL;
            aux -> data = NULL;
        }
        break;

    case 'L':
        aux -> next = transforma(b->r);
        aux -> data = transforma(b->l);
        break;

    case STR:
        aux = createDT(((struct elemval*)b)->elem);
        break;

    case UNION_CONCAT: case INTER: case DIFF: 
        aux = eval(b);
        break;

    case POP: 
        aux = eval(b);
        break;
    
    case PUSH:
        aux = eval(b);
        break;

    case 'N': 
        aux = ((struct symref *)b)->s->value;  
        break;

    default:
    printf("Error en el nodo %c\n", b->nodetype);
    aux = NULL;
        break;
    }
}else{
    return NULL;
    }  
    return aux;
}



/********************** INTERPRETE ************************** */
DataType eval(struct ast *a){
    struct dataType* aux = (struct dataType*)malloc(sizeof(struct dataType));
    struct dataType* der = (struct dataType*)malloc(sizeof(struct dataType));
    struct dataType* izq = (struct dataType*)malloc(sizeof(struct dataType));
    char* s = (char*)malloc(sizeof(char)*255);

    if(!a){
        yyerror("null eval");
        exit(0);
    }
    switch (a -> nodetype){
        
        case SET: case LIST: case STR: 
            aux = transforma(a);
        break;

        /* recuperar valor de variable*/
        case 'N': 
            aux = ((struct symref *)a)->s->value; 
        break;
    
        /* asignación */
        case '=': aux = ((struct symasgn *)a)->s->value = 
                    copiar(eval(((struct symasgn *)a)->v));
        break;

        /*Operaciones de listas y conjuntos*/

        case UNION_CONCAT://union y concatenacion
            if(a -> r != NULL && a -> l != NULL){
                izq = copiar(eval(a -> l));
                der = copiar(eval(a -> r));
                aux = operacion(izq, der);
            } 
            else{ 
                aux=NULL;
            }
            freeDT(izq);
            freeDT(der);
        break;

        case INTER:
            if(a -> r != NULL && a -> l != NULL){
                izq = copiar(eval(a -> l));
                der = copiar(eval(a -> r));
                if(izq -> nodeType == SET && der -> nodeType == SET){
                    aux = inter(izq, der);
                }
                else{
                    aux = NULL;
                    yyerror("(^)argumentos invalidos.");
                }
                freeDT(der);
                freeDT(izq);
            }
            else{ 
                aux = NULL;
            }
        break;

        case DIFF:
            if(a -> r != NULL && a -> l != NULL){
                izq = copiar(eval(a -> l));
                der = copiar(eval(a -> r));
                if(izq -> nodeType == SET && der -> nodeType == SET){
                    aux = diff(izq, der);
                }
                else{
                    aux = NULL;
                    yyerror("(-)argumentos invalidos.");
                }
                freeDT(izq);
                freeDT(der);
            }
            else{ 
                aux = NULL;
            }
        break;  

        case POP:
            if(a -> r != NULL){
                der = eval(a -> r);
                if(der -> nodeType == LIST){
                    aux = pop(&der);
                    if(a -> r -> nodetype == 'N'){
                        ((struct symasgn *)a -> r) -> s -> value = copiar(der);
                    }
                }
                else{
                    yyerror("(POP)argumento invalido.");
                    aux = NULL;
                }
                freeDT(der);
            }
            else{
                aux = NULL;
            }
        break;

        case PUSH: 
            if(a -> r != NULL && a -> l != NULL){
                der = eval(a -> r);
                izq = eval(a -> l);
                if(der -> nodeType == LIST){
                    push(der, izq);
                    aux = copiar(der);  
                }
                else{
                    aux = NULL;
                    yyerror("(PUSH)argumento invalido.");
                }
                //freeDT(der);
                //freeDT(izq);
            }
            else{
                aux = NULL;
            }
        break;

        case CARDINAL: 
            if(a -> r != NULL){
                der = eval(a -> r);
                if(der -> nodeType == SET){
                    sprintf(s, "%d", card(der));
                    aux = createDT(s);
                }
                else{
                    aux = NULL;
                    yyerror("(CARD)el argumento no es SET");
                }       
            }
            else{
                aux = NULL;
            }
            break;

        case SIZEL: 
            if(a -> r != NULL){
                der = eval(a -> r);
                if(der -> nodeType == LIST){
                    sprintf(s, "%d", sizel(der));
                    aux = createDT(s);
                }
                else{
                    yyerror("(SIZEL)el argumento no es LIST.");
                    aux = NULL;
                }
            }
            else{
                aux = NULL;
            }
            break;

        case IN:
            if(a -> r != NULL && a -> l != NULL){
                if(a->r->nodetype != STR){
                    if(in(eval(a -> r),eval(a->l))){
                        aux = createDT("true");
                    }
                    else{
                        aux = createDT("false");
                    }
                }
                else{
                    aux = NULL;
                    yyerror("(IN)argumentos invalidos.");
                } 
            }
            else{
                aux = NULL;
            }
            break; 
        
        /*lista de sentencias*/
        case ';': 
            if(a -> l != NULL){
                if(a -> r != NULL){
                    eval(a -> l);
                    aux = eval(a -> r);
                }
                else{
                    aux = eval(a -> l);
                }
            }
            else{
                aux = NULL;
            }
            break;

         /*operaciones logicas*/
        case AND:
            if(a -> l != NULL && a -> r != NULL){ 
                if(strcmp(strDT(eval(a->l)), "true") == 0 && strcmp(strDT(eval(a->r)), "true") == 0){
                    aux = createDT("true");
                }
                else{
                    aux = createDT("false");
                }
            }
            break;

        case OR:
             if(a -> l != NULL && a -> r != NULL){ 
                if(strcmp(strDT(eval(a->l)), "true") == 0 || strcmp(strDT(eval(a->r)), "true") == 0){
                    aux = createDT("true");
                }
                else{
                    aux = createDT("false");
                }
            }
            break;

        case NOT:
            if(a -> l != NULL){
                if(strcmp(strDT(eval(a->l)), "true") == 0){
                    aux = createDT("false");
                }
                else{
                    aux = createDT("true");
                }
            }
            break;

        /*print*/
        case PRINT: 
            if(a -> l != NULL){
                aux = eval(a -> l);
                printf("> ");
                printDT(aux);
                printf("\n");
            }
            else{
                aux = NULL;
            }
            break;

        /*estructuras de control*/
        case IF: 
            if(a -> l != NULL && a -> r != NULL){
                aux = eval(((struct if_struct *)a) -> cond);
                if(aux != NULL){
                    if(strcmp(strDT(aux), "true") == 0){
                        aux = eval(((struct if_struct *)a) -> thn);
                    }
                    else{
                        if(((struct if_struct *)a) -> els != NULL){
                            aux = eval(((struct if_struct *)a) -> els);   
                        }
                        else{
                            aux = NULL;
                        }
                    }
                }
            }
            else{
                aux = NULL;
            }
            break;

        case WHILE:
            aux = NULL;
            if(a -> l != NULL && a -> r != NULL){
                izq = eval(a->l);
                if(izq != NULL){
                    while(strcmp(strDT(eval(a->l)), "true") == 0){
                        aux = eval(a -> r);
                    }
                }
                else{
                    aux = NULL;
                }
            }
            break;
        
        case FOR_EACH:
            if( a -> l != NULL && a -> r != NULL){ 
                izq = copiar(eval(((struct for_struct*)a)->exp));
                if(izq -> nodeType == SET){
                    for(int i = 1; i <= card(izq); i++){
                        aux = elemDT(izq, i);
                        ((struct symasgn*)((struct for_struct*)a)->var)->s->value = aux; 
                        aux = eval(((struct for_struct *)a)->stmts);
                    }
                }
                else{
                    if(izq -> nodeType == LIST){
                       for(int i = 1; i <= sizel(izq); i++){
                        aux = elemDT(izq, i);
                        ((struct symasgn*)((struct for_struct*)a)->var)->s->value = aux;  
                        aux = eval(((struct for_struct *)a)->stmts);
                        } 
                    }
                    else{
                        aux = NULL;
                        yyerror("(FOR)argumento invalido.");
                    }
                }
            }
            else{
                aux = NULL;
            }
            break;

        /*COMPARACIONES*/
        case 4: //igualdad
            if(a -> l != NULL && a -> r != NULL){
                izq = eval(a->l);
                der = eval(a->r);
                if(izq && der){
                    if(iguales(eval(a -> l), eval(a -> r))){
                        aux = createDT("true");
                    }
                    else{
                        aux = createDT("false");
                    }
                }
                else{
                    aux = NULL;
                }
            }
            else{
                aux = NULL;
            }
            break;

        case 5: // distincion
            if(a -> l != NULL && a -> r != NULL){
                if(iguales(eval(a -> l), eval(a -> r))){
                    aux = createDT("false");
                }
                else{
                    aux = createDT("true");
                }
            }
            else{
                aux = NULL;
            }
            break;

        case 6: case 7: case 8: case 9: //inclusion y mayor/menor
            if(a -> l != NULL && a -> r != NULL){
                izq = eval(a->l);
                der = eval(a->r);
                if(izq != NULL && der != NULL){
                    if(comparacion(izq, der, a -> nodetype)){
                        aux = createDT("true");
                    }
                    else{
                        aux = createDT("false");
                    }
                }
                else{
                    aux = NULL;
                }  
            }
            else{
                aux = NULL;
            }
            break;

        /*booleanos*/
        case V: 
            aux = createDT("true");
            break;
        case F:
            aux = createDT("false");
            break;

        /*FUNCIONES*/
        case FUNC:
            if(a -> l != NULL){
                aux = callfndef((struct fncall*)a);
            }
            else{
                aux = NULL;
            }
            break;
        
        case RETURN:
            if(a -> l != NULL){
                aux = eval(a -> l);
            }
            else{
                aux = NULL;
            }
            break;
    }
    return aux;
}


/*******Tabla de simbolos*******/

static unsigned symhash(char *sym){
    unsigned int hash = 0;
    unsigned c;
    
    while(c = *sym++) hash = hash*9 ^ c;
    
    return hash;
}

struct symbol *lookup(char *sym){
    struct symbol *sp = &symtab[symhash(sym)%NHASH];
    int scount = NHASH;
    
    while(--scount >= 0){
        if(sp->name && !strcmp(sp->name, sym)) 
            return sp;
        
        if(!sp->name){
            sp->name = strdup(sym);
            sp->value = 0;
            sp->func = NULL;
            sp->syms = NULL;
            return sp;
        }
        
        if(++sp >= symtab+NHASH)    /* prueba con la siguiente entrada */
            sp = symtab;
    }
    yyerror("la tabla de símbolos está agotada\n");
    abort();
}

struct symlist *newsymlist(struct symbol *sym, struct symlist *next){
    struct symlist *sl = malloc(sizeof(struct symlist));
    
    if(!sl){
        yyerror("no hay espacio");
        exit(0);
    }
    
    sl->sym = sym;
    sl->next = next;
    return sl;
}

struct ast *newref(struct symbol *s){
    struct symref *a = malloc(sizeof(struct symref));
    
    if(!a){
        yyerror("no hay espacio");
        exit(0);
    }
    
    a->nodetype = 'N';
    a->s = s;
    return (struct ast *)a;
}

struct ast *newasgn(struct symbol *s, struct ast *v){
    struct symasgn *a = malloc(sizeof(struct symasgn));
    
    if(!a){
        yyerror("\nError: no hay espacio");
        exit(0);
    }
    
    a->nodetype = '=';
    a->s = s;
    a->v = v;
    return (struct ast *)a;
}

void symlistfree(struct symlist *sl){
    struct symlist *nsl;
    
    while(sl){
        nsl = sl->next;
        free(sl);
        sl = nsl;
    }
}


/*Limpiar el arbol*/
void freeast(struct ast* a){

    switch (a->nodetype){

    case SET: case LIST:
        if(a->l !=NULL){
            freeast(a -> l);
        }
        (a) = NULL;
        break;

    case 'L':
        if((a) -> r != NULL){
            freeast(a -> r); 
            freeast(a -> l);
        }
        (a) -> l = NULL;
        (a) = NULL;
    break;
    
    case STR:
        free(((struct elemval*)a)->elem);
        ((struct elemval*)a)->elem = NULL;
        free(a);
        a = NULL;

    default:
        printf("");    
    }

}


/*******FUNCIONES APARTE*******/

/*Para mostrar el ast*/
void mostrar_ast(struct ast* A){
    if(A != NULL){
        printf(" ");
        if(A -> nodetype != STR){
            mostrar_ast(A -> l);
            mostrar_ast(A -> r);
        }else{
            printf("%s", ((struct elemval*)A)->elem);
        }
    }
}

void mostrar_conj(struct ast* A){
    if(A != NULL){
        struct ast * aux = A -> l;
        printf("\n");
        while(aux!=NULL){
            printf("%s ",((struct elemval*)aux->l)->elem);
            aux = aux -> r;
        }
    }else{
        printf("null");
    }
}

int comparacion(DataType A, DataType B, int cmp){

    int num1, num2;

    switch (cmp){
    case 6://<
        if(A -> nodeType == SET && B -> nodeType == SET){
            return incluido(A, B);
        }
        else{
            if(A->nodeType == STR && B->nodeType == STR){
                if(sscanf(strDT(A), "%d", &num1) == 1 && sscanf(strDT(B), "%d", &num2) == 1){
                    return (num1 < num2);
                }
                else{
                    yyerror("(CMP)uno de los argumentos no es un numero.");
                }
            }
            else{
                yyerror("(CMP)argumentos invalidos.");
            }
        }
        break;
    
    case 7: //>
        if(A -> nodeType == SET && B -> nodeType == SET){
            return incluido(B, A);
        }
        else{
            if(A->nodeType == STR && B->nodeType == STR){
                if(sscanf(strDT(A), "%d", &num1) == 1 && sscanf(strDT(B), "%d", &num2) == 1){
                    return (num1 > num2);
                }
                else{
                    yyerror("(CMP)uno de los argumentos no es un numero.");
                }
            }
            else{
                yyerror("(CMP)argumentos invalidos.");
            }
        }
        break;

        case 8: //<=
            if(sscanf(strDT(A), "%d", &num1) == 1 && sscanf(strDT(B), "%d", &num2) == 1){
                return (num1 <= num2);
            }
            else{
                yyerror("(CMP)uno de los argumentos no es un numero.");
            }
        break;

        case 9: //>=
            if(sscanf(strDT(A), "%d", &num1) == 1 && sscanf(strDT(B), "%d", &num2) == 1){
                    return (num1 >= num2);
                }
                else{
                    yyerror("(CMP)uno de los argumentos no es un numero.");
                }
            break;

        default:
        break;
    }
    return 0;
}

DataType operacion(DataType A, DataType B){

    if(A -> nodeType == SET && B -> nodeType == SET){
            return unionn(A, B);
        }
        else{
            if(A->nodeType == LIST && B->nodeType == LIST){
                 return concatenar(A, B);
            }
            else{
                if(A->nodeType == STR && B->nodeType == STR){
                    int num1, num2;
                    if(sscanf(strDT(A), "%d", &num1) == 1 && sscanf(strDT(B), "%d", &num2) == 1){
                        char* cadena;
                        sprintf(cadena, "%d", num1+num2);
                        return createDT(cadena);
                    }
                    else{
                        return createDT(concatSTR(strDT(A), strDT(B)));
                    }
                }
                else{
                    yyerror("(+)argumentos invalidos.");
                    return NULL;
                }
                
            }
        }
}

DataType callfndef(struct fncall *aux){

    if(!aux->f->func){
        yyerror("funcion indefinida.");
        return NULL;
    }
    struct symlist *pFormales = aux -> f -> syms;  //variables que se usan en la funcion
    struct ast *pActuales = aux -> param;    //parametros pasados
    struct symlist *oldvals = copiar_symlist(aux -> f -> syms);
    DataType c;
    int i; 

    /*verifico que haya igual cantidad de parametros que los de la definicion
    y asigno los valores pasados a los symbolos de la def*/
    while(pActuales != NULL && pFormales != NULL){ 
        pFormales -> sym -> value = copiar(eval(pActuales->l));
        pActuales = pActuales -> r;
        pFormales = pFormales -> next;
    }
    if(pActuales != NULL || pFormales != NULL){
        yyerror("la cantidad de parametros no coincide con los especificados en la definicion.");
        return NULL;
    }

    c = copiar(eval(aux->f->func));

    pFormales = aux->f->syms;
    while(oldvals != NULL){
        pFormales->sym->value = copiar(oldvals->sym->value);
        pFormales = pFormales->next;
        oldvals = oldvals -> next; 
    }
    symlistfree(pFormales);
    symlistfree(oldvals);

    return c;
}


struct symlist *copiar_symlist(struct symlist *orig) {
    if (orig == NULL) {
        return NULL;
    }

    struct symlist *copia = NULL, *ultimo = NULL, *nuevo;

    while (orig != NULL) {
        // Crear un nuevo nodo de symlist
        nuevo = (struct symlist *)malloc(sizeof(struct symlist));
        if (!nuevo) {
            yyerror("Error de memoria al copiar symlist.");
            return NULL;
        }

        // Crear una nueva estructura symbol
        nuevo->sym = (struct symbol *)malloc(sizeof(struct symbol));
        if (!nuevo->sym) {
            yyerror("Error de memoria al copiar symbol.");
            free(nuevo);
            return NULL;
        }

        // Copiar el nombre del símbolo
        nuevo->sym->name = strdup(orig->sym->name);
        if (!nuevo->sym->name) {
            yyerror("Error de memoria al copiar nombre del símbolo.");
            free(nuevo->sym);
            free(nuevo);
            return NULL;
        }

        // Copiar el valor del símbolo
        nuevo->sym->value = copiar(orig->sym->value);

        // Copiar la función (si existe)
        nuevo->sym->func = orig->sym->func;  // Se asume que `func` es una referencia

        // Copiar la lista de parámetros si la tiene
        nuevo->sym->syms = copiar_symlist(orig->sym->syms);

        nuevo->next = NULL;

        // Insertar en la lista copiada
        if (copia == NULL) {
            copia = nuevo;
        } else {
            ultimo->next = nuevo;
        }
        ultimo = nuevo;

        orig = orig->next;
    }

    return copia;
}
