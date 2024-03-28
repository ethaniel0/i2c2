//
// Created by Ethan Horowitz on 3/14/24.
//

#ifndef I2C2_TOKENTYPES_H
#define I2C2_TOKENTYPES_H
#include "tokenize.h"

enum TokenType {
    TYPE_NONE,
    TYPE_BRACKET,
    TYPE_SEPARATOR,
    TYPE_OPERATOR,
    TYPE_IDENTIFIER,
    TYPE_VALUE,
    TYPE_TYPE,
    TYPE_KEYWORD,
    TYPE_GROUP,
};

enum TokenValue {
    NONE,

    // brackets
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    GROUP,

    // separators
    COMMA,
    SEMICOLON,

    // operators
    ADD, ADD_EQ,
    MINUS, MINUS_EQ, NEG,
    MULT, MULT_EQ, DEREF,
    DIV, DIV_EQ,
    BIN_AND, BIN_AND_EQ, AND, REF,
    BIN_OR, BIN_OR_EQ, OR,
    XOR, XOR_EQ,
    MOD, MOD_EQ,
    NOT, NOT_EQ,
    GT, GTE,
    LT, LTE,
    EQ, EQ_EQ,

    // literals and values
    IDENTIFIER, STRING, NUMBER, CHARACTER, NIL, FUNCTION,

    // data types
    INT, FLOAT, CHAR, DOUBLE, LONG, SHORT, VOID,

    // Keywords
    IF, ELSE, FOR, WHILE, BREAK, CONTINUE, RETURN
};

std::string tokenTypeAsString(TokenType type);

std::string tokenValueAsString(TokenValue value);

class Token {
public:
    TokenType type;
    TokenValue val_type;
    std::string lexeme;
    int line;
    Token* parseParent;
    int depth;

    Token(TokenType type, TokenValue value, std::string lexeme, int line) {
        this->type = type;
        this->val_type = value;
        this->lexeme = std::move(lexeme);
        this->line = line;
        this->parseParent = nullptr;
        this->depth = 0;
    }

    [[nodiscard]] std::string toString() const {
        return "<" + tokenTypeAsString(type) + " " + tokenValueAsString(val_type) + " " + lexeme + " line " + std::to_string(line) + ">";
    }
};

class BinaryOpToken : public Token {
public:
    Token* left;
    Token* right;
    bool useLeft;
    bool useRight;

    BinaryOpToken(TokenValue value, std::string lexeme, int line) : Token(TokenType::TYPE_OPERATOR, value, std::move(lexeme), line) {
        left = nullptr;
        right = nullptr;
        useLeft = false;
        useRight = false;
    }
};

class GroupToken : public Token {
public:
    std::vector<Token*> expressions;

    GroupToken(TokenType type, std::string lexeme, int line) : Token(type, TokenValue::NONE, std::move(lexeme), line) {
        expressions = std::vector<Token*>();
    }

    void addExpression(Token* token) {
        expressions.push_back(token);
    }
};

class IfElseToken : public Token {
public:
    Token* condition;
    std::vector<Token*> elseIfConditions;
    GroupToken* ifBody;
    std::vector<GroupToken*> elseIfBodies;
    GroupToken* elseBody;

    IfElseToken(Token* condition, GroupToken* ifBody, std::vector<Token*> elseIfConditions, std::vector<GroupToken*> elseIfBodies, GroupToken* elseBody, int line) : Token(TokenType::TYPE_KEYWORD, TokenValue::IF, "if", line) {
        this->condition = condition;
        this->ifBody = ifBody;
        this->elseIfBodies = std::move(elseIfBodies);
        this->elseBody = elseBody;
        this->elseIfConditions = std::move(elseIfConditions);
    }
};

class FunctionCallToken : public Token {
public:
    Token* function;
    std::vector<Token*> arguments;
    TokenValue returnType;

    FunctionCallToken(Token* function, std::vector<Token*> arguments, std::string lexeme, int line) : Token(TokenType::TYPE_OPERATOR, TokenValue::FUNCTION, std::move(lexeme), line) {
        this->function = function;
        this->arguments = std::move(arguments);
    }
};

class DefinitionToken : public Token {
public:
    std::string name;
    Token* value;
    TokenValue valueType;
    int refCount;

    DefinitionToken(TokenValue valueType, std::string name, int line) : Token(TokenType::TYPE_OPERATOR, TokenValue::IDENTIFIER, "=", line) {
        this->valueType = valueType;
        this->name = std::move(name);
        this->value = nullptr;
        this->refCount = 0;
    }
};

class FunctionToken : public Token {
public:
    std::string name;
    std::vector<DefinitionToken*> parameters;
    GroupToken* body;
    TokenValue returnType;
    int refCount;

    FunctionToken(TokenValue valueType, std::string name, std::vector<DefinitionToken*> parameters, int line) : Token(TokenType::TYPE_KEYWORD, TokenValue::FUNCTION, "function", line) {
        this->name = std::move(name);
        this->parameters = std::move(parameters);
        this->body = nullptr;
        this->returnType = valueType;
        this->refCount = 0;
    }
};

class ReturnToken : public Token {
public:
    Token* value;

    ReturnToken(Token* value, int line) : Token(TokenType::TYPE_KEYWORD, TokenValue::RETURN, "return", line) {
        this->value = value;
    }
};

class ForToken : public Token {
public:
    Token* init;
    Token* condition;
    Token* increment;
    GroupToken* body;

    ForToken(Token* init, Token* condition, Token* increment, GroupToken* body, int line) : Token(TokenType::TYPE_KEYWORD, TokenValue::FOR, "for", line) {
        this->condition = condition;
        this->body = body;
        this->increment = increment;
        this->init = init;
    }

};

class WhileToken : public Token {
public:
    Token* condition;
    GroupToken* body;

    WhileToken(Token* condition, GroupToken* body, int line) : Token(TokenType::TYPE_KEYWORD, TokenValue::WHILE, "while", line) {
        this->condition = condition;
        this->body = body;
    }

};

#endif //I2C2_TOKENTYPES_H
