# Aleph

Aleph es un lenguaje de programación interpretado diseñado para la manipulación de conjuntos, listas y cadenas de caracteres. El nombre proviene de la primera letra del alfabeto hebreo, representando el origen y fundamento de las estructuras de datos que maneja el lenguaje.

## Características

- **Manipulación de conjuntos**: Operaciones de unión, intersección y diferencia
- **Manipulación de listas**: push, pop, tamaño
- **Cardinalidad**: Cálculo del número de elementos en un conjunto
- **Pertenencia**: Verificación de elementos dentro de conjuntos
- **Funciones**: Definición y llamado de funciones personalizadas
- **Estructuras de control**: if/else, while, for
- **Tipos de datos**: Cadenas, conjuntos y listas

## Operaciones de Conjuntos

| Operador | Descripción |
|----------|-------------|
| `+` | Unión/Concatenación |
| `^` | Intersección |
| `-` | Diferencia |
| `cardinal` | Cardinalidad (número de elementos) |
| `in` | Verificar pertenencia |

## Operaciones de Listas

| Operador | Descripción |
|----------|-------------|
| `push` | Agregar elemento |
| `pop` | Eliminar último elemento |
| `sizel` | Tamaño de la lista |

## Asignación de Variables

### Asignación Simple
```
aleph> 'x' = 5
aleph> 'x'
5
```

### Asignación Múltiple
```
aleph> let ('a', 'b', 'c') = (1, 2, 3)
aleph> 'a'
1
```

## Estructuras de Control

### If-Else
```
if (condición) { 
    // código 
} else { 
    // código 
}
```

### While
```
while (condición) { 
    // código 
}
```

### For-Each
```
for 'elemento' in conjunto { 
    // código 
}
```

## Definición de Funciones

```
define fn_nombre(parámetros) {
    // cuerpo de la función
    return expresión;
}
```

## Compilación
### Requisitos
El proyecto utiliza Flex y Bison para el análisis léxico y sintáctico.

- Flex
- Bison
- GCC

### Compilar y ejecutar
flex tpt.l
bison -d tpt.y
gcc lex.yy.c tpt.tab.c TADdataType.c aleph.c -lfl
.\a.exe

## Estructura del Proyecto

```
TPTfinal/
├── Final/
│   ├── aleph.h          # Header principal con definiciones del AST
│   ├── aleph.c          # Implementación del intérprete
│   ├── tpt.l            # Archivo Lex/Flex
│   ├── tpt.y            # Archivo Yacc/Bison
│   ├── tpt.tab.c        # Parser generado
│   ├── tpt.tab.h        # Header del parser
│   ├── TADdataType.h    # Definiciones del TAD
│   └── TADdataType.c    # Implementación del TAD
└── README.md
```

### Para ver ejemplos de casos de uso mirar el archivo pruebitas.txt

## Licencia

Proyecto académico de Serena Ch. Rosas para la materia Teoria de la Computacion II - Universidad Nacional de Salta
