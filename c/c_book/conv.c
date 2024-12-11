#include <string.h>
#include <stdio.h>


void reverse(char s[]){
    int c, i, j;

    for(i = 0,j = strlen(s) - 1; i<j; i++,j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa1(int n, char* s){
    int i, sign;
    
    if((sign = n) < 0)
        n = -n;
    i = 0;
    do{
        *(s+i++) = n % 10 + '0';
    }while((n /= 10) > 0);
    if(sign < 0)
        *(s+i++) = '-';
    *(s+i) = '\0';
    reverse(s);
}

int main(void){
    char ch[6];
    int n = 3776;
    itoa1(n,&ch);
    printf("string:%s\n\n",ch);
    return 0;
}