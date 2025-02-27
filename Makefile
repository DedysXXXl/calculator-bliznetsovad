# Компиляторы и флаги
CC := gcc
CXX := g++

CFLAGS := -Wall -Wextra -Wpedantic -Werror -std=c11
CXXFLAGS := -Wall -Wextra -Wpedantic -Werror -std=c++17
LDFLAGS := -lstdc++ -lm -lpthread -lgtest -lgtest_main

# Каталоги
SRC_DIR := src
BUILD_DIR := build
UNIT_TESTS_DIR := tests/unit
INTEGRATION_TEST_DIR := tests/integration

# Исходники и артефакты приложения
APP_SRC := $(SRC_DIR)/main.c
APP_OBJ := $(BUILD_DIR)/main.o
APP_EXE := $(BUILD_DIR)/app.exe

# Исходники и артефакты юнит-тестов
TEST_SRC := $(UNIT_TESTS_DIR)/unit_tests.cpp
TEST_OBJ := $(BUILD_DIR)/unit_tests.o $(BUILD_DIR)/main_test.o
TEST_EXE := $(BUILD_DIR)/unit-tests.exe

# Параметры форматирования
FORMAT_DIRS := src tests/unit
CLANG_FORMAT := clang-format

# Параметры для интеграционных тестов (Python+pytest)
VENV := venv
PYTHON := $(VENV)/bin/python
PIP := $(VENV)/bin/pip
INT_TESTS := $(INTEGRATION_TEST_DIR)/integration_tests.py  # Исправлено на integration_tests.py

.PHONY: all clean run-int run-float run-unit-tests run-integration-tests venv format

# Цель "all" собирает приложение и юнит-тесты
all: $(APP_EXE) $(TEST_EXE)

# --- Сборка приложения ---
$(APP_EXE): $(APP_OBJ)
	@mkdir -p $(BUILD_DIR)
	@echo "Компилирование основного исполняемого файла..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/main.o: $(APP_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# --- Сборка тестового объекта ---
$(BUILD_DIR)/main_test.o: $(APP_SRC)
	@mkdir -p $(BUILD_DIR)
	@echo "Компилирование объектного файла для тестирования..."
	$(CC) $(CFLAGS) -DUNIT_TEST -c -o $@ $<

$(BUILD_DIR)/unit_tests.o: $(TEST_SRC)
	@mkdir -p $(BUILD_DIR)
	@echo "Компилирование объектного файла тестов..."
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c -o $@ $<

# --- Сборка юнит-тестов ---
$(TEST_EXE): $(TEST_OBJ)
	@mkdir -p $(BUILD_DIR)
	@echo "Компилирование юнит-тестов..."
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJ) $(LDFLAGS)

# --- Очистка ---
clean:
	@echo "Очистка проекта..."
	@rm -rf $(BUILD_DIR) $(VENV)

# --- Запуск приложения ---
run-int: $(APP_EXE)
	@echo "Запуск программы в целочисленном режиме..."
	@./$(APP_EXE)

run-float: $(APP_EXE)
	@echo "Запуск программы в режиме с плавающей точкой..."
	@./$(APP_EXE) --float

# --- Запуск юнит-тестов ---
run-unit-tests: $(TEST_EXE)
	@echo "Запуск юнит-тестов..."
	@./$(TEST_EXE)

# --- Форматирование исходников ---
format:
	@echo "Форматирование исходников..."
	@find $(FORMAT_DIRS) -type f \( -name "*.cpp" -o -name "*.c" -o -name "*.h" \) -exec $(CLANG_FORMAT) -i -style=file {} +

# --- Создание виртуального окружения и установка pytest ---
venv:
	@echo "Создание виртуального окружения..."
	@python3 -m venv $(VENV)
	@$(PIP) install --upgrade pip
	@$(PIP) install pytest

# --- Запуск интеграционных тестов ---
run-integration-tests: venv $(APP_EXE)
	@echo "Запуск интеграционных тестов..."
	@$(PYTHON) -m pytest -v $(INT_TESTS)
