#include <stdio.h>

void Add(int* a)
{
	*a += 5;
	printf("%d\n", *a);     //①
}

int main()
{
	int a = 10;
	printf("%d\n", a);     //②
	Add(&a);
	printf("%d\n", a);     //③
	return 0;
}