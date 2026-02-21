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
    int i;
    int x[8];
    int acc;
    for(i=8; i<16; i++){
        x[i-8]=i;
    }
    acc=0;
    for(i=0; i<8; i++){
        acc=acc+x[i+0];
    }
    return acc;
}
