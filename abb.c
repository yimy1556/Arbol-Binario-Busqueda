#include "abb.h"
#include <stdlib.h>
#include <sys/types.h>

#define IGUALES 0
#define MAYOR 1
#define MENOR -1
#define EXITO 0
#define ERROR -1
#define ELIMINAR 'E'
#define INSERTAR 'I'
#define BUSCAR 'B'
#define CANTIDAD_INICIAL 0
#define SIN_HIJOS 0
#define UN_HIJO 1
#define DOS_HIJOS 2

/******************
 * Estructura Aux *
 ******************/

typedef struct almazenador {
    void** elementos;
    int tamanio_elementos;
    int cantidad_elementos;
    bool termino;
}almazenador_t;

/*****************
 * Funciones AUX.*
 *****************/

/*
 */
nodo_abb_t** busqueda_ptr(nodo_abb_t** nodo_abb, abb_comparador comparador,void* elemento, char tipo_busqueda){
    
    if ( !(*nodo_abb) ) return nodo_abb;
    
    int comparacion = comparador((*nodo_abb)->elemento, elemento); 
    bool comdicion = (comparacion == IGUALES) && (tipo_busqueda != INSERTAR);
    if (comdicion) return nodo_abb;    
    
    return (comparacion == MENOR )? busqueda_ptr(&((*nodo_abb)->izquierda), comparador,elemento,tipo_busqueda):
        busqueda_ptr(&((*nodo_abb)->derecha),comparador,elemento,tipo_busqueda);
}

/*
 */
nodo_abb_t* nodo_abb_crear(void* elemento){
    nodo_abb_t* nodo_abb = malloc(sizeof(nodo_abb_t));

    if(!nodo_abb) return NULL;
    
    nodo_abb->elemento = elemento;
    nodo_abb->izquierda = NULL;
    nodo_abb->derecha = NULL;

    return nodo_abb;
}
/*
 */
bool guardar_elemento(void* elemento, void* contenedor){

    almazenador_t* almazenador = (almazenador_t*)contenedor;
    
    almazenador->elementos[almazenador->cantidad_elementos] = elemento;

    (almazenador->cantidad_elementos)++;

    return (almazenador->cantidad_elementos == almazenador->tamanio_elementos);
}

/*
 */
void recorrido_abb(nodo_abb_t* raiz,int recorrido, bool (*funcion)(void*, void*), void* extra, bool* termino){
    if(!raiz || *termino) return;
    
    if(recorrido == ABB_RECORRER_PREORDEN && !(*termino)) *termino = funcion(raiz->elemento, extra);
    recorrido_abb(raiz->derecha, recorrido, funcion, extra, termino);
    
    if(recorrido == ABB_RECORRER_INORDEN && !(*termino)) *termino = funcion(raiz->elemento, extra);
    recorrido_abb(raiz->izquierda, recorrido, funcion, extra, termino);
    
    if(recorrido == ABB_RECORRER_POSTORDEN && !(*termino))  *termino = funcion(raiz->elemento, extra);
}

/*
 */
int almazenage_de_elementos(abb_t* arbol, void** array,int tamanio_array,int recorrido){
    
    almazenador_t almazenador;
    almazenador.tamanio_elementos = tamanio_array;
    almazenador.elementos = array;
    almazenador.cantidad_elementos = CANTIDAD_INICIAL;
    almazenador.termino = false;

    if(arbol_vacio(arbol)) return almazenador.cantidad_elementos; 
    
    recorrido_abb(arbol->nodo_raiz,recorrido,guardar_elemento,&almazenador,&(almazenador.termino));
    
    return almazenador.cantidad_elementos;
}
/*
 */
void hoja_destruir(nodo_abb_t** hoja,abb_liberar_elemento destructor){
    if(destructor) destructor((*hoja)->elemento);
    free(*hoja);
    *hoja = NULL;
}
/*
 */
void hojas_destruir(nodo_abb_t** raiz, abb_liberar_elemento destructor){
    if(!(*raiz)) return;
    
    hojas_destruir(&((*raiz)->izquierda),destructor);

    hojas_destruir(&((*raiz)->derecha),destructor);

    hoja_destruir(raiz,destructor);

}

/*
 *post: devolvio el mayor de sus menores del nodo pasado por parametro.
 */
nodo_abb_t** obtener_mayor_de_menores(nodo_abb_t** hijo){
    
    return (!((*hijo)->izquierda))? hijo:obtener_mayor_de_menores(&(*hijo)->izquierda);
}


void swap(nodo_abb_t* elemento1, nodo_abb_t* elemento2){ 
    void* elemento_aux = elemento1->elemento;
    elemento1->elemento = elemento2->elemento;
    elemento2->elemento = elemento_aux;
}

/*
 */
int catidad_hijos(nodo_abb_t* raiz){
    int cant_hijos = SIN_HIJOS;
    if(raiz->izquierda == raiz->derecha) return cant_hijos;
    cant_hijos += (raiz->izquierda && raiz->derecha)? DOS_HIJOS:UN_HIJO;
        
    return cant_hijos;
}

/*
 */ 
void borrar_un_hijo(nodo_abb_t** nodo,abb_liberar_elemento destructor){
    nodo_abb_t* hijo = ((*nodo)->derecha)?(*nodo)->derecha:(*nodo)->izquierda;
    hoja_destruir(nodo,destructor);
    *nodo = hijo;
}

void borrar_dos_hijos(nodo_abb_t** nodo_a_borrar, abb_liberar_elemento destructor){
    nodo_abb_t** nodo_swap = obtener_mayor_de_menores( &(*nodo_a_borrar)->derecha);
    
    swap(*nodo_swap,*nodo_a_borrar);
    
    (catidad_hijos(*nodo_swap) == UN_HIJO)? borrar_un_hijo(nodo_swap, destructor)
    :hoja_destruir(nodo_swap, destructor); 
}

/*
 */
void borrar_con_hijos(nodo_abb_t** nodo_a_borrar,abb_liberar_elemento destructor,int cant_hijos){
    
    if (cant_hijos == DOS_HIJOS) borrar_dos_hijos(nodo_a_borrar, destructor);
    else
        borrar_un_hijo(nodo_a_borrar, destructor);
    
}

/*********************
 * Funciones del ABB.*
 *********************/

abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor){
    abb_t* abb = malloc(sizeof(abb_t));
    
    if(!abb) return NULL;
           
    abb->nodo_raiz = NULL;  
    abb->comparador = comparador;
    abb->destructor = destructor;
       
    return abb;
}

int arbol_insertar(abb_t *arbol, void *elemento){
    if(!arbol) return ERROR;

    nodo_abb_t** nodo = busqueda_ptr(&(arbol->nodo_raiz),arbol->comparador, elemento, INSERTAR);
    *nodo = nodo_abb_crear(elemento);   

    return (!(*nodo)) ? ERROR:EXITO;
}

int arbol_borrar(abb_t *arbol, void *elemento){
    
    if(!arbol || !(arbol->nodo_raiz)) return ERROR;
    
    nodo_abb_t** nodo_borrar = busqueda_ptr(&(arbol->nodo_raiz),arbol->comparador,elemento,ELIMINAR);
   
    int cant_hijos = catidad_hijos(*nodo_borrar);

    (cant_hijos == SIN_HIJOS)? hoja_destruir(nodo_borrar, arbol->destructor)
    :borrar_con_hijos(nodo_borrar, arbol->destructor, cant_hijos);  
    
    return EXITO;
}



void* arbol_buscar(abb_t *arbol, void *elemento){
    if(arbol_vacio(arbol)) return NULL;
    
    nodo_abb_t* nodo = *(busqueda_ptr(&(arbol->nodo_raiz),arbol->comparador,elemento,BUSCAR));
    
    return nodo->elemento;
}

void* arbol_raiz(abb_t *arbol){
   
    return (arbol_vacio(arbol))? NULL:arbol->nodo_raiz->elemento;
}

bool arbol_vacio(abb_t *arbol){
    bool condicion = (!arbol && !(arbol->nodo_raiz));
    return condicion;
}

int arbol_recorrido_inorden(abb_t *arbol, void **array, int tamanio_array){
       
    return almazenage_de_elementos(arbol, array, tamanio_array, ABB_RECORRER_INORDEN);
}

int arbol_recorrido_preorden(abb_t *arbol, void **array, int tamanio_array){
    
    return almazenage_de_elementos(arbol, array, tamanio_array, ABB_RECORRER_PREORDEN); 
}

int arbol_recorrido_postorden(abb_t *arbol, void **array, int tamanio_array){
    
    return almazenage_de_elementos(arbol, array, tamanio_array, ABB_RECORRER_POSTORDEN);
}

void arbol_destruir(abb_t *arbol){
    if(!arbol) return;
    
    hojas_destruir(&(arbol->nodo_raiz), arbol->destructor);
    free(arbol);
}

void abb_con_cada_elemento(abb_t *arbol, int recorrido, bool (*funcion)(void *, void *), void *extra){
    if(arbol_vacio(arbol) || !funcion) return;
    
    bool terminar = false;
    recorrido_abb(arbol->nodo_raiz, recorrido, funcion, extra, &terminar);
}
