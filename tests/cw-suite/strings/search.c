/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
char *search(char *x, char c)
{
    while(*x){
        if(*x==c){
            return x;
        }
        x=x+1;
    }
    return 0;
}
