#!/usr/bin/env python3

import sys
import random
import subprocess
import time
import os

sys.set_int_max_str_digits(0)

os.makedirs('output', exist_ok=True)

def gen():

    # 10^1000
    # min_val = 10**999
    # max_val = 10**1000 - 1

    # 10^100000
    # min_val = 10**99999
    # max_val = 10**100000 - 1

    # 10^1000000
    min_val = 10**999999
    max_val = 10**1000000 - 1

    print("Generating first number...")
    a = random.randint(min_val, max_val)

    print("Generating second number...")
    b = random.randint(min_val, max_val)
    
    a_str = str(a)
    b_str = str(b)
    
    print(f"A: exactly {len(a_str)} digits")
    print(f"B: exactly {len(b_str)} digits")
    
    return a_str, b_str

def run(a, b):
    with open('output/a.txt', 'w') as f:
        f.write(a)
    
    with open('output/b.txt', 'w') as f:
        f.write(b)
    
    print(f"Saved a and b to output/a.txt and output/b.txt")
    
    print("\nRunning multiplication program...")
    input_data = f"{a}\n{b}\n"
    
    try:
        start_time = time.time()

        result = subprocess.run(
            ['./multiply'],
            input=input_data,
            text=True,
            capture_output=True
        )

        end_time = time.time()
        
        if result.returncode == 0:
            lines = result.stdout.strip().split('\n')
            for line in lines:
                if line.startswith("Result: "):
                    result_number = line.replace("Result: ", "").strip()
                    
                    with open('output/result.txt', 'w') as f:
                        f.write(result_number)
                    
                    print(f"Program Success!")
                    print(f"Time: {end_time - start_time:.2f} seconds")
                    print(f"Result: {len(result_number)} digits")
                    print(f"Saved to: output/result.txt")
                    return True
        
        print(f"Program failed: {result.stderr}")
        return False
        
    except Exception as e:
        print(f"Error running program: {e}")
        return False

if __name__ == "__main__":
    a, b = gen()
    run(a, b)