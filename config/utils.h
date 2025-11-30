#ifndef UTILS_H
#define UTILS_H
#include "graph.h"

char **  get_grammaire_file(char * url);

list get_grammaire_list(char * line_transitions_tab);

list parse_grammaire_text(char * grammar_text);

int check_string(char * ch, list l);

#endif // UTILS_H
