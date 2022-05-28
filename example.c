#include "src/crude-xa.h"
#include <stdio.h>
#include <stdlib.h>

XA_VAL* CalcFactorial(unsigned num)
{
    XA_VAL* factorial = xa_load(1.0);
    XA_VAL* one = xa_load(1.0);
    XA_VAL* i = xa_load(2.0);
    XA_VAL* n = xa_load(num);
    while (xa_cmp(i,n)<=0)
    {
        XA_VAL* f = xa_mul(i,factorial);
        xa_free(factorial);
        factorial = f;

        XA_VAL* j = xa_add(i,one);
        xa_free(i);
        i = j;
    }
    xa_free(n);
    xa_free(i);
    xa_free(one);
    return factorial;
}

int main()
{
    XA_VAL* f = CalcFactorial(100000);
    char* str;
    int len = xa_extr_dec(f,&str);
    xa_free(f);
    printf("%s\n",str);
    free(str);
    return 0;
}

