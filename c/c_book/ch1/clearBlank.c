#include <stdio.h>
#define IN  1
#define OUT 0

int main(void){
    int i, j, c, state;
    int line[1000];
    i = 0;
    state = OUT;

    while((c = getchar()) != EOF){
        while((c == ' ' || c == '\t') && state == OUT)
            c = getchar();
        state = IN;
        line[i++] = c;
        if(c == ' ' || c == '\t')
            state = OUT;
        if(c == EOF)
            line[i] = '\0';
    }
    while(j < i){
        printf("%c",line[j]);
        j++;
    }
}