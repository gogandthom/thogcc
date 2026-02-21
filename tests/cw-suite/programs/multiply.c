/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
int multiply(int x, int y)
{
    int acc=0;
    if(x < 0){
        return -multiply(-x, y);
    }
    
    while(x > 0){
        acc += y;
        x--;
    }
    return acc;
}
