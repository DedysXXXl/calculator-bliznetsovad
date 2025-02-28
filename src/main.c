#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h> // Добавлено для fabs()

#define MAX_INPUT_SIZE 1024

struct Stack {
    int top;
    int capacity;
    void* array;
};

void initStack(struct Stack* stack, int size, bool floatMode) {
    stack->top = -1;
    stack->capacity = size;
    if (floatMode) {
        stack->array = malloc(stack->capacity * sizeof(double));
    } else {
        stack->array = malloc(stack->capacity * sizeof(int));
    }
}

bool isEmpty(struct Stack* stack) {
    return stack->top == -1;
}

void push(struct Stack* stack, void* value, bool floatMode) {
    if (stack->top >= stack->capacity - 1) {
        fprintf(stderr, "Ошибка: стек переполнен\n");
        exit(EXIT_FAILURE);
    }
    if (floatMode) {
        ((double*)(stack->array))[++stack->top] = *(double*)value;
    } else {
        ((int*)(stack->array))[++stack->top] = *(int*)value;
    }
}

void* pop(struct Stack* stack, bool floatMode) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Ошибка: стек пуст\n");
        exit(EXIT_FAILURE);
    }
    if (floatMode) {
        return &((double*)(stack->array))[stack->top--];
    } else {
        return &((int*)(stack->array))[stack->top--];
    }
}

void* peek(struct Stack* stack, bool floatMode) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Ошибка: стек пуст\n");
        exit(EXIT_FAILURE);
    }
    if (floatMode) {
        return &((double*)(stack->array))[stack->top];
    } else {
        return &((int*)(stack->array))[stack->top];
    }
}

int precedence(char op) {
    switch (op) {
        case '(': return 0;
        case ')': return 1;
        case '+':
        case '-': return 2;
        case '*':
        case '/': return 3;
        case '~': return 4;
        default: return -1;
    }
}

bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '~';
}

char* infixToPostfix(char* infix, bool floatMode) {
    struct Stack operatorStack;
    initStack(&operatorStack, MAX_INPUT_SIZE, false);
    int i = 0, k = 0;
    char* postfix = (char*)malloc(strlen(infix) * 2 + 1);
    postfix[0] = '\0';
    
    bool prevWasOperator = true;
    
    while (infix[i] != '\0') {
        if (isspace(infix[i])) {
            i++;
            continue;
        }
        
        if (infix[i] == '-' && (prevWasOperator || i == 0 || infix[i-1] == '(')) {
            if (infix[i+1] == '(') {
                int op = '~';
                push(&operatorStack, &op, false);
                i++;
            } else {
                postfix[k++] = '-';
                i++;
            }
            while (isspace(infix[i])) i++;
            continue;
        }
        
        if (isdigit(infix[i]) || (floatMode && infix[i] == '.')) {
            while (isdigit(infix[i]) || (floatMode && infix[i] == '.')) {
                postfix[k++] = infix[i++];
            }
            postfix[k++] = ' ';
            prevWasOperator = false;
            continue;
        }
        else if (infix[i] == '(') {
            int op = infix[i];
            push(&operatorStack, &op, false);
            prevWasOperator = true;
            i++;
        }
        else if (infix[i] == ')') {
            while (!isEmpty(&operatorStack) && (*(int*)peek(&operatorStack, false)) != '(') {
                int op = *(int*)pop(&operatorStack, false);
                postfix[k++] = (char)op;
                postfix[k++] = ' ';
            }
            if (isEmpty(&operatorStack)) {
                fprintf(stderr, "Ошибка: неверная расстановка скобок\n");
                free(operatorStack.array);
                free(postfix);
                return NULL;
            }
            pop(&operatorStack, false);
            if (!isEmpty(&operatorStack) && (*(int*)peek(&operatorStack, false)) == '~') {
                int op = *(int*)pop(&operatorStack, false);
                postfix[k++] = (char)op;
                postfix[k++] = ' ';
            }
            prevWasOperator = false;
            i++;
        }
        else if (is_operator(infix[i])) {
            // Проверка на последовательные операторы
            if (prevWasOperator && infix[i] != '-') {
                fprintf(stderr, "Ошибка: Последовательные операторы\n");
                free(operatorStack.array);
                free(postfix);
                return NULL;
            }
            while (!isEmpty(&operatorStack) &&
                   precedence(*(int*)peek(&operatorStack, false)) >= precedence(infix[i])) {
                int op = *(int*)pop(&operatorStack, false);
                postfix[k++] = (char)op;
                postfix[k++] = ' ';
            }
            int op = infix[i];
            push(&operatorStack, &op, false);
            prevWasOperator = true;
            i++;
        }
        else {
            i++;
        }
    }
    
    while (!isEmpty(&operatorStack)) {
        int op = *(int*)pop(&operatorStack, false);
        if (op == '(') {
            fprintf(stderr, "Ошибка: неверная расстановка скобок\n");
            free(operatorStack.array);
            free(postfix);
            return NULL;
        }
        postfix[k++] = (char)op;
        postfix[k++] = ' ';
    }
    if (k > 0 && postfix[k-1] == ' ') k--;
    postfix[k] = '\0';
    
    free(operatorStack.array);
    return postfix;
}

void* evaluatePostfix(char* postfix, bool floatMode) {
    if (postfix == NULL) {
        return NULL;
    }
    
    struct Stack operandStack;
    initStack(&operandStack, MAX_INPUT_SIZE, floatMode);
    int i = 0;
    
    while (postfix[i] != '\0') {
        if (isspace(postfix[i])) {
            i++;
            continue;
        }
        
        if (isdigit(postfix[i]) || (postfix[i] == '-' && (postfix[i+1] && (isdigit(postfix[i+1]) || (floatMode && postfix[i+1] == '.')))) || (floatMode && postfix[i] == '.')) {
            if (floatMode) {
                double num = strtod(&postfix[i], NULL);
                push(&operandStack, &num, true);
            } else {
                int num = atoi(&postfix[i]);
                push(&operandStack, &num, false);
            }
            while (postfix[i] && !isspace(postfix[i])) i++;
        }
        else if (postfix[i] == '~') {
            void* a_ptr = pop(&operandStack, floatMode);
            if (!a_ptr) {
                fprintf(stderr, "Ошибка: недостаточно операндов для унарного минуса\n");
                free(operandStack.array);
                return NULL;
            }
            if (floatMode) {
                double a = *(double*)a_ptr;
                double res = -a;
                push(&operandStack, &res, true);
            } else {
                int a = *(int*)a_ptr;
                int res = -a;
                push(&operandStack, &res, false);
            }
            i++;
        }
        else {
            void* b_ptr = pop(&operandStack, floatMode);
            void* a_ptr = pop(&operandStack, floatMode);
            if (!b_ptr || !a_ptr) {
                fprintf(stderr, "Ошибка: стек пуст при вычислении\n");
                free(operandStack.array);
                return NULL;
            }
            if (floatMode) {
                double a = *(double*)a_ptr;
                double b = *(double*)b_ptr;
                double res;
                switch (postfix[i]) {
                    case '+': res = a + b; break;
                    case '-': res = a - b; break;
                    case '*': res = a * b; break;
                    case '/':
                        if (b == 0.0) {
                            fprintf(stderr, "Ошибка: Деление на ноль\n");
                            free(operandStack.array);
                            return NULL;
                        }
                        if (fabs(b) < 0.0001) { // Проверка на слишком маленький делитель
                            fprintf(stderr, "Ошибка: Делитель слишком мал\n");
                            free(operandStack.array);
                            return NULL;
                        }
                        res = a / b;
                        break;
                    default:
                        fprintf(stderr, "Ошибка: неизвестный оператор\n");
                        free(operandStack.array);
                        return NULL;
                }
                push(&operandStack, &res, true);
            } else {
                int a = *(int*)a_ptr;
                int b = *(int*)b_ptr;
                int res;
                switch (postfix[i]) {
                    case '+': res = a + b; break;
                    case '-': res = a - b; break;
                    case '*': res = a * b; break;
                    case '/':
                        if (b == 0) {
                            fprintf(stderr, "Ошибка: Деление на ноль\n");
                            free(operandStack.array);
                            return NULL;
                        }
                        res = a / b;
                        break;
                    default:
                        fprintf(stderr, "Ошибка: неизвестный оператор\n");
                        free(operandStack.array);
                        return NULL;
                }
                push(&operandStack, &res, false);
            }
            i++;
        }
    }
    
    if (isEmpty(&operandStack)) {
        fprintf(stderr, "Ошибка: пустое выражение\n");
        free(operandStack.array);
        return NULL;
    }
    
    void* result = malloc(floatMode ? sizeof(double) : sizeof(int));
    void* res_ptr = pop(&operandStack, floatMode);
    if (floatMode) {
        *(double*)result = *(double*)res_ptr;
    } else {
        *(int*)result = *(int*)res_ptr;
    }
    
    if (!isEmpty(&operandStack)) {
        fprintf(stderr, "Ошибка: избыточные операнды\n");
        free(result);
        free(operandStack.array);
        return NULL;
    }
    
    free(operandStack.array);
    return result;
}

#ifndef UNIT_TEST
int main(int argc, char** argv) {
    bool floatMode = false;
    if (argc > 1 && strcmp(argv[1], "--float") == 0) {
        floatMode = true;
    }
    
    char expr[MAX_INPUT_SIZE];
    if (fgets(expr, MAX_INPUT_SIZE, stdin) == NULL) {
        fprintf(stderr, "Ошибка: не удалось прочитать ввод\n");
        return 1;
    }
    
    size_t len = strlen(expr);
    if (len > 0 && expr[len - 1] == '\n') {
        expr[len - 1] = '\0';
    }
    
    if (strlen(expr) == 0) {
        return 0;
    }
    
    char* postfix = infixToPostfix(expr, floatMode);
    if (postfix == NULL) {
        return 1;
    }
    
    void* result = evaluatePostfix(postfix, floatMode);
    if (result == NULL) {
        free(postfix);
        return 1;
    }
    
    if (floatMode) {
        printf("%.4f\n", *(double*)result);
    } else {
        printf("%d\n", *(int*)result);
    }
    
    free(postfix);
    free(result);
    return 0;
}
#endif
