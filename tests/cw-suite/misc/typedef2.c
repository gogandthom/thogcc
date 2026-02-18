/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
typedef int int_t;

typedef int_t *pint_t;

int_t g(int_t y)
{
    pint_t p;
    int_t x;
    x=y;
    p=&x;
    return 1+*p;
}
