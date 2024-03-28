//
// Created by Ethan Horowitz on 3/13/24.
//

#include "parserTests.h"

void run_parser_tests(){
    RUN_TEST_GROUP("parser");
}

TEST(parser, toRefs){
    std::vector<Token> tokens = tokenize
            ("int a = 4;\n"
             "int main(){\n"
             "    int x = 0;\n"
             "    int y = 5;\n"
             "    int z = x + y;\n"
             "    a += z;\n"
             "\n"
             "    return 0;\n"
             "}\n");
    std::vector<Token*> refs = toTokenRefs(tokens);
    ASSERT_EQ(refs.size(), tokens.size(), %d)
    for (int i = 0; i < refs.size(); i++){
        ASSERT_EQ(refs[i]->type, tokens[i].type, %d)
        ASSERT_EQ(refs[i]->lexeme.c_str(), tokens[i].lexeme.c_str(), %s)
    }
}

TEST(parser, simple_expression){
    std::vector<Token> token_list = tokenize("1 + 2");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token*> parsed = parse(iterator, &scope);
    ASSERT_EQ((int)parsed.size(), 1, %d)
    Token* start = parsed[0];
    ASSERT_EQ(start->type, TYPE_OPERATOR, %d)
    auto* op = (BinaryOpToken*)start;
    EXPECT_EQ(op->val_type, ADD, %d)

    Token* left = op->left;
    EXPECT_EQ(left->type, TYPE_VALUE, %d)
    EXPECT_EQ(left->val_type, NUMBER, %d)
    EXPECT_TRUE(left->lexeme == "1")

    Token* right = op->right;
    EXPECT_EQ(right->type, TYPE_VALUE, %d)
    EXPECT_EQ(right->val_type, NUMBER, %d)
    EXPECT_TRUE(right->lexeme == "2")
}

TEST(parser, complicated_expression){
    std::vector<Token> token_list = tokenize("1 + 4 * 2 / 9 & 5 - 8");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token*> parsed = parse(iterator, &scope);
    ASSERT_EQ((int)parsed.size(), 1, %d)
    Token* start = parsed[0];

    ASSERT_EQ(start->type, TYPE_OPERATOR, %d)
    auto* op = (BinaryOpToken*)start;
    ASSERT_EQ(op->val_type, BIN_AND, %d)
    auto* left = (BinaryOpToken*)op->left;
    ASSERT_EQ(left->val_type, ADD, %d)
    auto* right = (BinaryOpToken*)op->right;
    ASSERT_EQ(right->val_type, MINUS, %d)

    auto* right_left = right->left;
    ASSERT_EQ(right_left->val_type, NUMBER, %d)
    EXPECT_TRUE(right_left->lexeme == "5")

    auto* right_right = right->right;
    ASSERT_EQ(right_right->val_type, NUMBER, %d)
    EXPECT_TRUE(right_right->lexeme == "8")

    auto* left_left = left->left;
    EXPECT_EQ(left_left->val_type, NUMBER, %d)
    EXPECT_TRUE(left_left->lexeme == "1")

    auto* left_right = (BinaryOpToken*)left->right;
    ASSERT_EQ(left_right->val_type, DIV, %d)

    auto* left_right_left = (BinaryOpToken*) left_right->left;
    ASSERT_EQ(left_right_left->val_type, MULT, %d)

    auto* left_right_left_left = left_right_left->left;
    ASSERT_EQ(left_right_left_left->val_type, NUMBER, %d)
    EXPECT_TRUE(left_right_left_left->lexeme == "4")

    auto* left_right_left_right = left_right_left->right;
    ASSERT_EQ(left_right_left_right->val_type, NUMBER, %d)
    EXPECT_TRUE(left_right_left_right->lexeme == "2")

    auto* left_right_right = left_right->right;
    ASSERT_EQ(left_right_right->val_type, NUMBER, %d)
    EXPECT_TRUE(left_right_right->lexeme == "9")
}

TEST(parser, invalid_expression_op_missing_right){
    std::vector<Token> token_list = tokenize("1 + 4 * 2 / 9 & 5 -");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    try {
        std::vector<Token*> parsed = parse(iterator, &scope);
    } catch (std::runtime_error& e){
        EXPECT_TRUE(strcmp(e.what(), "Operator <OPERATOR MINUS - line 1> expected value on right") == 0)
    }
}

TEST(parser, invalid_expression_op_missing_right_2){
    std::vector<Token> token_list = tokenize("1 + 4 * 2 - / 9 & 5 -");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    try {
        std::vector<Token*> parsed = parse(iterator, &scope);
    } catch (std::runtime_error& e){
        EXPECT_TRUE(strcmp(e.what(), "Operator <OPERATOR MINUS - line 1> expected value on right") == 0)
    }
}

TEST(parser, invalid_expression_op_missing_right_3){
    std::vector<Token> token_list = tokenize("1 + 4 * * 2 - / 9 & 5 -");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    try {
        std::vector<Token*> parsed = parse(iterator, &scope);
    } catch (std::runtime_error& e){
        std::string result = e.what();
        EXPECT_EQ_SPECIAL(result, "Must use variable after ref or deref on line 1", %s, .c_str(),)
    }
}

TEST(parser, invalid_expression_op_missing_left){
    std::vector<Token> token_list = tokenize("+ 4 * 2 / 9 & 5 -");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    try {
        std::vector<Token*> parsed = parse(iterator, &scope);
    } catch (std::runtime_error& e){
        std::string result = e.what();
        EXPECT_EQ_SPECIAL(result, "Unexpected token <OPERATOR ADD + line 1>", %s, .c_str(),)
    }
}

TEST(parser, expression_with_parentheses){
std::vector<Token> token_list = tokenize("(1 + 4) * 2 / (9 & 5) - 8");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token*> parsed = parse(iterator, &scope);
    ASSERT_EQ((int)parsed.size(), 1, %d)
    Token* start = parsed[0];

    ASSERT_EQ(start->type, TYPE_OPERATOR, %d)
    auto* sub = (BinaryOpToken*)start;
    ASSERT_EQ(sub->val_type, MINUS, %d)
    // SUB RIGHT
    auto* eight = (BinaryOpToken*)sub->right;
    ASSERT_EQ(eight->val_type, NUMBER, %d)
    EXPECT_TRUE(eight->lexeme == "8")
    // SUB LEFT
    auto* div = (BinaryOpToken*)sub->left;
    ASSERT_EQ(div->val_type, DIV, %d)
    // DIV RIGHT
    auto* andOp = (BinaryOpToken*)div->right;
    ASSERT_EQ(andOp->val_type, BIN_AND, %d)
    // AND RIGHT
    auto* five = andOp->right;
    ASSERT_EQ(five->val_type, NUMBER, %d)
    EXPECT_TRUE(five->lexeme == "5")
    // AND LEFT
    auto* nine = andOp->left;
    ASSERT_EQ(nine->val_type, NUMBER, %d)
    EXPECT_TRUE(nine->lexeme == "9")
    // DIV LEFT
    auto* mult = (BinaryOpToken*)div->left;
    ASSERT_EQ(mult->val_type, MULT, %d)
    // MULT RIGHT
    auto* two = mult->right;
    ASSERT_EQ(two->val_type, NUMBER, %d)
    EXPECT_TRUE(two->lexeme == "2")
    // MULT LEFT
    auto* add = (BinaryOpToken*)mult->left;
    ASSERT_EQ(add->val_type, ADD, %d)
    // ADD RIGHT
    auto* four = add->right;
    ASSERT_EQ(four->val_type, NUMBER, %d)
    EXPECT_TRUE(four->lexeme == "4")
    // ADD LEFT
    auto* one = add->left;
    ASSERT_EQ(one->val_type, NUMBER, %d)
    EXPECT_TRUE(one->lexeme == "1")
}

TEST(parser, nested_parentheses){
    std::vector<Token> token_list = tokenize("((1 + 4) * 2) / 2");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token*> parsed = parse(iterator, &scope);
    ASSERT_EQ((int)parsed.size(), 1, %d)
    Token* start = parsed[0];

    ASSERT_EQ(start->type, TYPE_OPERATOR, %d)
    auto* div = (BinaryOpToken*)start;
    ASSERT_EQ(div->val_type, DIV, %d)
    // DIV RIGHT
    auto* two = div->right;
    ASSERT_EQ(two->val_type, NUMBER, %d)
    EXPECT_TRUE(two->lexeme == "2")
    // DIV LEFT
    auto* mult = (BinaryOpToken*)div->left;
    ASSERT_EQ(mult->val_type, MULT, %d)
    // MULT RIGHT
    auto* twoInner = mult->right;
    ASSERT_EQ(twoInner->val_type, NUMBER, %d)
    EXPECT_TRUE(twoInner->lexeme == "2")
    // MULT LEFT
    auto* add = (BinaryOpToken*)mult->left;
    ASSERT_EQ(add->val_type, ADD, %d)
    // ADD RIGHT
    auto* four = add->right;
    ASSERT_EQ(four->val_type, NUMBER, %d)
    EXPECT_TRUE(four->lexeme == "4")
    // ADD LEFT
    auto* one = add->left;
    ASSERT_EQ(one->val_type, NUMBER, %d)
    EXPECT_TRUE(one->lexeme == "1")
}

TEST(parser, simple_assignment){
    std::vector<Token> token_list = tokenize("int a = 4;");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token*> parsed = parse(iterator, &scope);
    ASSERT_EQ((int)parsed.size(), 1, %d)
    Token* start = parsed[0];

    ASSERT_EQ(start->type, TYPE_OPERATOR, %d)
    auto* assign = (DefinitionToken*)start;
    ASSERT_EQ(assign->val_type, IDENTIFIER, %d)

    EXPECT_EQ(assign->valueType, INT, %d)

    EXPECT_EQ(assign->value->val_type, NUMBER, %d)
    EXPECT_TRUE(assign->value->lexeme == "4")

    EXPECT_EQ(assign->refCount, 0, %d)

    EXPECT_TRUE(assign->name == "a")
}

TEST(parser, assignment_with_ref){
    std::vector<Token> token_list = tokenize("int a = 4;\n"
                                             "int* b = &a;");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token*> parsed = parse(iterator, &scope);
    ASSERT_EQ((int)parsed.size(), 2, %d)
    Token* start = parsed[1];

    ASSERT_EQ(start->type, TYPE_OPERATOR, %d)
    auto* assign = (DefinitionToken*)start;
    ASSERT_EQ(assign->val_type, IDENTIFIER, %d)

    EXPECT_EQ(assign->valueType, INT, %d)
    EXPECT_EQ(assign->refCount, 1, %d)

    EXPECT_EQ(assign->value->val_type, REF, %d)
    EXPECT_TRUE(assign->name == "b")

    auto* ref = (BinaryOpToken*)assign->value;
    EXPECT_EQ(ref->right->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(ref->right->lexeme == "a")
}

TEST(parser, invalid_assignment_no_expr){
    std::vector<Token> token_list = tokenize("int a =");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    try {
        std::vector<Token*> parsed = parse(iterator, &scope);
    } catch (std::runtime_error& e){
        std::string result = e.what();
        EXPECT_EQ_SPECIAL(result, "Operator <OPERATOR NONE  line 0> expected value on right", %s, .c_str(),)
    }
}

TEST(parser, invalid_assignment_bad_ops){
    std::vector<Token> token_list = tokenize("int a = = 4;");
    std::vector<Token*> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    try {
        std::vector<Token*> parsed = parse(iterator, &scope);
    } catch (std::runtime_error& e) {
        std::string result = e.what();
        EXPECT_EQ_SPECIAL(result, "Operator <OPERATOR NONE  line 0> expected value on right", %s, .c_str(),)
    }
}

TEST(parser, if_statement){
    std::vector<Token> token_list = tokenize("int x = 1; if(x < 5){\n"
                                             "    x = 5;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 2, %d)

    auto* if_statement = (IfElseToken*) parsed[1];

    Token* condition = if_statement->condition;
    ASSERT_EQ(condition->type, TYPE_OPERATOR, %d)
    auto* lessThan = (BinaryOpToken*)condition;
    ASSERT_EQ(lessThan->val_type, LT, %d)
    ASSERT_EQ(lessThan->left->val_type, IDENTIFIER, %d)
    ASSERT_EQ(lessThan->right->val_type, NUMBER, %d)

    GroupToken* ifBody = if_statement->ifBody;
    ASSERT_EQ(ifBody->expressions.size(), 1, %d)
    ASSERT_EQ(ifBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq = (BinaryOpToken*)ifBody->expressions[0];
    ASSERT_EQ(eq->val_type, EQ, %d)
    ASSERT_EQ(eq->left->val_type, IDENTIFIER, %d)
    ASSERT_TRUE(eq->left->lexeme == "x")
    ASSERT_EQ(eq->right->val_type, NUMBER, %d)

    EXPECT_TRUE(if_statement->elseIfBodies.empty())
    EXPECT_EQ(if_statement->elseBody, nullptr, %p)
}

TEST(parser, if_else_statement){
    std::vector<Token> token_list = tokenize("int x = 1; if(x < 5){\n"
                                             "    x = 5;\n"
                                             "} else {\n"
                                             "    x = 6;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 2, %d)

    auto* if_statement = (IfElseToken*) parsed[1];

    Token* condition = if_statement->condition;
    ASSERT_EQ(condition->type, TYPE_OPERATOR, %d)
    auto* lessThan = (BinaryOpToken*)condition;
    ASSERT_EQ(lessThan->val_type, LT, %d)
    ASSERT_EQ(lessThan->left->val_type, IDENTIFIER, %d)
    ASSERT_EQ(lessThan->right->val_type, NUMBER, %d)

    GroupToken* ifBody = if_statement->ifBody;
    ASSERT_EQ(ifBody->expressions.size(), 1, %d)
    ASSERT_EQ(ifBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq = (BinaryOpToken*)ifBody->expressions[0];
    ASSERT_EQ(eq->val_type, EQ, %d)
    EXPECT_EQ(eq->left->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(eq->left->lexeme == "x")
    EXPECT_EQ(eq->right->val_type, NUMBER, %d)
    EXPECT_TRUE(eq->right->lexeme == "5")

    GroupToken* elseBody = if_statement->elseBody;
    ASSERT_EQ(elseBody->expressions.size(), 1, %d)
    ASSERT_EQ(elseBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq2 = (BinaryOpToken*)elseBody->expressions[0];
    ASSERT_EQ(eq2->val_type, EQ, %d)
    EXPECT_EQ(eq2->left->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(eq2->left->lexeme == "x")
    EXPECT_EQ(eq2->right->val_type, NUMBER, %d)
    EXPECT_TRUE(eq2->right->lexeme == "6")
}

TEST(parser, if_else_if_statement){
    std::vector<Token> token_list = tokenize("int x = 1; if(x < 5){\n"
                                             "    x = 5;\n"
                                             "} else if(x < 6){\n"
                                             "    x = 6;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 2, %d)

    auto* if_statement = (IfElseToken*) parsed[1];

    Token* condition = if_statement->condition;
    ASSERT_EQ(condition->type, TYPE_OPERATOR, %d)
    auto* lessThan = (BinaryOpToken*)condition;
    ASSERT_EQ(lessThan->val_type, LT, %d)
    ASSERT_EQ(lessThan->left->val_type, IDENTIFIER, %d)
    ASSERT_EQ(lessThan->right->val_type, NUMBER, %d)

    GroupToken* ifBody = if_statement->ifBody;
    ASSERT_EQ(ifBody->expressions.size(), 1, %d)
    ASSERT_EQ(ifBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq = (BinaryOpToken*)ifBody->expressions[0];
    ASSERT_EQ(eq->val_type, EQ, %d)
    EXPECT_EQ(eq->left->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(eq->left->lexeme == "x")
    EXPECT_EQ(eq->right->val_type, NUMBER, %d)
    EXPECT_TRUE(eq->right->lexeme == "5")

    GroupToken* elseIfBody = if_statement->elseIfBodies[0];
    ASSERT_EQ(elseIfBody->expressions.size(), 1, %d)
    ASSERT_EQ(elseIfBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq2 = (BinaryOpToken*)elseIfBody->expressions[0];
    ASSERT_EQ(eq2->val_type, EQ, %d)
    EXPECT_EQ(eq2->left->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(eq2->left->lexeme == "x")
    EXPECT_EQ(eq2->right->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(eq2->right->lexeme,  "6", %s, .c_str(),)

    EXPECT_EQ(if_statement->elseBody, nullptr, %p)
}

TEST(parser, if_else_if_else_statement){
    std::vector<Token> token_list = tokenize("int x = 1; if(x < 5){\n"
                                             "    x = 5;\n"
                                             "} else if(x < 6){\n"
                                             "    x = 6;\n"
                                             "} else {\n"
                                             "    x = 7;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 2, %d)

    auto* if_statement = (IfElseToken*) parsed[1];

    Token* condition = if_statement->condition;
    ASSERT_EQ(condition->type, TYPE_OPERATOR, %d)
    auto* lessThan = (BinaryOpToken*)condition;
    ASSERT_EQ(lessThan->val_type, LT, %d)
    ASSERT_EQ(lessThan->left->val_type, IDENTIFIER, %d)
    ASSERT_EQ(lessThan->right->val_type, NUMBER, %d)

    GroupToken* ifBody = if_statement->ifBody;
    ASSERT_EQ(ifBody->expressions.size(), 1, %d)
    ASSERT_EQ(ifBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq = (BinaryOpToken*)ifBody->expressions[0];
    ASSERT_EQ(eq->val_type, EQ, %d)
    EXPECT_EQ(eq->left->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(eq->left->lexeme == "x")
    EXPECT_EQ(eq->right->val_type, NUMBER, %d)
    EXPECT_TRUE(eq->right->lexeme == "5")

    GroupToken* elseIfBody = if_statement->elseIfBodies[0];
    ASSERT_EQ(elseIfBody->expressions.size(), 1, %d)
    ASSERT_EQ(elseIfBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq2 = (BinaryOpToken*)elseIfBody->expressions[0];
    ASSERT_EQ(eq2->val_type, EQ, %d)
    EXPECT_EQ(eq2->left->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(eq2->left->lexeme == "x")
    EXPECT_EQ(eq2->right->val_type, NUMBER, %d)
    EXPECT_TRUE(eq2->right->lexeme == "6")

    GroupToken* elseBody = if_statement->elseBody;
    ASSERT_EQ(elseBody->expressions.size(), 1, %d)
    ASSERT_EQ(elseBody->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* eq3 = (BinaryOpToken*)elseBody->expressions[0];
    ASSERT_EQ(eq3->val_type, EQ, %d)
    EXPECT_EQ(eq3->left->val_type, IDENTIFIER, %d)
    EXPECT_TRUE(eq3->left->lexeme == "x")
    EXPECT_EQ(eq3->right->val_type, NUMBER, %d)
    EXPECT_TRUE(eq3->right->lexeme == "7")
}

TEST(parser, function_definition_with_empty_function){
    std::vector<Token> token_list = tokenize("int main(){\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* function = (FunctionToken*) parsed[0];
    EXPECT_EQ(function->returnType, INT, %d)
    EXPECT_EQ(function->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(function->name, "main", %s, .c_str(),)
    EXPECT_TRUE(function->parameters.empty())
    EXPECT_TRUE(function->body->expressions.empty())
}

TEST(parser, function_definition_with_pointer_empty){
    std::vector<Token> token_list = tokenize("int* main(){\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* function = (FunctionToken*) parsed[0];
    EXPECT_EQ(function->returnType, INT, %d)
    EXPECT_EQ(function->refCount, 1, %d)
    EXPECT_EQ_SPECIAL(function->name, "main", %s, .c_str(),)
    EXPECT_TRUE(function->parameters.empty())
    EXPECT_TRUE(function->body->expressions.empty())
}

TEST(parser, function_definition_with_body){
    std::vector<Token> token_list = tokenize("int main(){\n"
                                             "    int x = 5;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* function = (FunctionToken*) parsed[0];
    EXPECT_EQ(function->returnType, INT, %d)
    EXPECT_EQ(function->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(function->name, "main", %s, .c_str(),)
    EXPECT_TRUE(function->parameters.empty())
    EXPECT_EQ(function->body->expressions.size(), 1, %d)
    EXPECT_EQ(function->body->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* def = (DefinitionToken*)function->body->expressions[0];
    EXPECT_EQ(def->valueType, INT, %d)
    EXPECT_EQ(def->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(def->name, "x", %s, .c_str(),)
    EXPECT_EQ(def->value->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(def->value->lexeme, "5", %s, .c_str(),)
}

TEST(parser, function_call){
    std::vector<Token> token_list = tokenize("int add(int x, int y){\n"
                                             "    return x + y;\n"
                                             "}\n"
                                             "int z = add(2, 3);\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 2, %d)
    auto* expr = (DefinitionToken*)parsed[1];
    EXPECT_EQ(expr->valueType, INT, %d)
    EXPECT_EQ(expr->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(expr->name, "z", %s, .c_str(),)
    auto* call = (FunctionCallToken*)expr->value;
    ASSERT_EQ(call->type, TokenType::TYPE_OPERATOR, %d)
    ASSERT_EQ(call->val_type, TokenValue::FUNCTION, %d)
    EXPECT_EQ(call->function->val_type, TokenValue::IDENTIFIER, %d)
    EXPECT_EQ_SPECIAL(call->function->lexeme, "add", %s, .c_str(),)
    ASSERT_EQ(call->arguments.size(), 2, %d)
    EXPECT_EQ(call->arguments[0]->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(call->arguments[0]->lexeme, "2", %s, .c_str(),)
    EXPECT_EQ(call->arguments[1]->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(call->arguments[1]->lexeme, "3", %s, .c_str(),)
}

TEST(parser, function_header){
    std::vector<Token> token_list = tokenize("int thing(int x, int y);");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* function = (FunctionToken*) parsed[0];
    EXPECT_EQ(function->returnType, INT, %d)
    EXPECT_EQ(function->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(function->name, "thing", %s, .c_str(),)
    ASSERT_EQ(function->parameters.size(), 2, %d)
    auto* x = function->parameters[0];
    EXPECT_EQ(x->valueType, INT, %d)
    EXPECT_EQ(x->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(x->name, "x", %s, .c_str(),)
    auto* y = function->parameters[1];
    EXPECT_EQ(y->valueType, INT, %d)
    EXPECT_EQ(y->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(y->name, "y", %s, .c_str(),)
    EXPECT_TRUE(function->body == nullptr)
}

TEST(parser, function_impl_following_function_header){
    std::vector<Token> token_list = tokenize("int thing(int x, int y);\n"
                                             "int thing(int x, int y){\n"
                                             "    return x + y;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 2, %d)

    auto* function = (FunctionToken*) parsed[0];
    EXPECT_EQ(function->returnType, INT, %d)
    EXPECT_EQ(function->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(function->name, "thing", %s, .c_str(),)
    ASSERT_EQ(function->parameters.size(), 2, %d)
    auto* x = function->parameters[0];
    EXPECT_EQ(x->valueType, INT, %d)
    EXPECT_EQ(x->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(x->name, "x", %s, .c_str(),)
    auto* y = function->parameters[1];
    EXPECT_EQ(y->valueType, INT, %d)
    EXPECT_EQ(y->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(y->name, "y", %s, .c_str(),)
    EXPECT_EQ(function->body->expressions.size(), 1, %d)
    EXPECT_EQ(function->body->expressions[0]->type, TYPE_KEYWORD, %d)

    auto* function2 = (FunctionToken*) parsed[1];
    EXPECT_EQ(function2->returnType, INT, %d)
    EXPECT_EQ(function2->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(function2->name, "thing", %s, .c_str(),)
    ASSERT_EQ(function2->parameters.size(), 2, %d)
    auto* x2 = function2->parameters[0];
    EXPECT_EQ(x2->valueType, INT, %d)
    EXPECT_EQ(x2->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(x2->name, "x", %s, .c_str(),)
    auto* y2 = function2->parameters[1];
    EXPECT_EQ(y2->valueType, INT, %d)
    EXPECT_EQ(y2->refCount, 0, %d)
}

TEST(parser, function_with_return){
    std::vector<Token> token_list = tokenize("int main(){\n"
                                             "    return 5;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* function = (FunctionToken*) parsed[0];
    EXPECT_EQ(function->returnType, INT, %d)
    EXPECT_EQ(function->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(function->name, "main", %s, .c_str(),)
    EXPECT_TRUE(function->parameters.empty())
    EXPECT_EQ(function->body->expressions.size(), 1, %d)
    EXPECT_EQ(function->body->expressions[0]->type, TYPE_KEYWORD, %d)

    auto* ret = (ReturnToken*)function->body->expressions[0];
    EXPECT_EQ(ret->value->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(ret->value->lexeme, "5", %s, .c_str(),)
}

TEST(parser, for_loop_with_empty_body){
    std::vector<Token> token_list = tokenize("for(int i = 0; i <= 5; i += 1){\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* forLoop = (ForToken*) parsed[0];
    ASSERT_EQ(forLoop->init->val_type, IDENTIFIER, %d)
    auto* def = (DefinitionToken*)forLoop->init;
    EXPECT_EQ(def->valueType, INT, %d)
    EXPECT_EQ(def->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(def->name, "i", %s, .c_str(),)
    EXPECT_EQ(def->value->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(def->value->lexeme, "0", %s, .c_str(),)

    auto* condition = (BinaryOpToken*)forLoop->condition;
    EXPECT_EQ(condition->val_type, LTE, %d)
    EXPECT_EQ(condition->left->val_type, IDENTIFIER, %d)
    EXPECT_EQ_SPECIAL(condition->left->lexeme, "i", %s, .c_str(),)
    EXPECT_EQ(condition->right->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(condition->right->lexeme, "5", %s, .c_str(),)

    auto* increment = (BinaryOpToken*)forLoop->increment;
    EXPECT_EQ(increment->val_type, ADD_EQ, %d)
    EXPECT_EQ(increment->left->val_type, IDENTIFIER, %d)
    EXPECT_EQ_SPECIAL(increment->left->lexeme, "i", %s, .c_str(),)
    EXPECT_EQ(increment->right->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(increment->right->lexeme, "1", %s, .c_str(),)

    EXPECT_TRUE(forLoop->body->expressions.empty())
}

TEST(parser, for_loop_with_simple_body){
    std::vector<Token> token_list = tokenize("for(int i = 0; i <= 5; i += 1){\n"
                                             "    int x = 5;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* forLoop = (ForToken*) parsed[0];
    ASSERT_EQ(forLoop->init->val_type, IDENTIFIER, %d)
    auto* def = (DefinitionToken*)forLoop->init;
    EXPECT_EQ(def->valueType, INT, %d)
    EXPECT_EQ(def->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(def->name, "i", %s, .c_str(),)
    EXPECT_EQ(def->value->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(def->value->lexeme, "0", %s, .c_str(),)

    auto* condition = (BinaryOpToken*)forLoop->condition;
    EXPECT_EQ(condition->val_type, LTE, %d)
    EXPECT_EQ(condition->left->val_type, IDENTIFIER, %d)
    EXPECT_EQ_SPECIAL(condition->left->lexeme, "i", %s, .c_str(),)
    EXPECT_EQ(condition->right->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(condition->right->lexeme, "5", %s, .c_str(),)

    auto* increment = (BinaryOpToken*)forLoop->increment;
    EXPECT_EQ(increment->val_type, ADD_EQ, %d)
    EXPECT_EQ(increment->left->val_type, IDENTIFIER, %d)
    EXPECT_EQ_SPECIAL(increment->left->lexeme, "i", %s, .c_str(),)
    EXPECT_EQ(increment->right->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(increment->right->lexeme, "1", %s, .c_str(),)

    EXPECT_EQ(forLoop->body->expressions.size(), 1, %d)
    EXPECT_EQ(forLoop->body->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* def2 = (DefinitionToken*)forLoop->body->expressions[0];
    EXPECT_EQ(def2->valueType, INT, %d)
    EXPECT_EQ(def2->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(def2->name, "x", %s, .c_str(),)
    EXPECT_EQ(def2->value->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(def2->value->lexeme, "5", %s, .c_str(),)
}

TEST(parser, while_loop_with_empty_body){
    std::vector<Token> token_list = tokenize("while(1){\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* whileLoop = (WhileToken*) parsed[0];
    ASSERT_EQ(whileLoop->condition->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(whileLoop->condition->lexeme, "1", %s, .c_str(),)
    EXPECT_TRUE(whileLoop->body->expressions.empty())
}

TEST(parser, while_loop_with_expr_in_header_empty_body){
    std::vector<Token> token_list = tokenize("int x = 0; while(x < 5){\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 2, %d)

    auto* whileLoop = (WhileToken*) parsed[1];
    ASSERT_EQ(whileLoop->condition->val_type, LT, %d)
    auto* lt = (BinaryOpToken*)whileLoop->condition;
    EXPECT_EQ(lt->left->val_type, IDENTIFIER, %d)
    EXPECT_EQ_SPECIAL(lt->left->lexeme, "x", %s, .c_str(),)
    EXPECT_EQ(lt->right->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(lt->right->lexeme, "5", %s, .c_str(),)
    EXPECT_TRUE(whileLoop->body->expressions.empty())
}

TEST(parser, while_loop_with_body){
    std::vector<Token> token_list = tokenize("while(1){\n"
                                             "    int x = 5;\n"
                                             "}\n");
    std::vector<Token *> tokens = toTokenRefs(token_list);
    TokenIterator iterator(tokens);
    Scope scope = Scope(nullptr);
    std::vector<Token *> parsed = parse(iterator, &scope);
    ASSERT_EQ((int) parsed.size(), 1, %d)

    auto* whileLoop = (WhileToken*) parsed[0];
    ASSERT_EQ(whileLoop->condition->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(whileLoop->condition->lexeme, "1", %s, .c_str(),)
    EXPECT_EQ(whileLoop->body->expressions.size(), 1, %d)
    EXPECT_EQ(whileLoop->body->expressions[0]->type, TYPE_OPERATOR, %d)
    auto* def = (DefinitionToken*)whileLoop->body->expressions[0];
    EXPECT_EQ(def->valueType, INT, %d)
    EXPECT_EQ(def->refCount, 0, %d)
    EXPECT_EQ_SPECIAL(def->name, "x", %s, .c_str(),)
    EXPECT_EQ(def->value->val_type, NUMBER, %d)
    EXPECT_EQ_SPECIAL(def->value->lexeme, "5", %s, .c_str(),)
}

