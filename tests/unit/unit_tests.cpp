#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

extern "C" {
    struct Stack {
        int top;
        int capacity;
        void* array;
    };

    void initStack(struct Stack* stack, int size, bool floatMode);
    bool isEmpty(struct Stack* stack);
    int push(struct Stack* stack, void* value, bool floatMode);
    void* pop(struct Stack* stack, bool floatMode);
    void* peek(struct Stack* stack, bool floatMode);
    int precedence(char op);
    char* infixToPostfix(char* infix, bool floatMode);
    void* evaluatePostfix(char* postfix, bool floatMode);
}

TEST(StackInitTests, InitStack_Int) {
    struct Stack stack;
    initStack(&stack, 10, false);
    EXPECT_EQ(stack.top, -1);
    EXPECT_EQ(stack.capacity, 10);
    EXPECT_NE(stack.array, nullptr);
    free(stack.array);
}

TEST(StackInitTests, InitStack_Double) {
    struct Stack stack;
    initStack(&stack, 10, true);
    EXPECT_EQ(stack.top, -1);
    EXPECT_EQ(stack.capacity, 10);
    EXPECT_NE(stack.array, nullptr);
    free(stack.array);
}

TEST(StackIsEmptyTests, EmptyStack) {
    struct Stack stack;
    stack.top = -1;
    EXPECT_TRUE(isEmpty(&stack));
}

TEST(StackIsEmptyTests, NonEmptyStack) {
    struct Stack stack;
    stack.top = 0;
    EXPECT_FALSE(isEmpty(&stack));
}

TEST(StackPushTests, PushInt) {
    struct Stack stack;
    initStack(&stack, 10, false);
    int value = 42;
    EXPECT_EQ(push(&stack, &value, false), 0);
    EXPECT_EQ(*(int*)(stack.array), 42);
    EXPECT_EQ(stack.top, 0);
    free(stack.array);
}

TEST(StackPushTests, PushDouble) {
    struct Stack stack;
    initStack(&stack, 10, true);
    double value = 3.14;
    EXPECT_EQ(push(&stack, &value, true), 0);
    EXPECT_DOUBLE_EQ(*(double*)(stack.array), 3.14);
    EXPECT_EQ(stack.top, 0);
    free(stack.array);
}

TEST(StackPushTests, PushOverflow) {
    struct Stack stack;
    initStack(&stack, 1, false);
    int value = 123;
    EXPECT_EQ(push(&stack, &value, false), 0);
    EXPECT_EQ(push(&stack, &value, false), -1);
    EXPECT_EQ(stack.top, 0);
    EXPECT_EQ(*(int*)stack.array, 123);
    free(stack.array);
}

TEST(StackPopTests, PopInt) {
    struct Stack stack;
    initStack(&stack, 10, false);
    int value = 100;
    push(&stack, &value, false);
    int* poppedValue = (int*)pop(&stack, false);
    EXPECT_EQ(*poppedValue, 100);
    EXPECT_EQ(stack.top, -1);
    free(stack.array);
}

TEST(StackPopTests, PopDouble) {
    struct Stack stack;
    initStack(&stack, 10, true);
    double value = 12.34;
    push(&stack, &value, true);
    double* poppedValue = (double*)pop(&stack, true);
    EXPECT_DOUBLE_EQ(*poppedValue, 12.34);
    EXPECT_EQ(stack.top, -1);
    free(stack.array);
}

TEST(StackPopTests, PopFromEmptyStack) {
    struct Stack stack;
    initStack(&stack, 10, false);
    void* result = pop(&stack, false);
    EXPECT_EQ(result, nullptr);
    free(stack.array);
}

TEST(StackPeekTests, PeekInt) {
    struct Stack stack;
    initStack(&stack, 10, false);
    int value = 99;
    push(&stack, &value, false);
    int* peekedValue = (int*)peek(&stack, false);
    EXPECT_EQ(*peekedValue, 99);
    EXPECT_EQ(stack.top, 0);
    free(stack.array);
}

TEST(StackPeekTests, PeekDouble) {
    struct Stack stack;
    initStack(&stack, 10, true);
    double value = 9.87;
    push(&stack, &value, true);
    double* peekedValue = (double*)peek(&stack, true);
    EXPECT_DOUBLE_EQ(*peekedValue, 9.87);
    EXPECT_EQ(stack.top, 0);
    free(stack.array);
}

TEST(StackPeekTests, PeekFromEmptyStack) {
    struct Stack stack;
    initStack(&stack, 10, false);
    void* result = peek(&stack, false);
    EXPECT_EQ(result, nullptr);
    free(stack.array);
}

TEST(PrecedenceTests, PrecedenceParentheses) {
    EXPECT_EQ(precedence('('), 0);
    EXPECT_EQ(precedence(')'), 1);
}

TEST(PrecedenceTests, PrecedenceAdditionSubtraction) {
    EXPECT_EQ(precedence('+'), 2);
    EXPECT_EQ(precedence('-'), 2);
}

TEST(PrecedenceTests, PrecedenceMultiplicationDivision) {
    EXPECT_EQ(precedence('*'), 3);
    EXPECT_EQ(precedence('/'), 3);
}

TEST(PrecedenceTests, InvalidOperator) {
    EXPECT_EQ(precedence('#'), -1);
}

TEST(InfixToPostfixTests, SimpleExpression) {
    char infix[] = "2+3";
    bool floatMode = false;
    const char* expectedPostfix = "2 3 + ";
    char* actualPostfix = infixToPostfix(infix, floatMode);
    EXPECT_STREQ(actualPostfix, expectedPostfix);
    free(actualPostfix);
}

TEST(InfixToPostfixTests, ComplexExpression) {
    char infix[] = "(2+3)*4/(8-2)";
    bool floatMode = false;
    const char* expectedPostfix = "2 3 + 4 * 8 2 - / ";
    char* actualPostfix = infixToPostfix(infix, floatMode);
    EXPECT_STREQ(actualPostfix, expectedPostfix);
    free(actualPostfix);
}

TEST(InfixToPostfixTests, FloatModeExpression) {
    char infix[] = "2.5+3.7";
    bool floatMode = true;
    const char* expectedPostfix = "2.5 3.7 + ";
    char* actualPostfix = infixToPostfix(infix, floatMode);
    EXPECT_STREQ(actualPostfix, expectedPostfix);
    free(actualPostfix);
}

TEST(EvaluatePostfixTests, SimpleAddition) {
    char postfix[] = "2 3 + ";
    bool floatMode = false;
    int expectedResult = 5;
    void* actualResult = evaluatePostfix(postfix, floatMode);
    ASSERT_NE(actualResult, nullptr);
    EXPECT_EQ(*(int*)actualResult, expectedResult);
    free(actualResult);
}

TEST(EvaluatePostfixTests, ComplexExpression) {
    char postfix[] = "2 3 + 4 * 8 2 - / ";
    bool floatMode = false;
    int expectedResult = 10; // (2+3)*4/(8-2) = 20/2 = 10
    void* actualResult = evaluatePostfix(postfix, floatMode);
    ASSERT_NE(actualResult, nullptr);
    EXPECT_EQ(*(int*)actualResult, expectedResult);
    free(actualResult);
}

TEST(EvaluatePostfixTests, DivisionByZero) {
    char postfix[] = "2 0 / ";
    bool floatMode = false;
    void* result = evaluatePostfix(postfix, floatMode);
    EXPECT_EQ(result, nullptr);
}

TEST(EvaluatePostfixTests, FloatModeEvaluation) {
    char postfix[] = "2.5 3.7 + ";
    bool floatMode = true;
    double expectedResult = 6.2;
    void* actualResult = evaluatePostfix(postfix, floatMode);
    ASSERT_NE(actualResult, nullptr);
    EXPECT_DOUBLE_EQ(*(double*)actualResult, expectedResult);
    free(actualResult);
}
