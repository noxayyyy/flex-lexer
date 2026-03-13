import os
import subprocess
import sys

PARSER_EXEC = os.path.join("build", "syntax")
INPUT_DIR = os.path.join("tests", "inputs")
OUTPUT_DIR = os.path.join("tests", "outputs")

GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

def run_test(input_file):
    filename = os.path.basename(input_file)
    test_name = os.path.splitext(filename)[0] 
    
    expected_output_file = os.path.join(OUTPUT_DIR, f"{test_name}_output.txt")

    if not os.path.exists(expected_output_file):
        print(f"[{test_name}] {RED}SKIPPED{RESET} (No expected output file found)")
        return False

    try:
        with open(input_file, "r") as f_in:
            result = subprocess.run(
                [PARSER_EXEC], 
                stdin=f_in, 
                capture_output=True, 
                text=True
            )
    except FileNotFoundError:
        print(f"{RED}Error: Executable '{PARSER_EXEC}' not found. Did you run 'make'?{RESET}")
        sys.exit(1)

    actual_output = result.stdout.strip().replace("\r\n", "\n")
    
    with open(expected_output_file, "r") as f_exp:
        expected_output = f_exp.read().strip().replace("\r\n", "\n")

    if actual_output == expected_output:
        print(f"[{test_name}] {GREEN}PASS{RESET}")
        return True
    else:
        print(f"[{test_name}] {RED}FAIL{RESET}")
        print("-" * 20)
        print(f"Expected:\n{expected_output[:200]}...")
        print("-" * 20)
        print(f"Got:\n{actual_output[:200]}...")
        print("-" * 20)
        return False

def main():
    if not os.path.exists(PARSER_EXEC):
        print(f"{RED}Error: Parser executable not found at {PARSER_EXEC}{RESET}")
        print("Please run 'make' first.")
        sys.exit(1)

    if not os.path.exists(INPUT_DIR):
        print(f"{RED}Error: Input directory {INPUT_DIR} not found.{RESET}")
        sys.exit(1)

    files = sorted([f for f in os.listdir(INPUT_DIR) if f.endswith(".txt")])
    
    if not files:
        print("No test files found in tests/inputs/")
        sys.exit(0)

    print(f"Found {len(files)} tests. Running...\n")

    passed = 0
    total = 0

    for f in files:
        total += 1
        input_path = os.path.join(INPUT_DIR, f)
        if run_test(input_path):
            passed += 1

    print("\n" + "=" * 30)
    if passed == total:
        print(f"{GREEN}All {total} tests passed!{RESET}")
    else:
        print(f"{RED}{passed}/{total} tests passed.{RESET}")
    print("=" * 30)

    if passed != total:
        sys.exit(1)

if __name__ == "__main__":
    main()
