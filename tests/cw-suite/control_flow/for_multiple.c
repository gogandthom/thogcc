/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
int f()
{
    int x;
    int y;
    y=0;
    for(x=0; x<10; x=x+1){
        y=y-1;
    }
    return y;
}
