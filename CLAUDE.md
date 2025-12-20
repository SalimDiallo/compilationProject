# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a compiler/formal grammar processing project written in C. The project implements a graph-based representation of formal grammars using finite state automata concepts. The main components include:

- Graph structures to represent grammar states and transitions
- Utilities for parsing and processing grammar files or manual text input
- Console application for basic grammar testing
- GTK3 GUI with dual input modes: file loading and manual text entry
- Support for reading grammar definitions from text files (see `input/grammaire.txt`)

## Build & Run

**Using Makefile (recommended):**
```bash
make              # Build all targets
make main         # Build console version only
make grammar_app  # Build GUI version only
make run          # Build and run console version
make run-gui      # Build and run GUI version
make clean        # Clean build files
```

**Manual compilation:**

Console version:
```bash
gcc main.c config/graph.c config/utils.c config/constant.c -o main
./main
```

GUI version (requires GTK3):
```bash
gcc ui/start.c ui/grammar_ui.c config/graph.c config/utils.c config/constant.c -o grammar_app $(pkg-config --cflags --libs gtk+-3.0)
./grammar_app
```

**Prerequisites for GUI:**
```bash
sudo apt-get install build-essential libgtk-3-dev
```

## Architecture

### Core Data Structures (config/graph.h)

The project uses a linked-list based graph structure to represent finite automata:

- **`children`**: Represents a transition edge with a `value` (input symbol), `transition` (target state), and pointer to next transition
- **`Transitions`**: Typedef for `children*` - a linked list of transitions from a state
- **`node`**: Represents a state/node in the automaton with:
  - `value`: State identifier (char)
  - `is_final`: Flag indicating if this is an accepting state
  - `transitions`: Linked list of outgoing transitions
  - `next`: Pointer to next node in the list
- **`list`**: Typedef for `node*` - a linked list of all nodes/states in the graph

### Module Structure

- **config/graph.{c,h}**: Core graph/automaton data structures and manipulation functions
  - Node creation and management (`create_node`, `add_node_to_liste`)
  - Transition handling (`add_children`, `get_transition`)
  - Graph traversal and display (`afficher_liste`, `get_node`)

- **config/utils.{c,h}**: Higher-level utilities for grammar processing
  - `get_grammaire_file(char *url)`: Reads grammar file and returns array of lines (char**)
  - `get_grammaire_list(char *file_path)`: Parses grammar file into graph structure (list)
  - `parse_grammaire_text(char *text)`: Parses grammar text string into graph structure (for manual input mode)
  - `check_string(char *str, list l)`: Validates if a string is accepted by the grammar automaton

- **config/constant.{c,h}**: Project constants

- **main.c**: Console entry point demonstrating grammar loading and string checking

- **ui/**: GTK3 graphical user interface
  - `start.c`: GUI application entry point
  - `grammar_ui.{c,h}`: UI implementation with:
    - Tabbed interface for file loading vs. manual input
    - Grammar visualization showing states and transitions
    - String validation interface with color-coded results (green for accepted, red for rejected)

### Grammar File Format

Grammar files in `input/` use the format:
```
A -> aB
B -> cB
B -> \t
```
Where:
- `A -> aB` means: from state A, on input 'a', transition to state B
- `\t` represents terminal/epsilon transitions (marks the state as final)

### Data Flow

1. **Grammar Input**: Either load from file (`get_grammaire_list`) or parse manual text (`parse_grammaire_text`)
2. **Graph Construction**: Parse rules into linked list of nodes with transitions
3. **String Validation**: `check_string` traverses the graph following input symbols, checking if a final state is reached

## Important Notes

- All graph operations use manual memory management - ensure proper malloc/free pairing
- Nodes and transitions are stored as linked lists for flexibility
- The project is modular - graph logic is separated from grammar parsing utilities
- Type `type` is aliased to `char` for state and symbol representation
- The GUI supports both file-based and manual text input modes via tabs
