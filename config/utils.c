#include "utils.h"
#include "constant.h"
#include "graph.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **  get_grammaire_file(char * url){
    FILE *file = fopen(url, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", url);
        return NULL;
    }

    int line_count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    fseek(file, -1, SEEK_END);
    ch = fgetc(file);
    if (ch != '\n' && ch != EOF) {
        line_count++;
    }

    char **lines = (char **)malloc((line_count + 1) * sizeof(char *));
    if (lines == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fseek(file, 0, SEEK_SET);

    char buffer[1024];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && i < line_count) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        lines[i] = (char *)malloc(len + 1);
        if (lines[i] == NULL) {
            printf("Error: Memory allocation failed for line %d\n", i);
            for (int j = 0; j < i; j++) {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }

        strcpy(lines[i], buffer);
        i++;
    }

    lines[i] = NULL;

    fclose(file);
    return lines;
}

list get_grammaire_list(char * line_transitions_tab){
    if (line_transitions_tab == NULL) {
        return NULL;
    }

    list l = NULL;
    char **lines = get_grammaire_file(line_transitions_tab);

    if (lines == NULL) {
        return NULL;
    }

    for (int i = 0; lines[i] != NULL; i++) {
        char *line = lines[i];

        if (strlen(line) == 0) {
            continue;
        }

        char state;
        char transition_input;
        char next_state;
        int is_terminal = 0;

        int pos = 0;
        while (line[pos] == ' ') pos++; 
        state = line[pos];
        pos++;

        while (line[pos] != '-' && line[pos] != '\0') pos++;
        if (line[pos] == '\0') continue; 
        pos += 2; 

        while (line[pos] == ' ') pos++;

        if (line[pos] == '\\' && line[pos + 1] == 't') {
            is_terminal = 1;
            transition_input = '\0';
            next_state = '\0';
        } else {
            transition_input = line[pos];
            pos++;

            if (line[pos] != '\0' && line[pos] != ' ') {
                next_state = line[pos];
            } else {
                next_state = '\0';
            }
        }

        node *current_node = get_node(l, state);

        if (current_node == NULL) {
            Transitions trans = NULL;
            if (!is_terminal) {
                add_children(&trans, transition_input, next_state);
            }
            add_node_to_liste(&l, trans, state, is_terminal);
        } else {
            if (!is_terminal) {
                add_children(&(current_node->transitions), transition_input, next_state);
            }
            if (is_terminal) {
                current_node->is_final = 1;
            }
        }
    }

    for (int i = 0; lines[i] != NULL; i++) {
        free(lines[i]);
    }
    free(lines);

    return l;
}

int check_string(char * ch, list l){
    int n = strlen(ch);
    if (n == 0) return 0;
    if (l == NULL) return 0;

    node * current = l;

    for (int i = 0; ch[i] != '\0'; i++) {
        children * transition = get_transition(*current, ch[i]);

        if (transition == NULL) {
            if (current->is_final == 1) {
                return 1;
            }
            return 0;
        }

        node * next = get_node(l, transition->transition);
        if (next == NULL) {
            return 0;
        }
        current = next;
    }

    return current->is_final;
}
