import os
import subprocess
import difflib
import sys

LEXER_EXECUTABLE = "./build/lexer"  
INPUT_TEST_DIR = "tests/inputs"                
OUTPUT_TEST_DIR = "tests/outputs"
INPUT_PREFIX = "test"
INPUT_SUFFIX = ".txt"
OUTPUT_SUFFIX = "_output.txt"
NUM_TESTS = 4

def run_test(test_num):
    test_id = f"{test_num:02d}"
    input_file = os.path.join(INPUT_TEST_DIR, f"{INPUT_PREFIX}{test_id}{INPUT_SUFFIX}")
    expected_output_file = os.path.join(OUTPUT_TEST_DIR, f"{INPUT_PREFIX}{test_id}{OUTPUT_SUFFIX}")
    
    if not os.path.exists(input_file):
        print(f"⚠️  Skipping Test {test_id}: Input file not found ({input_file})")
        return False
    if not os.path.exists(expected_output_file):
        print(f"⚠️  Skipping Test {test_id}: Golden output file not found ({expected_output_file})")
        return False

    try:
        result = subprocess.run(
            [LEXER_EXECUTABLE, input_file],
            capture_output=True,
            text=True,
            check=True
        )
        actual_output = result.stdout.strip()
        
        with open(expected_output_file, 'r') as f:
            expected_output = f.read().strip()

        actual_output = actual_output.replace('\r\n', '\n')
        expected_output = expected_output.replace('\r\n', '\n')

        if actual_output == expected_output:
            print(f"✅ Test {test_id}: PASS")
            return True
        else:
            print(f"❌ Test {test_id}: FAIL")
            print("-" * 40)
            print("Diff (Expected vs Actual):")
            
            diff = difflib.unified_diff(
                expected_output.splitlines(),
                actual_output.splitlines(),
                fromfile='Expected',
                tofile='Actual',
                lineterm=''
            )
            for line in diff:
                print(line)
            print("-" * 40)
            return False

    except subprocess.CalledProcessError as e:
        print(f"❌ Test {test_id}: CRASHED (Exit Code {e.returncode})")
        print(f"Error Output:\n{e.stderr}")
        return False
    except FileNotFoundError:
        print(f"❌ Error: Lexer executable '{LEXER_EXECUTABLE}' not found.")
        print("   Did you forget to compile it? (e.g., 'gcc lex.yy.c driver.c -o lexer')")
        sys.exit(1)

def main():
    print(f"🚀 Starting Test Runner for {NUM_TESTS} tests...\n")
    
    passed_count = 0
    attempted_count = 0

    for i in range(1, NUM_TESTS + 1):
        if run_test(i):
            passed_count += 1
        attempted_count += 1

    print("\n" + "=" * 30)
    print(f"SUMMARY: {passed_count}/{attempted_count} Tests Passed")
    print("=" * 30)

    if passed_count == attempted_count:
        print("🎉 All systems nominal.")
        sys.exit(0)
    else:
        print("⚠️  Some tests failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()
