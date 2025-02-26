# Определяем компиляторы и флаги
CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra -std=c11
CXXFLAGS := $(CFLAGS) -std=c++17
LDFLAGS := -lgtest -lpthread

# Определяем исходники и объекты
SRCS := src/calc.c
OBJS := $(patsubst %.c,%.o,$(SRCS))

# Цель для сборки основного приложения
calc.exe: $(OBJS)
    $(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Цель для сборки юнит-тестов
unit-tests.exe: tests/unit/unit_tests.cpp $(OBJS)
    $(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Цель для компиляции отдельных объектов
build/%.o: src/%.c
    $(CC) $(CFLAGS) -c $< -o $@

# Цель для очистки всех артефактов
clean:
    rm -rf build/* *.exe

# Цели для запуска приложений
run-calc: calc.exe
    ./calc.exe

run-unit-test: unit-tests.exe
    ./unit-tests.exe

# Очистка артефактов
.PHONY: all clean run-calc run-unit-test
