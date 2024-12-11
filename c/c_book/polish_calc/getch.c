#include <stdio.h>
#define BUFSIZE 100

static char buf[BUFSIZE];   //static means that only this file can access them
static int bufp = 0;        // also it means they have to be initalised with constants (see page 84)



int getch(void){
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c){
    if(bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
    
}