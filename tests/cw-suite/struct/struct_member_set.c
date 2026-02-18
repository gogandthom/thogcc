/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
struct x{
    int y;
};

int f()
{
    struct x z;
    z.y=17;
    return 13;
}