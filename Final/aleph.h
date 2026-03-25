#include "TADdataType.h"
/* interfaz para el lexer */
extern int yylineno;
void yyerror(char *);

/* nodos en el árbol de sintaxis abstracta */
/* todos tienen un nodetype inicial en común */
struct symbol{
    char *name;
    DataType value;
    struct ast *func;
    struct symlist *syms;
};

/* tabla de símbolos simple de tamaño fijo */
#define NHASH 9997
struct symbol symtab[NHASH];

struct symbol *lookup(char*);

/* lista de símbolos, para una lista de argumentos */
struct symlist{
    struct symbol *sym;
    struct symlist *next;
};

struct symlist *newsymlist(struct symbol *sym, struct symlist *next);
void symlistfree(struct symlist *sl);

struct symref{
    int nodetype;       /* tipo N */
    struct symbol *s;
};

struct symasgn{
    int nodetype;       /* tipo = */
    struct symbol *s;
    struct ast *v;      /* valor */
};

struct elemval{//numval
    int nodetype;
    char* elem;
};

struct ast{
    int nodetype;
    struct ast *l;
    struct ast *r;
};

struct if_struct{
    int nodetype;
    struct ast *cond;
    struct ast *thn;
    struct ast *els;
};

struct for_struct{
    int nodetype;
    struct ast* var;
    struct ast* exp;
    struct ast* stmts; 
};

struct asgnmult{
    int nodetype;
    struct symlist *vars;
    struct ast *vals;
};

struct fncall{
    int nodetype;
    struct symbol *f;
    struct ast *param;
};


/* construcción del AST */
struct ast *newast(int nodetype, struct ast *l, struct ast *r);
struct ast *newelem(int nodetype, char* d);
struct ast *newref(struct symbol *s);
struct ast *newasgn(struct symbol *s, struct ast *v);
struct ast *newif(int nodetype, struct ast *cond, struct ast *th, struct ast *els);
struct ast *newfor(int nodetype, struct ast *var, struct ast *exp, struct ast *stmts);
void newfn(struct symbol *s, struct symlist* param, struct ast *body);
struct ast *newfncall(struct symbol *s, struct ast* param);
void *newasgnmult(struct symlist* vars, struct ast *vals);


/* evaluar un AST */
DataType eval(struct ast *);

/* borrar y liberar un AST */
void freeast(struct ast* );

/* debug */
void mostrar_conj(struct ast*);

void mostrar_arbol(DataType);

void mostrar_ast(struct ast*);

/*operaciones aparte*/
int comparacion(DataType A, DataType B, int cmp);

DataType operacion(DataType A, DataType B);

DataType callfndef(struct fncall* f);

struct symlist *copiar_symlist(struct symlist *orig);