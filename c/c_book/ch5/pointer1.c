#include <stdio.h>

void swap(int *a,int *b);
void swapArray(int c[],int d[]);

int main(void){
    int i , p;
    int ai[4] = {1,2,3,4},ap[4] = {4,3,2,1};
    i = 5;
    p = 10;
    printf("i:%d p:%d\n",i,p);
    swap(&i,&p);
    printf("i:%d p:%d\n",i,p);
    for(int j=0; j<4;j++)
        printf("%d ",ai[j]);
    printf("\n");
    for(int j=0; j<4;j++)
        printf("%d ",ap[j]);

    printf("\n------\n");
    swapArray(ai,ap);

    for(int j=0; j<4;j++)
        printf("%d ",ai[j]);
    printf("\n");
    for(int j=0; j<4;j++)
        printf("%d ",ap[j]);
    printf("\n");
}

void swap(int *x, int *y){
    int tmp;
    tmp = *x;
    *x = *y;
    *y = tmp;
    
}
void swapArray(int c[], int d[]){
    int tmp;
    for(int i = 0; i<4 ;i++){
        tmp  = c[i];
        c[i] = d[i];
        d[i] = tmp;
    }
    
}