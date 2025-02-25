# Настройки
CC := gcc
CXX := g++
CFLAGS := -Wall -Wextra -std=c11
CPPFLAGS := -Wall -Wextra -std=c++17
LDFLAGS :=

# Артефакты
APP_EXE := app.exe
UNIT_TESTS_EXE := unit-tests.exe

# Цели
.PHONY: all clean run-int run-float run-unit-test

all: $(APP_EXE) $(UNIT_TESTS_EXE)

$(APP_EXE): src/main.c
    $(CC) $(CFLAGS) -o $@ $^

$(UNIT_TESTS_EXE): tests/unit/unit_tests.cpp
    $(CXX) $(CPPFLAGS) -o $@ $^ -lgtest -lpthread

clean:
    rm -f $(APP_EXE) $(UNIT_TESTS_EXE)

run-int: $(APP_EXE)
    ./$<

run-float: $(APP_EXE)
    ./$< --float

run-unit-test: $(UNIT_TESTS_EXE)
    ./$<
