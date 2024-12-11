#include <stdio.h>

int main(void){
    int i, j, c, line[1000];
    i = 0;
    j = 0;

    while((c = getchar()) != EOF){
        if(c != '\n')
            line[i++] = c;
        else{
            line[i] = '\0';
             if(i > 20)
                while(j<i){
                    printf("%c",line[j]);
                    j++;
                }
                printf("\n");
                i = j = 0;
        }
    
    
    
    }
}