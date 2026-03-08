CC = gcc
CXX = g++

SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build

CFLAGS   = -I$(SRC_DIR) -I$(BUILD_DIR)
CXXFLAGS = -I$(SRC_DIR) -I$(BUILD_DIR)

LEXER_SRC  = $(LIB_DIR)/lexer.l
PARSER_SRC = $(SRC_DIR)/parser.y
AST_SRC    = $(SRC_DIR)/ast.cpp
DRIVER_SRC = $(SRC_DIR)/driver.cpp

LEXER_C  = $(BUILD_DIR)/lex.yy.c
PARSER_C = $(BUILD_DIR)/parser.tab.c
PARSER_H = $(BUILD_DIR)/parser.tab.h

OBJ_LEX    = $(BUILD_DIR)/lex.o
OBJ_PARSE  = $(BUILD_DIR)/parser.o
OBJ_AST    = $(BUILD_DIR)/ast.o
OBJ_DRIVER = $(BUILD_DIR)/driver.o

TARGET = $(BUILD_DIR)/syntax


all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	bison -d -o $(PARSER_C) $(PARSER_SRC)

$(LEXER_C): $(LEXER_SRC) $(PARSER_H)
	flex -o $(LEXER_C) $(LEXER_SRC)

$(OBJ_PARSE): $(PARSER_C)
	$(CC) $(CFLAGS) -c $(PARSER_C) -o $(OBJ_PARSE)

$(OBJ_LEX): $(LEXER_C)
	$(CC) $(CFLAGS) -c $(LEXER_C) -o $(OBJ_LEX)

$(OBJ_AST): $(AST_SRC)
	$(CXX) $(CXXFLAGS) -c $(AST_SRC) -o $(OBJ_AST)

$(OBJ_DRIVER): $(DRIVER_SRC)
	$(CXX) $(CXXFLAGS) -c $(DRIVER_SRC) -o $(OBJ_DRIVER)

$(TARGET): $(OBJ_PARSE) $(OBJ_LEX) $(OBJ_AST) $(OBJ_DRIVER)
	$(CXX) -o $(TARGET) $(OBJ_PARSE) $(OBJ_LEX) $(OBJ_AST) $(OBJ_DRIVER)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean