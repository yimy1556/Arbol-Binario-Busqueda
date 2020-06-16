#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "abb.h"

#define IGUALES 0
#define MAYOR 1
#define MENOR -1
#define ANSI_COLOR_ROJO    "\x1b[1m\x1b[31m"
#define ANSI_COLOR_VERDE    "\x1b[1m\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define EXITO 0
#define ERROR -1

typedef struct elemento{
    int clave;
    void* contenido;
}elemento_t;

elemento_t*  crear_elemento(int clave){
    elemento_t* elem = malloc(sizeof(elemento_t));

    if (!elem) return NULL;

    elem->clave = clave;
    return elem;
}
int comparar_elemento(void* elemento, void* aux){
    
    if(!(aux)|| !(elemento)) return IGUALES;

    if(((elemento_t*)elemento)->clave > ((elemento_t*)aux)->clave)
        return MAYOR;
    
    if(((elemento_t*)elemento)->clave < ((elemento_t*)aux)->clave)
        return MENOR;
    
    return IGUALES;
}

void destruir_elemento(void* elemento){
    if(!elemento) return;
    
    free((elemento_t*)elemento);
}


void print_test(const char* mensaje, bool ok){
    char* color = (ok)?ANSI_COLOR_VERDE:ANSI_COLOR_ROJO;
    char* estado = (ok)? "OK\n":"ERROR\n";

    printf("  %s*%s => %s%s",color ,mensaje ,estado,ANSI_COLOR_RESET);
}

void prueba_crear_abb_vacio(){
    abb_t* abb = arbol_crear(comparar_elemento, destruir_elemento);
    
    elemento_t* aux = crear_elemento(8);

    print_test("Prueba crear abb",abb);
    print_test("Prueba abb raiz es NULL", (NULL == arbol_raiz(abb)));
    print_test("Prueba abb esta vacio", arbol_vacio(abb));
    print_test("Prueba abb buscar elemento que no esta", !(arbol_buscar(abb, aux)));
    print_test("Prueba abb borrar elemento que no esta", arbol_borrar(abb, aux) == ERROR);   
    destruir_elemento(aux);
    arbol_destruir(abb);
}

int main(){
    
    prueba_crear_abb_vacio();

    return 0;
}
