import subprocess
import os
import glob

INPUT_DIR = "tests/inputs"
EXPECTED_IR_DIR = "tests/expected_ir"
EXPECTED_SPIM_DIR = "tests/expected_output"
MIPS_OUTPUT_DIR = "tests/mips"
COMPILER_BIN = "./build/compiler"

def run_command(command):
    result = subprocess.run(command, capture_output=True, text=True)
    return result

def clean_spim_output(raw_output):
    lines = raw_output.splitlines()
    return "\n".join(lines[5:]).strip()

def run_tests():
    count = 0
    print("--- Building Compiler ---")
    make_proc = subprocess.run(["make"], capture_output=True, text=True)
    if make_proc.returncode != 0:
        print("❌ Make failed!")
        print(make_proc.stderr)
        return

    os.makedirs(MIPS_OUTPUT_DIR, exist_ok=True)

    input_files = glob.glob(os.path.join(INPUT_DIR, "test*.txt"))
    input_files.sort()

    for input_path in input_files:
        ir_matched, spim_matched = False, False
        test_name = os.path.basename(input_path).replace(".txt", "")
        mips_path = os.path.join(MIPS_OUTPUT_DIR, f"{test_name}.s")
        
        print(f"\n🚀 Running {test_name}...")

        compiler_proc = run_command([COMPILER_BIN, input_path, mips_path])
        
        expected_ir_path = os.path.join(EXPECTED_IR_DIR, f"{test_name}_output.txt")
        if os.path.exists(expected_ir_path):
            with open(expected_ir_path, 'r') as f:
                expected_ir = f.read().strip()
            
            actual_ir = compiler_proc.stdout.strip()
            if actual_ir == expected_ir:
                ir_matched = True
                print(f"  ✅ IR Optimization Match")
            else:
                print(f"  ❌ IR Optimization Mismatch")
        else:
            print(f"  ⚠️  Skipping IR check (Missing {expected_ir_path})")

        spim_proc = run_command(["spim", "-file", mips_path])
        actual_runtime_output = clean_spim_output(spim_proc.stdout)
        
        expected_spim_path = os.path.join(EXPECTED_SPIM_DIR, f"{test_name}_spim.txt")
        if os.path.exists(expected_spim_path):
            with open(expected_spim_path, 'r') as f:
                expected_runtime_output = clean_spim_output(f.read())
            
            if actual_runtime_output == expected_runtime_output:
                spim_matched = True
                print(f"  ✅ Runtime Output Match")
            else:
                print(f"  ❌ Runtime Output Mismatch")
                print(f"     Expected: {expected_runtime_output}")
                print(f"     Got:      {actual_runtime_output}")
        else:
            print(f"  ⚠️  Skipping Runtime check (Missing {expected_spim_path})")
        
        if ir_matched and spim_matched:
            count = count + 1
    
    print(f"\nPassed {count}/{len(input_files)} test cases\n")

if __name__ == "__main__":
    run_tests()
