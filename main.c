#include <stdio.h>

#include "config/graph.h"
#include "config/utils.h"


int main(){
    list l = get_grammaire_list("input/grammaire.txt");
    if (l != NULL) {
        printf("Grammar loaded successfully:\n");
        afficher_liste(l);
    } else {
        printf("Failed to load grammar\n");
    }
    printf("result : %d", check_string("bdddccccdcc", l));

    return 0;
}