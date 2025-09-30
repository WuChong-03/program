#include <stdio.h>

int Power(int a) {
	return a * 2;
}
float Power(float a) {
	return a * 2;
}
int main() {
	printf("引数int Power関数 = %d\n", Power(2));
	printf("引数float Power関数 = %f", Power(2.0f));
    return 0;
}


