/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
int fib(int x)
{
    if(x <= 0){
        return 0;
    }
    if(x == 1){
        return 1;
    }
    return fib(x-1)+fib(x-2);
}
