/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike --isa=rv32gc pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
struct x{
    int y;
    int z;
};

int f()
{
    struct x g;
    g.y=17;
    g.z=13;
    return g.y+g.z;
}