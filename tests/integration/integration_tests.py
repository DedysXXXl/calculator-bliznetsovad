import subprocess
import os

def run_calculator(input_data, float_mode=False):
    calculator_path = os.path.join("build", "app.exe")
    command = [calculator_path]
    if float_mode:
        command.append("--float")
    
    # Передаем входные данные через stdin
    result = subprocess.run(
        command,
        input=input_data,
        text=True,
        capture_output=True
    )
    return result.stdout.strip()

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
    expected_result = "2.5000"
    actual_result = run_calculator("5 / 2", float_mode=True)
    assert expected_result == actual_result, f"Expected {expected_result}, but got {actual_result}"

# Вызов всех тестов
if __name__ == "__main__":
    test_addition()
    test_subtraction()
    test_multiplication()
    test_division()
    test_float_mode()
