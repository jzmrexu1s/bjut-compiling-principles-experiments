# bjut-compiling-principles-experiments
Coursework of compiling principles in BJUT. 
## Environment
Tested on Ubuntu 18.04.4 and macOS 10.15.4 with gcc (or clang), flex and bison installed. 

Yacc_LR_demo is only avilible on Windows now.
## Usage
### Task1
Under /task1:  
```shell
lex t1_demo_lzh.lex  
gcc -o task1 lex.yy.c -lm  
./task1  
```
Or:  
```shell
./generate_lex.sh  
./task1  
or
./task1 in.txt
or
./task1 in.txt chart.txt
```
### Task2
Under /task2t:  
```shell
make clean
make all 
./task2
./task2 in.txt  
```
Check out.txt to obtain result after execution if an argument has been passed.

### Task3
Under /task3:
'''
make clean
make all
./task3
./task3 in.txt
'''
Check out.txt to obtain result after execution if an argument has been passed.

### yacc_LR_demo
Under /yacc_LR_demo:
```shell
Only avilible on Windows:
Click "yacc_LR_demo.exe",then you can get everything you want.
If you want to use this yacc_demo to analyze another sentence, please check "User_Guide.txt".
```
