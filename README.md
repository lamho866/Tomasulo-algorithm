# Tomasulo's algorithm Project
如果有學弟妹看見，內容只供參考
Wiki: https://en.wikipedia.org/wiki/Tomasulo%27s_algorithm

Compile Code:
```
g++ -std=c++11 Tomasulo_algorithm.cpp -o Tomasulo_algorithm.exe
```

Execution:
T仁
```
Tomasulo_algorithm.exe
```
---
Description
可以看到每個Cycle中Buffer存的內容與數值改變。
```
Cycle 14

     _RF_______
     FF1|    0|
     FF2|   24|
     FF3|    4|
     FF4|    6|
     FF5|   64|
     -------

     _RAT_______
     FF1|ROB12|
     FF2| ROB9|
     FF3|     |
     FF4|ROB10|
     FF5|     |
     -------

   ___RS________________
   RS1|    +|    6|   24|
   RS2|    +|    0|    2|
   RS3|    +|ROB10|ROB10|
    ----------------------
BUFFER: (RS1) 6 + 24

   _____________________
   RS1|     |     |     |
   RS2|     |     |     |
    ----------------------
BUFFER:   
```

inFile.txt是存放Command
目前只有ADDI, ADD, SUB, MUL, DIV
