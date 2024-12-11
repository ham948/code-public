#include <stdio.h>

int main(){
    int c, i, nw, no;
    int nd[10];
    for(i = 0;i<10;i++)
        nd[i] = 0;
    nw = no = 0;

    while((c = getchar()) != '.'){
        if(c >= '0' && c<= '9')
            nd[c-'0']++;
        else if(c == ' ' || c == '\n' || c == '\t')
            nw++;
        else
            no++;    
    }
    printf("digits: ");
    for(i = 0;i<10;i++)
        printf("%d ",nd[i]);
    printf("\nwhite space: %d other: %d",nw,no);

}