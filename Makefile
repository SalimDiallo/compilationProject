# Makefile for Grammar Checker Project

CC = gcc
CFLAGS = -Wall -Wextra
GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0)

# Source files
CONFIG_SRC = config/graph.c config/utils.c config/constant.c
UI_SRC = ui/start.c ui/grammar_ui.c
MAIN_SRC = main.c

# Object files
CONFIG_OBJ = $(CONFIG_SRC:.c=.o)

# Targets
all: main grammar_app

# Console version
main: $(MAIN_SRC) $(CONFIG_SRC)
	$(CC) $(CFLAGS) $(MAIN_SRC) $(CONFIG_SRC) -o main

# GTK GUI version
grammar_app: $(UI_SRC) $(CONFIG_SRC)
	$(CC) $(CFLAGS) $(UI_SRC) $(CONFIG_SRC) -o grammar_app $(GTK_FLAGS)

# Clean build files
clean:
	rm -f main grammar_app $(CONFIG_OBJ)

# Run console version
run: main
	./main

# Run GUI version
run-gui: grammar_app
	./grammar_app

.PHONY: all clean run run-gui
