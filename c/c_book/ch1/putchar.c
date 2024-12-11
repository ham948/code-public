#include <stdio.h>




/* chapter 1 ex 10
int main(void){
    int c;
    while((c= getchar()) != '\n'){
        if(c == '\t'){
            putchar('\\');
            putchar('t');
        }
        else if(c == '\\'){
            putchar('\\');
            putchar('\\');
        }
        else
            putchar(c);
    }
}
*/


/* chapter 1 ex 9
int main(void){
    int c;
    while((c = getchar()) != '\n'){
        if(c == ' '){);
        }
            putchar(c);
            while((c = getchar()) == ' ');
        }
        putchar(c);
    }
    printf("\n");
}
*/

/*   chapter 1 ex 8
int main(void){
    int c, nl=0,nt=0,nb=0;
    while((c = getchar()) != '.'){
        if(c == '\n')
            nl++;
        if(c == '\t')
            nt++;
        if(c == ' ')
            nb++;
    }
    printf("no lines     :%d\n",nl);
    printf("no tabs      :%d\n",nt);
    printf("no blanks    :%d\n",nb);
}
*/