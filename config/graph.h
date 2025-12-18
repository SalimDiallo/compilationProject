#ifndef GRAPH_H
#define GRAPH_H
#include <stddef.h>

typedef char type;

typedef struct Children {
    type value;
    type transition;
    struct Children * next; 
} children;

typedef children * Transitions;

typedef struct Node{
    type value;
    int is_final;
    Transitions transitions;
    struct Node * next;
} node;

typedef node * list;

children create_children(type value,type transition);


void add_children(Transitions * transitions, type value, type transition);

node create_node(Transitions transitions, type value, int is_final);

void add_node_to_liste(list* l, Transitions transitions, type value, int is_final);

children * get_transition(node n, type value);

node* get_node(list l,type value);

void afficher_liste(list l);



#endif // GRAPH_H
