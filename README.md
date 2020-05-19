# bjut-compiling-principles-experiments
Coursework of compiling principles in BJUT. 
## Environment
Tested on Ubuntu 18.04.4 and macOS 10.15.4.
## Usage
### Task1
Under /task1:  
```shell
lex t1_demo_lzh.lex  
gcc -o task1 lex.yy.c -lm  
./task1  
```