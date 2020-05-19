# bjut-compiling-principles-experiments
Coursework of compiling principles in BJUT. 
## Environment
Tested on Ubuntu 18.04.4 and macOS 10.15.4.
## Usage
### 实验一
在/task1目录下：  
lex t1_demo_lzh.lex  
gcc -o task1 lex.yy.c -lm  
./task1