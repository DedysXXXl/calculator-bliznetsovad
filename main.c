#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // Для функции isdigit()
#include <string.h> // Для функции strlen()
#include <limits.h> // Для INT_MIN и INT_MAX
#include <stdbool.h> // Для bool

#define MAX_INPUT_SIZE 1024

// Структура стека для чисел
struct Stack {
    int top;
    int capacity;
    void* array;
};

// Инициализация стека
void initStack(struct Stack* stack, int size, bool floatMode) {
    stack->top = -1;
    stack->capacity = size;
    if (floatMode) {
        stack->array = malloc(stack->capacity * sizeof(double));
    } else {
        stack->array = malloc(stack->capacity * sizeof(int));
    }
}

// Проверка пустоты стека
bool isEmpty(struct Stack* stack) {
    return stack->top == -1;
}

// Добавление элемента в стек
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

// Извлечение элемента из стека
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

// Получение верхнего элемента стека
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

// Определение приоритета оператора
int precedence(char op) {
    switch (op) {
        case '(': return 0;
        case ')': return 1;
        case '+':
        case '-': return 2;
        case '*':
        case '/': return 3;
        default : return -1;
    }
}

// Преобразование инфиксной записи в постфиксную
char* infixToPostfix(char* infix, bool floatMode) {
    struct Stack operatorStack;
    initStack(&operatorStack, MAX_INPUT_SIZE, false); // Стек операторов всегда хранит символы
    
    int i = 0, k = 0;
    char* postfix = (char*)malloc(strlen(infix) + 1);
    
    while (infix[i] != '\0') {
        if (isspace(infix[i])) {
            i++;
            continue;
        }
        
        if (isdigit(infix[i]) || (floatMode && infix[i] == '.')) {
            if (floatMode) {
                double num = strtod(&infix[i], NULL);
                push(&operandStack, &num, floatMode);
            } else {
                int num = atoi(&infix[i]);
                push(&operandStack, &num, floatMode);
            }
            while (isdigit(infix[i]) || (floatMode && infix[i] == '.'))
                i++;
        } else if (infix[i] == '(')
            push(&operatorStack, infix[i]);
        else if (infix[i] == ')') {
            while (!isEmpty(&operatorStack) && peek(&operatorStack) != '(')
                postfix[k++] = pop(&operatorStack);
            
            if (isEmpty(&operatorStack))
                fprintf(stderr, "Ошибка: неверная расстановка скобок\n");
            else
                pop(&operatorStack);
        } else {
            while (!isEmpty(&operatorStack) &&
                   precedence(peek(&operatorStack)) >= precedence(infix[i]))
                postfix[k++] = pop(&operatorStack);
            
            push(&operatorStack, infix[i]);
        }
        i++;
    }
    
    while (!isEmpty(&operatorStack))
        postfix[k++] = pop(&operatorStack);
    
    postfix[k] = '\0';
    free(operatorStack.array);
    return postfix;
}

// Вычисление значения постфиксного выражения
void* evaluatePostfix(char* postfix, bool floatMode) {
    struct Stack operandStack;
    initStack(&operandStack, MAX_INPUT_SIZE, floatMode);
    
    int i = 0;
    while (postfix[i] != '\0') {
        if (isspace(postfix[i])) {
            i++;
            continue;
        }
        
        if (isdigit(postfix[i]) || (floatMode && postfix[i] == '.')) {
            if (floatMode) {
                double num = strtod(&postfix[i], NULL);
                push(&operandStack, &num, floatMode);
            } else {
                int num = atoi(&postfix[i]);
                push(&operandStack, &num, floatMode);
            }
            while (isdigit(postfix[i]) || (floatMode && postfix[i] == '.'))
                i++;
        } else {
            void* b = pop(&operandStack, floatMode);
            void* a = pop(&operandStack, floatMode);
            void* res;
            
            switch (postfix[i]) {
                case '+': res = floatMode ? *(double*)a + *(double*)b : *(int*)a + *(int*)b; break;
                case '-': res = floatMode ? *(double*)a - *(double*)b : *(int*)a - *(int*)b; break;
                case '*': res = floatMode ? *(double*)a * *(double*)b : *(int*)a * *(int*)b; break;
                case '/': 
                    if (*(double*)b == 0.0 || *(int*)b == 0) {
                        fprintf(stderr, "Ошибка: деление на ноль\n");
                        exit(EXIT_FAILURE);
                    }
                    res = floatMode ? *(double*)a / *(double*)b : *(int*)a / *(int*)b;
                    break;
                default : fprintf(stderr, "Ошибка: неизвестный оператор\n");
                          exit(EXIT_FAILURE);
            }
            
            push(&operandStack, &res, floatMode);
        }
        i++;
    }
    
    void* result = pop(&operandStack, floatMode);
    free(operandStack.array);
    return result;
}

// Основная функция
int main(int argc, char** argv) {
    bool floatMode = false;
    if (argc > 1 && strcmp(argv[1], "--float") == 0) {
        floatMode = true;
    }

    char expr[MAX_INPUT_SIZE];
    
    while (fgets(expr, MAX_INPUT_SIZE, stdin)) {
        char* postfix = infixToPostfix(expr, floatMode);
        void* result = evaluatePostfix(postfix, floatMode);
        if (floatMode) {
            printf("%.2f\n", *(double*)result);
        } else {
            printf("%d\n", *(int*)result);
        }
        free(postfix);
    }
    
    return 0;
}
