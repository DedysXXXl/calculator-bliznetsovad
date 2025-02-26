import subprocess
import os

def run_calculator(input_data):
    # Указываем путь к исполняемому файлу калькулятора
    calculator_path = os.path.join("build", "calc.exe")
    # Запускаем калькулятор и передаем входные данные
    result = subprocess.run([calculator_path, input_data], capture_output=True, text=True)
    return result.stdout.strip()  # Возвращаем очищенное от пробелов и переносов строк значение

def test_addition():
    expected_result = "30"
    actual_result = run_calculator("10 + 20")
    assert expected_result == actual_result, f"Expected {expected_result}, but got {actual_result}"

def test_subtraction():
    expected_result = "-10"
    actual_result = run_calculator("20 - 30")
    assert expected_result == actual_result, f"Expected {expected_result}, but got {actual_result}"

def test_multiplication():
    expected_result = "200"
    actual_result = run_calculator("10 * 20")
    assert expected_result == actual_result, f"Expected {expected_result}, but got {actual_result}"

def test_division():
    expected_result = "5"
    actual_result = run_calculator("100 / 20")
    assert expected_result == actual_result, f"Expected {expected_result}, but got {actual_result}"

def test_float_mode():
    expected_result = "2.50"
    actual_result = run_calculator("--float 5 / 2")
    assert expected_result == actual_result, f"Expected {expected_result}, but got {actual_result}"

# Вызов всех тестов
if __name__ == "__main__":
    test_addition()
    test_subtraction()
    test_multiplication()
    test_division()
    test_float_mode()
