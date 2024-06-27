import os
import random
import subprocess
import argparse

unit_tests = [ 
          [ 'task1/1', 'data/unit_tests/task1/1/inst.mem', 'data/unit_tests/task1/1/reg-out.mem' ],
          [ 'task1/2', 'data/unit_tests/task1/2/inst.mem', 'data/unit_tests/task1/2/reg-out.mem' ],
          [ 'task2/1', 'data/unit_tests/task2/1/inst.mem', 'data/unit_tests/task2/1/reg-out.mem' ],
          [ 'task2/2', 'data/unit_tests/task2/2/inst.mem', 'data/unit_tests/task2/2/reg-out.mem' ],
          [ 'task3/1', 'data/unit_tests/task3/1/inst.mem', 'data/unit_tests/task3/1/reg-out.mem' ],
          [ 'task4/1', 'data/unit_tests/task4/1/inst.mem', 'data/unit_tests/task4/1/reg-out.mem' ],
          [ 'task4/2', 'data/unit_tests/task4/2/inst.mem', 'data/unit_tests/task4/2/reg-out.mem' ],
          [ 'task4/3', 'data/unit_tests/task4/3/inst.mem', 'data/unit_tests/task4/3/reg-out.mem' ],
          [ 'task4/4', 'data/unit_tests/task4/4/inst.mem', 'data/unit_tests/task4/4/reg-out.mem' ],
          [ 'task5/1', 'data/unit_tests/task5/1/inst.mem', 'data/unit_tests/task5/1/reg-out.mem' ],
          [ 'task6/1', 'data/unit_tests/task6/1/inst.mem', 'data/unit_tests/task6/1/reg-out.mem' ],
        ]

benchmarks = [
          [ 'bst_array',   'data/benchmarks/bst_array/inst.mem',   'data/benchmarks/bst_array/reg-out.mem' ],
          [ 'fibo',        'data/benchmarks/fibo/inst.mem',        'data/benchmarks/fibo/reg-out.mem' ],
          [ 'matmul',      'data/benchmarks/matmul/inst.mem',      'data/benchmarks/matmul/reg-out.mem' ],
          [ 'prime_fact',  'data/benchmarks/prime_fact/inst.mem',  'data/benchmarks/prime_fact/reg-out.mem' ],
          [ 'quicksort',   'data/benchmarks/quicksort/inst.mem',   'data/benchmarks/quicksort/reg-out.mem' ],
          [ 'spmv',        'data/benchmarks/spmv/inst.mem',        'data/benchmarks/spmv/reg-out.mem' ],
          [ 'spconv',      'data/benchmarks/spconv/inst.mem',      'data/benchmarks/spconv/reg-out.mem' ],
        ]

hw_counters = [
        'CORE_CYCLE', 'NUM_COND_BRANCHES', 'NUM_UNCOND_BRANCHES', 'BP_CORRECT', 'BP_INCORRECT'
        ]

def get_ans_reg(path_to_reg):
    cwd = os.getcwd()
    reg_f = open(os.path.join(cwd, path_to_reg), "r")
    lines = reg_f.readlines()

    reg = [] 
    for line in lines:
        words = line.split()
        if len(words) == 0:
            continue
        reg.append(int(words[0]))
    return reg


def execute_veri():
    process = os.system("./simple_cpu > data/stat.out")


def get_veri_reg():
    cwd = os.getcwd()
    reg_f = open(os.path.join(cwd, "data/stat.out"), "r")
    lines = reg_f.readlines()
    
    reg = []
    for line in lines:
        words = line.split()
        if len(words) == 0:
            continue
        if "Reg" in words[1]:
            if (words[3] == 'x' or words[3] == 'z' or words[3] == 'X'):
                reg.append(words[3])
            else:
                reg_val = int(words[3])
                reg.append(reg_val)

    return reg


def print_hw_stats():
    print("[*] ----- Hardware counter stats -----")
    cwd = os.getcwd()
    stats_f = open(os.path.join(cwd, "data/stat.out"), "r")
    lines = stats_f.readlines()

    for line in lines:
        words = line.split()
        if len(words) == 0:
            continue
        else: 
            for hw_counter in hw_counters:
                if hw_counter in words[1]:
                    print("[*] {}: {}".format(hw_counter, words[2]))


def check_sim(tests, test_type):
    for test in tests:
        print("\n")
        print("[*] ----- Starting test " + test[0] + " -----")
        os.system('cp ' + test[1] + ' ./data/inst.mem')
        if os.path.exists("./data/" + test_type + '/' + test[0] + '/register.mem'):
            os.system('cp ' + './data/' + test_type + '/' + test[0] + '/register.mem' + ' ./data/register.mem')
        if os.path.exists("./data/" + test_type + '/' + test[0] + '/data.mem'):
            os.system('cp ' + './data/' + test_type + '/' + test[0] + '/data.mem' + ' ./data/data_memory.mem')
 
        ans_reg = get_ans_reg(test[2])

        print("[*] Your register values should be")
        print(ans_reg)

        execute_veri()

        veri_reg = get_veri_reg()
        print("[*] Your verilog register values are")
        print(veri_reg)

        if ans_reg == veri_reg:
            print("[Passed]")
            print_hw_stats()
            os.system("rm ./data/inst.mem ./data/stat.out")
        else:
            print("------ [Failed] -----")
            os.system("cat ./data/stat.out")
            break


def main():
    parser = argparse.ArgumentParser(description="Test Verilog")
    parser.add_argument('--test', type=str, help='needs to be one of [unit, benchmark, all]')

    args = parser.parse_args()

    if (args.test == 'unit'):
        print("[*] Unit test start")
        check_sim(unit_tests, 'unit_tests')
    elif (args.test == 'benchmark'):
        print("[*] Benchmark test start")
        check_sim(benchmarks, 'benchmarks')
    elif (args.test == 'all'):
        print("[*] Unit test start")
        check_sim(unit_tests, 'unit_tests')
        print("\n")

        print("[*] Benchmark test start")
        check_sim(benchmarks, 'benchmarks')
    else:
        print("usage: python test.py --test=unit")

if __name__ == '__main__':
    main()
