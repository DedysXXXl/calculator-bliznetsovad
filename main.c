#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>  // Для функции isdigit()
#include <string.h> // Для функции strlen()

#define MAX_INPUT_SIZE 1024

// Структура стека для чисел
struct Stack {
    int top;
    int capacity;
    int* array;
};

// Инициализация стека
void initStack(struct Stack* stack, int size) {
    stack->top = -1;
    stack->capacity = size;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
}

// Проверка пустоты стека
int isEmpty(struct Stack* stack) {
    return stack->top == -1;
}

// Добавление элемента в стек
void push(struct Stack* stack, int value) {
    if (stack->top >= stack->capacity - 1) {
        fprintf(stderr, "Ошибка: стек переполнен\n");
        exit(EXIT_FAILURE);
    }
    stack->array[++stack->top] = value;
}

// Извлечение элемента из стека
int pop(struct Stack* stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Ошибка: стек пуст\n");
        exit(EXIT_FAILURE);
    }
    return stack->array[stack->top--];
}

// Получение верхнего элемента стека
int peek(struct Stack* stack) {
    if (isEmpty(stack)) {
        fprintf(stderr, "Ошибка: стек пуст\n");
        exit(EXIT_FAILURE);
    }
    return stack->array[stack->top];
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
char* infixToPostfix(char* infix) {
    struct Stack operatorStack;
    initStack(&operatorStack, MAX_INPUT_SIZE);
    
    int i = 0, k = 0;
    char* postfix = (char*)malloc(strlen(infix) + 1);
    
    while (infix[i] != '\0') {
        if (isspace(infix[i])) {
            i++;
            continue;
        }
        
        if (isdigit(infix[i])) {
            while (isdigit(infix[i]) && infix[i] != '\0')
                postfix[k++] = infix[i++];
            
            postfix[k++] = ' ';
            i--;
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
int evaluatePostfix(char* postfix) {
    struct Stack operandStack;
    initStack(&operandStack, MAX_INPUT_SIZE);
    
    int i = 0;
    while (postfix[i] != '\0') {
        if (isspace(postfix[i])) {
            i++;
            continue;
        }
        
        if (isdigit(postfix[i])) {
            int num = 0;
            while (isdigit(postfix[i]))
                num = num * 10 + (postfix[i++] - '0');
            
            push(&operandStack, num);
            i--;
        } else {
            int b = pop(&operandStack);
            int a = pop(&operandStack);
            int res;
            
            switch (postfix[i]) {
                case '+': res = a + b; break;
                case '-': res = a - b; break;
                case '*': res = a * b; break;
                case '/': res = a / b; break;
                default : fprintf(stderr, "Ошибка: неизвестный оператор\n");
                          exit(EXIT_FAILURE);
            }
            
            push(&operandStack, res);
        }
        i++;
    }
    
    int result = pop(&operandStack);
    free(operandStack.array);
    return result;
}

// Основная функция
int main() {
    char expr[MAX_INPUT_SIZE];
    
    while (fgets(expr, MAX_INPUT_SIZE, stdin)) {
        char* postfix = infixToPostfix(expr);
        int result = evaluatePostfix(postfix);
        printf("%d\n", result);
        free(postfix);
    }
    
    return 0;
}
