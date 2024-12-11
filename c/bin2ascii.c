#include    <stdio.h>

void itoa2(unsigned char n_h, unsigned char n_l, unsigned char s[]);
void reverse(unsigned char s[]);

int main(void){
    unsigned char H, L;
    unsigned char con[] = "00000";
    H = 0B00000001;
    L = 0B11111100;
    itoa2(H,L,con);
    printf("\ncon:%c%c%c%c%c\n",con[0],con[1],con[2],con[3],con[4]);

}

void itoa2(unsigned char n_h, unsigned char n_l, unsigned char s[]){
    int i = 0;
    int h = 0;
    h = (n_h<<8) | n_l;
    printf("current number:%d\n",h);
    do{
        s[i++] = h % 10 + '0';
    }while((h /= 10) > 0);
    while(i < 6){
        s[i++] = '0';
    }
    s[i] = '\0';
    printf("current str:%s",s);
    reverse(s);
}

void reverse(unsigned char s[]){
    int c, i, j;

    for(i = 0, j = 4; i < j; i++, j--){
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}