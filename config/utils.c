#include "utils.h"
#include "constant.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **  get_grammaire_file(char * url){
    FILE *file = fopen(url, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", url);
        return NULL;
    }

    // Count lines first
    int line_count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            line_count++;
        }
    }

    // Check if last line doesn't end with newline
    fseek(file, -1, SEEK_END);
    ch = fgetc(file);
    if (ch != '\n' && ch != EOF) {
        line_count++;
    }

    // Allocate array for line pointers (+ 1 for NULL terminator)
    char **lines = (char **)malloc((line_count + 1) * sizeof(char *));
    if (lines == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    // Reset to beginning of file
    fseek(file, 0, SEEK_SET);

    // Read lines
    char buffer[1024];
    int i = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && i < line_count) {
        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        // Allocate memory for this line
        lines[i] = (char *)malloc(len + 1);
        if (lines[i] == NULL) {
            printf("Error: Memory allocation failed for line %d\n", i);
            // Free previously allocated lines
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

    // NULL terminate the array
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

    // Parse each line
    for (int i = 0; lines[i] != NULL; i++) {
        char *line = lines[i];

        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }

        // Parse format: "A -> bB" or "B -> \t"
        char state;
        char transition_input;
        char next_state;
        int is_terminal = 0;

        // Find the state (before "->")
        int pos = 0;
        while (line[pos] == ' ') pos++; // Skip leading spaces
        state = line[pos];
        pos++;

        // Skip to "->"
        while (line[pos] != '-' && line[pos] != '\0') pos++;
        if (line[pos] == '\0') continue; // Invalid format
        pos += 2; // Skip "->"

        // Skip spaces after "->"
        while (line[pos] == ' ') pos++;

        // Check for terminal/epsilon (\t)
        if (line[pos] == '\\' && line[pos + 1] == 't') {
            is_terminal = 1;
            transition_input = '\0';
            next_state = '\0';
        } else {
            // Read transition input and next state
            transition_input = line[pos];
            pos++;

            if (line[pos] != '\0' && line[pos] != ' ') {
                next_state = line[pos];
            } else {
                next_state = '\0';
            }
        }

        // Find or create the node for this state
        node *current_node = get_node(l, state);

        if (current_node == NULL) {
            // Create new node
            Transitions trans = NULL;
            if (!is_terminal) {
                add_children(&trans, transition_input, next_state);
            }
            add_node_to_liste(&l, trans, state, is_terminal);
        } else {
            // Add transition to existing node
            if (!is_terminal) {
                add_children(&(current_node->transitions), transition_input, next_state);
            }
            if (is_terminal) {
                current_node->is_final = 1;
            }
        }
    }

    // Free the lines array
    for (int i = 0; lines[i] != NULL; i++) {
        free(lines[i]);
    }
    free(lines);

    return l;
}

int check_string(char * ch, list l){

    return 0;
}
