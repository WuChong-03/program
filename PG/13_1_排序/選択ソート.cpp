#include <stdio.h>
int main() {
	int arr[] = {7,5,6,3,4};
	int size = sizeof(arr) / sizeof(arr[0]);
	printf("選択ソートまえの配列");
	for (int i = 0; i < size; i++)
	{
		printf("%d ", arr[i]);
	}
	//選択ソートを実行
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = size - 1 ; j > i; j--)
		{
			if (arr[j - 1] > arr[j])
			{
				int temp = arr[j - 1];
				arr[j - 1] = arr[j];
				arr[j] = temp;
			}
		}
	}


	printf("\n選択ソート後の配列");
	for (int i = 0; i < size; i++)
	{
		printf("%d ", arr[i]);
	}
	return 0;
}
