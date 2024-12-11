#include <stdio.h>
#define IN  1
#define OUT 0


int main(){
    int c, nc, nw, nl, state;
    state = OUT;
    nc = nw = nl = 0;
    while((c = getchar()) != '.'){
        ++nc;
        if(c == '\n')
            nl++;
        if(c == ' ' || c == '\n' || c == '\t')
            state = OUT;
        else if(state == OUT){      //anytime you want to count a comination of ' ' and char use
            nw++;                   //flag with else if
            state = IN;
        }
    }
    printf("%d %d %d\n", nl, nw, nc);
}