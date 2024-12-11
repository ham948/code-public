#include <stdio.h>
#include <string.h>
#define MAXLEN 1000
#define ALLOCSIZE 10000

int getline1(char *, int);
char *alloc1(int);

static char allocbuf[ALLOCSIZE];
static char *allocp = allocbuf;

int readlines(char *lineptr[], int maxlines){
    int len, nlines;
    char *p, line[MAXLEN];

    nlines = 0;
    while((len = getline1(line,MAXLEN)) > 0)
        if(nlines >= maxlines || (p = alloc1(len)) == NULL)
            return -1;
        else{
            line[len-1] = '\0';
            strcpy(p, line);
            lineptr[nlines++] = p;
        }
    return nlines;
}

void writelines(char *lineptr[],int nlines){
    int i;

    for(i = 0; i < nlines; i++)
        printf("%s\n", lineptr[i]);
}

char *alloc1(int n){
    if(allocbuf + ALLOCSIZE - allocp >= n){
        allocp += n;
        return allocp - n;
    } else
        return 0;
}

int getline1(char s[], int lim){
  int c, i;
  i = 0;
  while(--lim > 0 && (c = getchar()) != EOF && c != '\n')
    s[i++] = c;
  if(c == '\n')
    s[i++] = c;
  s[i] = '\0';
  return i;
  }