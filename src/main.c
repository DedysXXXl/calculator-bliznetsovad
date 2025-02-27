#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>    // Для isdigit()
#include <string.h>   // Для strlen(), strcat()
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>   // Для intptr_t

#define MAX_INPUT_SIZE 1024

// Структура стека
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
    initStack(&operatorStack, MAX_INPUT_SIZE, false); // Стек для операторов
    int i = 0, k = 0;
    // Выделяем достаточно памяти (наихудший случай: каждый символ и пробел)
    char* postfix = (char*)malloc(strlen(infix) * 2 + 1);
    postfix[0] = '\0';
    
    bool prevWasOperator = true; // В начале считаем, что предыдущего токена нет (т.е. унарный минус возможен)
    
    while (infix[i] != '\0') {
        if (isspace(infix[i])) {
            i++;
            continue;
        }
        // Если текущий символ '-' и предыдущий токен — оператор или его нет, то это унарный минус
        if (infix[i] == '-' && prevWasOperator) {
            // Копируем знак минус в выходную строку как часть числа
            postfix[k++] = '-';
            i++;
        }
        // Если число (цифра или точка для float)
        if (isdigit(infix[i]) || (floatMode && infix[i] == '.')) {
            while (isdigit(infix[i]) || (floatMode && infix[i] == '.')) {
                postfix[k++] = infix[i++];
            }
            postfix[k++] = ' ';
            postfix[k] = '\0';
            prevWasOperator = false;
            continue;
        }
        else if (infix[i] == '(') {
            int op = infix[i];
            push(&operatorStack, &op, false);
            prevWasOperator = true;
        }
        else if (infix[i] == ')') {
            while (!isEmpty(&operatorStack) && (*(int*)peek(&operatorStack, false)) != '(') {
                int op = *(int*)pop(&operatorStack, false);
                postfix[k++] = (char)op;
                postfix[k++] = ' ';
            }
            if (isEmpty(&operatorStack)) {
                fprintf(stderr, "Ошибка: неверная расстановка скобок\n");
                exit(EXIT_FAILURE);
            } else {
                pop(&operatorStack, false); // Удаляем '('
            }
            prevWasOperator = false;
        }
        else { // оператор (кроме унарного минуса, который уже обработан)
            while (!isEmpty(&operatorStack) &&
                   precedence(*(int*)peek(&operatorStack, false)) >= precedence(infix[i])) {
                int op = *(int*)pop(&operatorStack, false);
                postfix[k++] = (char)op;
                postfix[k++] = ' ';
            }
            int op = infix[i];
            push(&operatorStack, &op, false);
            prevWasOperator = true;
        }
        i++;
    }
    
    while (!isEmpty(&operatorStack)) {
        int op = *(int*)pop(&operatorStack, false);
        postfix[k++] = (char)op;
        postfix[k++] = ' ';
    }
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
        if (isdigit(postfix[i]) || (floatMode && (postfix[i] == '-' || postfix[i] == '.') ) || 
           (postfix[i] == '-' && (postfix[i+1] && (isdigit(postfix[i+1]) || (floatMode && postfix[i+1]=='.'))))) {
            // Читаем число (strtod корректно обработает знак минус)
            if (floatMode) {
                double num = strtod(&postfix[i], NULL);
                push(&operandStack, &num, true);
            } else {
                int num = atoi(&postfix[i]);
                push(&operandStack, &num, false);
            }
            while (postfix[i] && !isspace(postfix[i]))
                i++;
        }
        else {
            // Оператор: извлекаем два числа из стека
            void* b_ptr = pop(&operandStack, floatMode);
            void* a_ptr = pop(&operandStack, floatMode);
            if (!b_ptr || !a_ptr) {
                fprintf(stderr, "Ошибка: стек пуст при вычислении\n");
                exit(EXIT_FAILURE);
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
                            fprintf(stderr, "Ошибка: деление на ноль\n");
                            exit(EXIT_FAILURE);
                        }
                        res = a / b;
                        break;
                    default:
                        fprintf(stderr, "Ошибка: неизвестный оператор\n");
                        exit(EXIT_FAILURE);
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
                            fprintf(stderr, "Ошибка: деление на ноль\n");
                            exit(EXIT_FAILURE);
                        }
                        res = a / b;
                        break;
                    default:
                        fprintf(stderr, "Ошибка: неизвестный оператор\n");
                        exit(EXIT_FAILURE);
                }
                push(&operandStack, &res, false);
            }
            i++;
        }
    }
    // Извлекаем результат из стека и копируем в выделенную память
    void* res_ptr = pop(&operandStack, floatMode);
    void* result;
    if (floatMode) {
        result = malloc(sizeof(double));
        *(double*)result = *(double*)res_ptr;
    } else {
        result = malloc(sizeof(int));
        *(int*)result = *(int*)res_ptr;
    }
    free(operandStack.array);
    return result;
}

// Основная функция (однократное вычисление и завершение)
#ifndef UNIT_TEST
int main(int argc, char** argv) {
    bool floatMode = false;
    if (argc > 1 && strcmp(argv[1], "--float") == 0) {
        floatMode = true;
    }
    
    char expr[MAX_INPUT_SIZE];
    if (fgets(expr, MAX_INPUT_SIZE, stdin)) {
        char* postfix = infixToPostfix(expr, floatMode);
        void* result = evaluatePostfix(postfix, floatMode);
        if (floatMode) {
            printf("%.2f\n", *(double*)result);
        } else {
            printf("%d\n", *(int*)result);
        }
        free(postfix);
        free(result);
    }
    
    return 0;
}
#endif
