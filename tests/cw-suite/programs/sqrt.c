/*
RUN: %thogcc -S %s -o %t.s
RUN: %riscv_gcc -c %t.s -o %t.o
RUN: %riscv_gcc -static %t.o %S/driver/$(basename %s .c)_driver.c -o %t
RUN: spike pk %t
RUN: echo $? | FileCheck %s

CHECK: 0
*/
int bsqrt(int lo, int hi, int val)
{
    while(lo+1 < hi){
        int mid=(lo+hi)>>1;
        int sqr=mid*mid;
        if(sqr <= val){
            lo=mid;
        }else{
            hi=mid;
        }
    }
    if( lo*lo < val ) {
        return hi;
    }else{
        return lo;
    }
}
