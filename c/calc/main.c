#include <stdio.h>
#include "funcs.h"

// int atoi(char a[]);

int main(void){
	int i;
	char a[16],b[16];
	int con_a = 0,con_b = 0,ans;
	char op, c;

	printf("please enter a number:");
	for(i = 0;(c = getchar()) != '\n';i++)
		if(c >= '0' && c <= '9')
			a[i] = c;
	con_a = atoi(a);
	printf("you entered: %d\n\n",con_a);


	printf("please enter an operation:");
	op = getchar();
	printf("you entered op:%c\n",op);
	getchar();		//clear \n from inuput buffer
	

	printf("please enter a second number:");
	for(i = 0;(c = getchar()) != '\n';i++)
		if(c >= '0' && c <= '9')
			b[i] = c;
	con_b = atoi(b);
	printf("you entered for B:%d\n",con_b);
	
	switch(op){
	case '+':
		ans = con_a + con_b;
		printf("%d + %d = %d\n",con_a,con_b,ans);
		break;
	case '-':
		ans = con_a - con_b;
		printf("%d - %d = %d\n",con_a,con_b,ans);
		break;
	}
	return 0;
}
/*
int atoi(char a[]){
	int n = 0,i;
	for(i = 0 ; a[i] >= '0' && a[i] <= '9' ; i++)
		n = n*10 + (a[i] - '0');
	return n;
}
*/
