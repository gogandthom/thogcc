/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
int r2(int n);

int r1(int n)
{
    if(n==0){
        return 1;
    }else{
        return r2(n-1)+r2(n-1);
    }
}
