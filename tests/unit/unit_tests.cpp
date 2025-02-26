#include <gtest/gtest.h>
#include "../src/calc.c"

TEST(StackTests, InitStack) {
    struct Stack stack;
    initStack(&stack, 10, true);

    EXPECT_EQ(stack.top, -1);
    EXPECT_EQ(stack.capacity, 10);
    ASSERT_NE(stack.array, nullptr);

    free(stack.array);
}

TEST(StackTests, IsEmpty) {
    struct Stack stack;
    initStack(&stack, 10, true);

    EXPECT_TRUE(isEmpty(&stack));

    double value = 42.0;
    push(&stack, &value, true);

    EXPECT_FALSE(isEmpty(&stack));

    free(stack.array);
}

TEST(StackTests, PushPopPeek) {
    struct Stack stack;
    initStack(&stack, 10, true);

    double value1 = 12.34;
    double value2 = 56.78;

    push(&stack, &value1, true);
    push(&stack, &value2, true);

    EXPECT_DOUBLE_EQ(*(double*)peek(&stack, true), value2);
    EXPECT_DOUBLE_EQ(*(double*)pop(&stack, true), value2);
    EXPECT_DOUBLE_EQ(*(double*)pop(&stack, true), value1);

    free(stack.array);
}

TEST(InfixToPostfixTests, SimpleExpression) {
    const char* infix = "1 + 2";
    char* postfix = infixToPostfix(infix, false);

    EXPECT_STREQ(postfix, "12+");
    free(postfix);
}

TEST(EvaluatePostfixTests, SimpleAddition) {
    const char* postfix = "12+";
    void* result = evaluatePostfix(postfix, false);

    EXPECT_EQ(*(int*)result, 3);
    free(result);
}

TEST(EvaluatePostfixTests, FloatModeDivision) {
    const char* postfix = "24/";
    void* result = evaluatePostfix(postfix, true);

    EXPECT_DOUBLE_EQ(*(double*)result, 6.0);
    free(result);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
