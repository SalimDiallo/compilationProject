#include <stdio.h>
#include <stdlib.h>

#include "config/graph.h"
#include "config/utils.h"
#include "config/constant.h"


int main(){

    // list l  = NULL;
    // Transitions A = NULL, B = NULL;
    // add_children(&A, 'a', 'B');
    // add_children(&A, 'b', 'B');
    // add_children(&B, 'a', 'B');

    // add_node_to_liste(&l, A, 'A', 0);
    // add_node_to_liste(&l, B, 'B', 1);

    // afficher_liste(l);

    // Test with grammar file
    list l = get_grammaire_list("input/grammaire.txt");

    if (l != NULL) {
        printf("Grammar loaded successfully:\n");
        afficher_liste(l);
    } else {
        printf("Failed to load grammar\n");
    }

    return 0;
}