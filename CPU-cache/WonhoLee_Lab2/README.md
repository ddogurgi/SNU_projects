# [Lab 2] Pipelined CPU Implementation

## Introduction

The goal of this lab is to implement a pipelined CPU that supports the RV32I ISA with Verilog. Building on the simple CPU that we have implemented in Lab 1, you will need to implement pipeline registers, a forwarding unit, a hazard detection unit, and supporting control logic. There are 8 tasks that you need to complete.

- Task 1~6
- Task fibo
- Task sum

Since you need to implement long lines of code in this project, **we strongly suggest you start early**. If you have any questions regarding the project, please post the questions on eTL.

Note that even though you passed all the test cases, it does not mean that your design is complete. You might end up with unexpected bugs when you do the following labs, so make sure to check your design carefully.

## Project Repository

The repository structure is almost similar to Lab 1.

```
Top (project directory)
├── src
├── data
├── docs
├── test.py
└── Makefile
```

- **src** directory
    - **modules** directory: source code for the modules that ***you will need to implement*** 
    - **simple_cpu.v**: source code for the simple CPU that ***you will need to implement***
    - **riscv_tb.v**: test bench to test out your simple CPU
- **data** directory
    - **task#**, **fib**, and **sum** directories: test cases
        - **inst.txt**: assembly code for the task
        - **inst_disassembled.mem**: machine code for the task
        - **reg_in.mem**: initial register values *before* executing the instructions
        - **reg_out.mem**: correct register values *after* executing the instructions
- **docs** directory: the pipelined CPU diagram and documentation of this lab
- **test.py**: a script to run simulations and compare the results with the correct ones 
- **Makefile**: makefile to compile your code

## How to Compile, Run, and Test Your Code

> Unless otherwise specified, the working directory is your project directory.

### Compile Code

To compile your code, you need to run `make` in your project directory. This will compile all the Verilog code including the test bench (`riscv_tb.v`) and generate a `simple_cpu` binary file, which you can execute for simulations. 

```
$ make
```

Note that whenever you modify the code, you need to recompile your code via ``make``.

### Run Simulation

After building the binary, you can simply run simulations with the following command. 

```
$ ./simple_cpu
```

By default, this reads the instructions and initial registers from `data/inst.mem` and `data/register.mem`, and runs simulations using `riscv_tb.v`. At the end of the simulation, it will display some information on the instruction memory, PC, register file, and data memory. Please check with `riscv_tb.v` to see how it displays the information. You may want to modify these files for `debugging purposes`. This will also dump the waveforms of your simulation into `sim.vcd`.

### Run Test Cases

We provide you with `test.py` to automatically run simulations and compare the output with the correct register values for the following test cases.

- `Task 1~3`: Simple arithmetic operations 
  - You should be able to pass all of these test cases if you implemented datapaths of the CPU correctly: Modules and pipeline registers
- `Task 4`: RAW instructions with 1-cycle dependency
  -  You should pass the test cases if you implemented forwarding logic correctly
- `Task 5/1`: Branch instructions
  - You should pass the test cases if you implemented flush logic correctly 
- `Task 5/2~`: RAW instructions with 2-cycle dependency: Data hazard with a preceding LW instruction 
  - You should pass the test cases if you implemented stalling logic correctly

```
$ python test.py
```

This will output the results on your terminal.

```
[*] test start


[*] starting test task1/1
[*] your register values should be
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 3, -4, 6, 7, 1, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
[*] your verilog register values are
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 3, -4, 6, 7, 1, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
[passed]


[*] starting test task1/2
[*] your register values should be
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 4, 16, 1073741822, 1, -2, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, -5]
[*] your verilog register values are
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 4, 16, 1073741822, 1, -2, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, -5]
[passed]

...
```

## How to Start?

* To understand the large codebase, it is often helpful to first analyze the wrapper module and understand the overall structure of the design. So before you get started, you may want to read the `simple_cpu.v` file first. 
* Also, we have provided `Pipelined-CPU-diagram.pdf` in the `docs` directory. Follow the `simple_cpu.v` code along with the diagram, and complete & add datapaths and control logic.
* You can also refer to the RISC-V manual that we provided you with in Lab 1.

## Caution ⚠️ 

* Do not modify any I/O ports in the modules except for forwarding, hazard detection, and pipeline registers. If you must, please consult with the TAs. We can help you and discuss what, why, and how you can do it. 
* Because the test code and Makefile rely on the pre-defined directory structure and file names, please try not to change the locations of the files. In case you want to change the directory structure, you should change the test code and Makefile too!
  * If you changed the src file name or directory, change the corresponding line in the Makefile and test.py if necessary.

## Submission

### DUE: **5/5 (FRI) 11:59 PM**

### Late Policy (**3 Days**)
* 10% discounted per day (5/6 12:00 AM is a late submission) 
* After 5/9 (TUE) 12:00 AM, you will get a **zero** score for the assignment

### What to Submit
*  Your code that we can compile and run
*  Submit all the files/directories (do `make clean` first)

### How to Submit
* Upload your compressed file (zip) to eTL
* Format: **YourStudentID_YOURLASTNAME_lab#**
	* e.g., 2023-12345_KIM_lab2.zip
* Please make sure you follow the format 
	* **10% penalty** for the wrong format

## Honor Code

**SIMPLE.** **DO NOT CHEAT!**

* This is an **individual** project. You may discuss high-level concepts with other students, but you should implement your code **all by yourself**. 
* **DO NOT POST ANY CODE IN PUBLIC** including the skeleton code and your implementations.
* We use very sophisticated tools for detecting code plagiarism (not relying on eTL). 
* All the students who get involved in violation will get a zero score for the assignment. If it is serious, there will be department-wise actions.