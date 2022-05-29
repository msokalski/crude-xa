#define _CRT_SECURE_NO_WARNINGS // ms spam

#include <assert.h>
#include <float.h>
#include <string.h>
#include "src/crude-xa.h"

// make types names shorter


/////////////////////////////////////////////////////////////////////////////
#include <random>
#include <chrono>

// fix older cpp
#ifndef DBL_DECIMAL_DIG
#define DBL_DECIMAL_DIG DBL_DIG
#endif

#ifndef LDBL_DECIMAL_DIG
#define LDBL_DECIMAL_DIG LDBL_DIG
#endif

void check(const char *desc, long double v1, long double v2, long double r1, long double r2)
{
	if (fabs(v1 - v2) > fabs(v1 * pow(2.0, -63)))
	{
		printf("%s(%Lf,%Lf): \n%Lf vs %Lf\n", desc, r1, r2, v1, v2);
		// exit(0);
	}
}

#include <math.h>
#include <float.h>

int main()
{
	struct PERF
	{
		typedef std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;

		PERF(const char* desc)
		{
			str = desc ? desc : "";
			sum = 0;
		}

		void begin()
		{
			ref = std::chrono::high_resolution_clock::now();
		}

		void end()
		{
			timestamp end = std::chrono::high_resolution_clock::now();
			int64_t nanos = (end - ref).count();
			ref = end;
			sum += nanos / 1000;
		}


		void print()
		{
			std::cout << str << ": " << sum << "µs\n";
		}

		timestamp ref;
		int64_t sum;
		const char* str;
	};

	int num = 100000;

	/*
	PERF f0("warmup");
	int acc = 1;
	char* prv = 0;
	f0.begin();
	for (int i=2; i<num/120; i++)
	{
		acc += i;
		char* xxx = (char*)malloc(acc);
		free(prv);
		prv = xxx;
	}
	free(prv);
	f0.end();
	printf("ACC=%d\n",acc);
	*/

	for (int test=0; test<1; test++)
	{
		PERF f1("factorial1");
		PERF d1("factorial1 dec");
		PERF f2("factorial2");
		PERF d2("factorial2 dec");
		PERF f3("factorial3");
		PERF d3("factorial3 dec");

		{
			f1.begin();
			XA_REF factorial = 1;
			XA_REF number = num;
			for (XA_REF i=2; i<=number; i++)
				factorial *= i;
			f1.end();
			d1.begin();
			std::cout << std::dec << (factorial) << std::endl;
			d1.end();
		}	

		{
			f2.begin();
			XA_VAL* factorial = xa_load(1);
			PERF $("factorial2");
			for (int i=2; i<=num; i++)
			{
				XA_VAL* n = xa_load(i);
				XA_VAL* f = xa_mul(n,factorial);
				xa_free(n);
				xa_free(factorial);
				factorial = f;
			}
			f2.end();
			d2.begin();
			char* str;
			int factorial_dec_digits = xa_extr_dec(factorial,&str);
			printf("%d! (%d digits) = \n%s\n", num, factorial_dec_digits, str);
			free(str);
			d2.end();
			xa_free(factorial);
		}

		{
			f3.begin();
			XA_VAL* factorial = xa_load(1);
			XA_VAL* one = xa_load(1);
			XA_VAL* i = xa_load(2);
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
			f3.end();

			d3.begin();
			char* str;
			int factorial_dec_digits = xa_extr_dec(factorial,&str);
			printf("%d! (%d digits) = \n%s\n", num, factorial_dec_digits, str);
			free(str);
			d3.end();

			xa_free(factorial);
		}

		f1.print();
		d1.print();
		f2.print();
		d2.print();
		f3.print();
		d3.print();
	}

	//f0.print();
	int bytes, allocs = xa_leaks(&bytes);
	printf("LEAKED: %d bytes in %d allocs\n", bytes, allocs);
	exit(0);


	std::random_device rd{};
	std::mt19937_64 gen{rd()};

	// values near the mean are the most likely
	// standard deviation affects the dispersion of generated values from the mean
	
	//std::normal_distribution<long double> d{0.0, 2.0};
	std::gamma_distribution<long double> d(0.01L,1.0L);

#if 0
	long double big = 1000.0L;
	XA_VAL* xa_big = xa_load(big);
	long double c_big = xa_extr(xa_big);

	long double small = 0x1P-80L; // decimal exponent but 
	XA_VAL* xa_small = xa_load(small);
	long double c_small = xa_extr(xa_small);

	long double sum = big + small;
	XA_VAL* xa_sum = xa_add(xa_big,xa_small);
	long double c_sum = xa_extr(xa_sum);

	long double should_be_small_not_zero = sum - big;
	XA_VAL* xa_result = xa_sub(xa_sum,xa_big);
	long double c_result = xa_extr(xa_result); // SUPERIORITY PROOVED!

	assert( xa_cmp(xa_small,xa_result) == 0 ); // OK :)

	// assert( should_be_small_not_zero == small ); // FAIL

	xa_free(xa_big);
	xa_free(xa_small);
	xa_free(xa_sum);
	xa_free(xa_result);
#endif

	uint64_t digits_accum = 0;
	uint64_t abs_exp_accum = 0;
	int accums = 0;

	int pass = 0, fail = 0;
	for (; pass < 100000000; pass++)
	{
		long double r1 = d(gen) * 1.E+15L;
		long double r2 = d(gen) * 1.E+15L;

		// char s1[60];
		// char s2[60];

		// sprintf(s1,"%.50Lf", r1);
		// sprintf(s2,"%.50Lf", r2);

		XA_VAL* v1 = xa_load(r1);
		XA_VAL* v2 = xa_load(r2);

		digits_accum += v1->digs + v2->digs;
		abs_exp_accum += std::max(v1->quot+1,(int)v1->digs-(v1->quot+1)) + std::max(v2->quot+1, (int)v2->digs-(v2->quot+1));
		accums += 2;

		long double c1 = xa_extr(v1);
		long double c2 = xa_extr(v2);
		assert(r1 == c1);
		assert(r2 == c2);

		char *dec = 0;
		int declen = xa_extr_dec(v1, &dec);
		// printf("%s\n", dec);

		long double scan;
		sscanf(dec, "%Lf", &scan);
		if (scan != r1)
		{
			printf("--- PROBLEM ---\n");
			printf("%s\n", dec);
			printf("%.*Le\n", LDBL_DECIMAL_DIG, r1);
			printf("---------------\n");



			free(dec);
			declen = xa_extr_dec(v1, &dec);
			sscanf(dec, "%Lf", &scan);
			if (scan != r1)
			{
				// confirmed
				printf("confirmed.\n");
			}

		}
		else
		{
			// printf("OK: %s\n", dec);
		}

		free(dec);

		// printf("%s , %s   verified.\n", s1,s2);

		// xa_add
		{
			XA_VAL* v12 = xa_add(v1, v2);
			XA_VAL* v21 = xa_add(v2, v1);
			long double r12 = r1 + r2;
			long double r21 = r2 + r1;
			long double c12 = xa_extr(v12);
			long double c21 = xa_extr(v21);

			//check("xa_add", r12, c12, r1, r2);
			//check("xa_add", r21, c21, r2, r1);

			assert(r12 == c12);
			assert(r21 == c21);

			xa_free(v12);
			xa_free(v21);
		}

		// xa_sub
		{
			XA_VAL* v12 = xa_sub(v1, v2);
			XA_VAL* v21 = xa_sub(v2, v1);
			long double r12 = r1 - r2;
			long double r21 = r2 - r1;
			long double c12 = xa_extr(v12);
			long double c21 = xa_extr(v21);

			//check("xa_sub", r12, c12, r1, r2);
			//check("xa_sub", r21, c21, r2, r1);

			assert(r12 == c12);
			assert(r21 == c21);

			xa_free(v12);
			xa_free(v21);
		}

		// xa_mul
		{
			XA_VAL* v12 = xa_mul(v1, v2);
			XA_VAL* v21 = xa_mul(v2, v1);
			long double r12 = r1 * r2;
			long double r21 = r2 * r1;
			long double c12 = xa_extr(v12);
			long double c21 = xa_extr(v21);

			// check("xa_mul", r12, c12, r1, r2);
			// check("xa_mul", r21, c21, r2, r1);

			assert(r12 == c12);
			assert(r21 == c21);

			xa_free(v12);
			xa_free(v21);
		}

		int c = xa_cmp(v1, v2);
		int d = r1 < r2 ? -1 : r1 > r2 ? +1 : 0;

		assert(c == d);

		xa_free(v1);
		xa_free(v2);
	}

	printf("%d passes done, %d failed.\n", pass, fail);

	return 0;
}
