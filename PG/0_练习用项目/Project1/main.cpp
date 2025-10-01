#include <stdio.h>
#include "calc.h"

int main() {
	int a = 6, b = 10;
	float x = 5.5f, y = 10.5f;
	printf("Sum of %d and %d is %d\n", a, b, Power(a, b));
	printf("Sum of %.2f and %.2f is %.2f\n", x, y, Power(x, y));
	return 0;
}