# crude-xa
## Crude Exact Arithmetic in C/C++ 

### Using C++ interface
```C++
#include "src/crude-xa.h"

XA_REF CalcFactorial(unsigned num)
{
    XA_REF factorial = 1.0;
    XA_REF number = num;
    for (XA_REF i=2.0; i<=number; i++)
        factorial *= i;
    return factorial;
}

int main()
{
    XA_REF f = CalcFactorial(100000);
    std::cout << std::dec << f << std::endl;
    return 0;
}

// build:
// g++ example.cpp src/crude-xa.c -o example_cpp
```

### Using C interface (not so pretty)
```C
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

// build:
// gcc example.c src/crude-xa.c -o example_c
```
