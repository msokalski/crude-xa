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

