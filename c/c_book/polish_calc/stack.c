#include <stdio.h>
#include "calc.h"
#define MAXVAL 100

static int sp = 0;      //static means only this file can access them
static double val[MAXVAL]; //static vars in functions, instead provide permanent memory in funcs

void push(double f){
    if(sp < MAXVAL)
        val[sp++] = f;
    else
        printf("error: stack full, cant't push %g\n", f);
}

double pop(void){
    if(sp > 0)
        return val[--sp];
    else{
        printf("eeror: stack empty\n");
        return 0.0;
    }
}