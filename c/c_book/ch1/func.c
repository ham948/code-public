#include <stdio.h>

int power(int, int);

int main(void){
    printf("numbers to power");
    int a, b, c;
    a = 3;
    b =2;
    c = power(3,2);
    printf("answer is:%d\n",c);
}

int power(int base, int expon){
    int i, ans;
    ans = 1;
    for(i = 1; i <= expon; i++){
        ans = base*ans;
    }

    return ans;
} 