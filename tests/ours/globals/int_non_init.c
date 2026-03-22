/*
RUN: %runtest | FileCheck %s

CHECK: 0
*/
int g_myvar;

int f() {
    g_myvar = 3;
    return g_myvar + 2;
}
