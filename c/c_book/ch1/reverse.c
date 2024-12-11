#include <stdio.h>

void reverse(char in[],int len);

int main(void){
    int c, i, j;
    char line[1000];
    i = j = 0;
    while((c = getchar()) != '\n')
        line[i++] = c;
    line[i] = '\0';

    reverse(line,i);
    while(j<=i)
        printf("%c",line[j++]);

}

void reverse(char in[], int len){
    int i, j, tmp;
    for(i = 0, j = len;i < j; i++, j--){
        tmp = in[i];
        in[i] = in[j];
        in[j] = tmp;

    }
}