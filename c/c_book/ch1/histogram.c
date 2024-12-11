#include <stdio.h>
#define IN  1
#define OUT 0

int main(){
    int c, nc, i, j, state;
    int nw[10];
    
    c = nc = i = j = 0;
    for(i = 0; i < 10;i++)
        nw[i] = 0;

    state = OUT;

    while((c = getchar()) != '.'){

        if(c == ' ' || c == '\n' || c == '\t')
            state = OUT;
        else if(state == OUT){
            state = IN;
            nw[nc]++;
            nc = 0;
        }
        if(c >= 'A' && c <= 'z'){
            nc++;
        }
    }


    printf("\nword lenght histogram:\n");
    for(i = 0; i <10; i++){
        printf("%d:",i);
        for(j= 0; j < nw[i]; j++)
            printf("|");
        printf("\n");
    }
    printf("\n");

}