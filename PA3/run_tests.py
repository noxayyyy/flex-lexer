import os
import subprocess
import sys
import glob
import difflib

EXECUTABLE = os.path.join("build", "compiler")
INPUT_DIR = os.path.join("tests", "inputs")
EXPECTED_DIR = os.path.join("tests", "outputs")

class Colors:
    HEADER = '\033[95m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

def run_make():
    print(f"{Colors.HEADER}--- Building Project ---{Colors.ENDC}")
    result = subprocess.run(["make"], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"{Colors.FAIL}Build Failed!{Colors.ENDC}")
        print(result.stderr) # Print the compiler error
        sys.exit(1)
    print(f"{Colors.OKGREEN}Build Successful.{Colors.ENDC}\n")

def normalize_output(text):
    if not text: return ""
    return text.strip().replace('\r\n', '\n')

def get_expected_filename(input_filename):
    base_name = os.path.basename(input_filename)
    name_part, ext_part = os.path.splitext(base_name)
    return os.path.join(EXPECTED_DIR, f"{name_part}_output{ext_part}")

def run_test(input_file):
    expected_file = get_expected_filename(input_file)
    test_name = os.path.basename(input_file)

    if not os.path.exists(expected_file):
        return "MISSING", f"Missing expected output file: {expected_file}", ""

    try:
        result = subprocess.run(
            [EXECUTABLE, input_file],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT, 
            text=True
        )
        
        actual_output = normalize_output(result.stdout)
        
        with open(expected_file, "r") as f:
            expected_output = normalize_output(f.read())

        if actual_output == expected_output:
            return "PASS", actual_output, expected_output
        else:
            return "FAIL", actual_output, expected_output

    except Exception as e:
        return "ERROR", str(e), ""

def main():
    run_make()

    input_files = sorted(glob.glob(os.path.join(INPUT_DIR, "*.txt")))
    
    if not input_files:
        print(f"{Colors.WARNING}No input files found in {INPUT_DIR}{Colors.ENDC}")
        return

    print(f"{Colors.HEADER}--- Running {len(input_files)} Tests ---{Colors.ENDC}")
    
    passed_count = 0
    failed_tests = []

    for input_file in input_files:
        filename = os.path.basename(input_file)
        status, actual, expected = run_test(input_file)

        if status == "PASS":
            print(f"[{Colors.OKGREEN}PASS{Colors.ENDC}] {filename}")
            passed_count += 1
        elif status == "FAIL":
            print(f"[{Colors.FAIL}FAIL{Colors.ENDC}] {filename}")
            failed_tests.append((filename, expected, actual))
        elif status == "MISSING":
            print(f"[{Colors.WARNING}SKIP{Colors.ENDC}] {filename} (Expected output missing)")
        else:
            print(f"[{Colors.FAIL}ERR {Colors.ENDC}] {filename}: {actual}")

    if failed_tests:
        print(f"\n{Colors.HEADER}--- Failure Details ---{Colors.ENDC}")
        for fname, exp, act in failed_tests:
            print(f"{Colors.WARNING}Differences for {fname}:{Colors.ENDC}")
            diff = difflib.unified_diff(
                exp.splitlines(), 
                act.splitlines(), 
                fromfile='Expected', 
                tofile='Actual', 
                lineterm=''
            )
            for line in diff:
                if line.startswith('+'):
                    print(f"{Colors.OKGREEN}{line}{Colors.ENDC}")
                elif line.startswith('-'):
                    print(f"{Colors.FAIL}{line}{Colors.ENDC}")
                else:
                    print(line)
            print("-" * 30)

    print(f"\n{Colors.HEADER}--- Summary ---{Colors.ENDC}")
    total = len(input_files)
    if passed_count == total:
        print(f"{Colors.OKGREEN}All {total} tests passed! Great job!{Colors.ENDC}")
        sys.exit(0)
    else:
        print(f"{Colors.FAIL}{passed_count}/{total} tests passed.{Colors.ENDC}")
        sys.exit(1)

if __name__ == "__main__":
    main()