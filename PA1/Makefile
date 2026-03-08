CC = gcc
LEX = flex
PYTHON = python

SRC_DIR = src
BUILD_DIR = build

LEXER_SOURCE = $(SRC_DIR)/lexer.l
DRIVER_SOURCE = $(SRC_DIR)/driver.cpp

LEXER_GENERATED_C = $(BUILD_DIR)/lex.yy.c
LEXER_EXE = $(BUILD_DIR)/lexer

all: $(LEXER_EXE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LEXER_GENERATED_C): $(LEXER_SOURCE) | $(BUILD_DIR)
	$(LEX) -o $(LEXER_GENERATED_C) $(LEXER_SOURCE)

$(LEXER_EXE): $(LEXER_GENERATED_C) $(DRIVER_SOURCE)
	$(CC) -I$(SRC_DIR) $(LEXER_GENERATED_C) $(DRIVER_SOURCE) -o $(LEXER_EXE)

test: all
	$(PYTHON) run_tests.py

clean:
	rm -rf $(BUILD_DIR)
