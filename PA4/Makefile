SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build

CC = gcc
CXX = g++
YACC = bison
LEX = flex

CFLAGS = -Wall -g -I$(BUILD_DIR) -I$(SRC_DIR) -I$(LIB_DIR)
CXXFLAGS = -Wall -g -I$(BUILD_DIR) -I$(SRC_DIR) -I$(LIB_DIR)

TARGET = $(BUILD_DIR)/compiler

DRIVER_SRC    = $(SRC_DIR)/driver.cpp
CODEGEN_SRC   = $(SRC_DIR)/codegen.cpp
OPTIMIZER_SRC = $(SRC_DIR)/optimizer.cpp

AST_SRC       = $(LIB_DIR)/ast.cpp
ERRORS_SRC    = $(LIB_DIR)/errors.cpp
IR_GEN_SRC    = $(LIB_DIR)/ir_generator.cpp
IR_SRC        = $(LIB_DIR)/ir.cpp
SEMANTICS_SRC = $(LIB_DIR)/semantics.cpp
SYMTAB_SRC    = $(LIB_DIR)/symbol_table.cpp

PARSER_SRC    = $(LIB_DIR)/parser.y
LEXER_SRC     = $(LIB_DIR)/lexer.l

PARSER_C      = $(BUILD_DIR)/parser.tab.c
PARSER_H      = $(BUILD_DIR)/parser.tab.h
LEXER_C       = $(BUILD_DIR)/lex.yy.c

OBJS = $(BUILD_DIR)/driver.o \
       $(BUILD_DIR)/codegen.o \
       $(BUILD_DIR)/optimizer.o \
       $(BUILD_DIR)/ast.o \
       $(BUILD_DIR)/errors.o \
       $(BUILD_DIR)/ir_generator.o \
       $(BUILD_DIR)/ir.o \
       $(BUILD_DIR)/semantics.o \
       $(BUILD_DIR)/symbol_table.o \
       $(BUILD_DIR)/parser.tab.o \
       $(BUILD_DIR)/lex.yy.o

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/parser.tab.o: $(PARSER_C)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lex.yy.o: $(LEXER_C)
	$(CC) $(CFLAGS) -c $< -o $@

$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	$(YACC) -d -o $(PARSER_C) $(PARSER_SRC)

$(LEXER_C): $(LEXER_SRC) $(PARSER_H)
	$(LEX) -o $(LEXER_C) $(LEXER_SRC)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf tests/mips/

.PHONY: all clean test