int atoi(char a[]){
        int n = 0,i;
        for(i = 0 ; a[i] >= '0' && a[i] <= '9' ; i++)
                n = n*10 + (a[i] - '0');
        return n;

}
