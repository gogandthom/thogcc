/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
int g(int t)
{
    int x;
    x=0;
    switch(t)
    {
        case 0:
            x=1;
            break;
        case 2:
            x=2;
        case 1:
            x=x+1;
            break;
        default:
            x=t+1;
    }
    return x;
}
