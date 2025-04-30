#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;

struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}


TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

/*
1.- Implemente la función createTreeMap. Esta función recibe la función de comparación de claves y crea un 
mapa (TreeMap) inicializando sus variables. El siguiente código muestra como inicializar la función de 
comparación. Reserve memoria, inicialice el resto de variables y retorne el mapa.
*/
TreeMap *createTreeMap(int (*lower_than) (void *key1, void *key2)) 
{
    TreeMap *nuevo = (TreeMap *) malloc(sizeof(TreeMap)) ;

    if (nuevo == NULL)  // Verficación de reserva de memoria
        exit(EXIT_FAILURE) ;
    
    // Inicialización de variables
    nuevo->root = NULL ;
    nuevo->current = NULL ;
    nuevo->lower_than = lower_than ;

    return nuevo ;  // Retorna el mapa
}

/*
3.- Implemente la función void insertTreeMap(TreeMap * tree, void* key, void * value). Esta función inserta un 
nuevo dato (key,value) en el árbol y hace que el current apunte al nuevo nodo. Para insertar un dato, primero debe 
realizar una búsqueda para encontrar donde debería ubicarse. Luego crear el nuevo nodo y enlazarlo. Si la clave del 
dato ya existe retorne sin hacer nada (recuerde que el mapa no permite claves repetidas).
*/
void insertTreeMap(TreeMap *tree, void *key, void *value) 
{
    TreeNode *padre = NULL ;
    TreeNode *current_aux = tree->root ;

    while (current_aux != NULL)
    {
        if (is_equal(tree, key, current_aux->pair->key))    // Si la clave ya existe, no se inserta nada
            return ;

        padre = current_aux ;

        if (tree->lower_than(key, current_aux->pair->key))
            current_aux = current_aux->left ;
        else
            current_aux = current_aux->right ;
    }
    
    TreeNode *nuevo = createTreeNode(key, value) ;  // Se crea el nuevo nodo
    nuevo->parent = padre ;

    if (tree->lower_than(key, padre->pair->key))
        padre->left = nuevo ;
    else
        padre->right = nuevo ;

    tree->current = nuevo ; // Apunta al nuevo nodo que se insertó
}

/*
4.- Implemente la función TreeNode * minimum(TreeNode * x). Esta función retorna el nodo con la mínima clave ubicado 
en el subárbol con raiz x. Para obtener el nodo tiene que, a partir del nodo x, irse por la rama izquierda hasta 
llegar al final del subárbol. Si x no tiene hijo izquierdo se retorna el mismo nodo.
*/
TreeNode *minimum(TreeNode *x)
{
    while (x->left != NULL) // Mientras la rama izquierda no sea NULL
    {
        x = x->left ;   // Seguirá avanzando por la izquierda
    }
    return x ;  // Retorna el nodo con la minima clave
}

/*
5.- Implemente la función void removeNode(TreeMap * tree, TreeNode* node). Esta función elimina el nodo node del 
árbol tree. Recuerde que para eliminar un node existen 3 casos: Nodo sin hijos: Se anula el puntero del padre que 
apuntaba al nodo Nodo con un hijo: El padre del nodo pasa a ser padre de su hijo Nodo con dos hijos: Descienda al 
hijo derecho y obtenga el menor nodo del subárbol (con la función minimum). Reemplace los datos (key,value) de node 
con los del nodo "minimum". Elimine el nodo minimum (para hacerlo puede usar la misma función removeNode).
*/
void removeNode(TreeMap *tree, TreeNode *node) 
{
    if (node->left == NULL && node->right == NULL)  // Caso 1: Nodo sin hijos
    {
        if (node->parent == NULL)
            tree->root = NULL ;
        else
        {
            if (node->parent->left == node)
                node->parent->left = NULL ;
            else
                node->parent->right = NULL ;
        }
        free(node->pair) ;
        free(node) ;
    
        return ;
    }

    if (node->left == NULL || node->right == NULL)  // Caso 2: Nodo con un solo hijo
    {
        TreeNode *hijo ;

        if (node->left != NULL)
            hijo = node->left ;
        else
            hijo = node->right ;

        if (node->parent == NULL)
            tree->root = hijo ;
        else
        {
            if (node->parent->left == node)
                node->parent->left = hijo ;
            else
                node->parent->right = hijo ;
        }
        hijo->parent = node->parent ;
        
        free(node->pair) ;
        free(node) ;

        return ;
    }
    
    TreeNode *nodo_min = minimum(node->right) ; // Caso 3: Nodo con dos hijos
    node->pair->key = nodo_min->pair->key ;
    node->pair->value = nodo_min->pair->value ;

    removeNode(tree, nodo_min) ;
}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}

/*
2.- Implemente la función Pair* searchTreeMap(TreeMap* tree, void* key), la cual busca el nodo con clave igual 
a key y retorna el Pair asociado al nodo. Si no se encuentra la clave retorna NULL. Recuerde hacer que el current 
apunte al nodo encontrado.
*/
Pair *searchTreeMap(TreeMap *tree, void *key) 
{
    TreeNode *current_aux = tree->root ;

    while (current_aux != NULL)
    {
        if (is_equal(tree, key, current_aux->pair->key))
        {
            tree->current = current_aux ;
            return current_aux->pair ; // Se retorna el par en caso de que se encuentre
        }
        else if (tree->lower_than(key, current_aux->pair->key))
            current_aux = current_aux->left ;
        else
            current_aux = current_aux->right ;
    }

    return NULL ;   // Si no se encuentra la clave, retorna NULL
}

/*
7.- La función Pair* upperBound(TreeMap* tree, void* key) retorna el Pair con clave igual a key. En caso de no 
encontrarlo retorna el primer par asociado a una clave mayor o igual a key. Para implementarla puede realizar 
una búsqueda normal y usar un puntero a nodo auxiliar ub_node que vaya guardando el nodo con la menor clave mayor 
o igual a key. Finalmente retorne el par del nodo ub_node.
*/
Pair *upperBound(TreeMap *tree, void *key) 
{

    return NULL ;
}

/*
6.- Implemente las funciones para recorrer la estructura: Pair* firstTreeMap(TreeMap* tree) retorna el primer Pair 
del mapa (el menor). Pair* nextTreeMap(TreeMap* tree) retornar el siguiente Pair del mapa a partir del puntero 
TreeNode* current. Recuerde actualizar este puntero.
*/
Pair *firstTreeMap(TreeMap *tree) 
{

    return NULL ;
}

Pair *nextTreeMap(TreeMap *tree) 
{

    return NULL ;
}
