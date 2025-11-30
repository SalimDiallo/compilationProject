#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

children create_children(type value, type transition) {
    children c;
    c.transition = transition;
    c.value = value;
    c.next = NULL;
    return c;
}

void add_children(Transitions * transitions, type value, type transition){
    children *c = (children*)malloc(sizeof(children));
    if (!c) return; 
    c->transition = transition;
    c->value = value;
    c->next = NULL;

    if (*transitions == NULL) {
        *transitions = c;
    } else {
        children *cur = *transitions;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = c;
    }
}

node create_node(Transitions transitions, type value, int is_final){
    node n;
    n.is_final = is_final;
    n.value = value;
    n.transitions = transitions;
    n.next = NULL;
    return n;
}

void add_node_to_liste(list* l, Transitions transitions, type value, int is_final){
    node *n = (node*)malloc(sizeof(node));
    if (!n) return; 
    n->is_final = is_final;
    n->value = value;
    n->transitions = transitions;
    n->next = NULL;

    if (*l == NULL) {
        *l = n;
    } else {
        node *cur = *l;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = n;
    }
}

children * get_transition(node n, type value){
    Transitions t = n.transitions;
    while (t != NULL) {
        if (t->value == value) {
            return t;
        }
        t = t->next;
    }
    return NULL;
}

node * get_node(list l,type value){
    node *cur = l;
    while (cur != NULL) {
        if (cur->value == value) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

void afficher_liste(list l) {
    node *cur = l;
    while (cur != NULL) {
        children *trans = cur->transitions;
        printf("%c", cur->value);
        if (cur->is_final) {
            printf(" *");
        }
        printf(" : ");
        while (trans != NULL) {
            printf("->%c%c", trans->value, trans->transition);
            if (trans->next != NULL) {
                printf(" | ");
            }
            trans = trans->next;
        }
        printf("\n");
        cur = cur->next;
    }
}
