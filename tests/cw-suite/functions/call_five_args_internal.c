/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
int g(int a, int b, int c, int d, int e)
{
    return a+b+c+d+e;
}

int f()
{
    return g(1,2,3,4,5);
}
