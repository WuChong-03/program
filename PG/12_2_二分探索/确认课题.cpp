#include <stdio.h>
int main() {
	//検索対象の配列の宣言、初期化
	int arr[100];	
	for (int i = 0; i < 100; i++){
		arr[i] = i + 1;
	}
	//検索する値の宣言、初期化
	int seachValue;
	//最小、最大要素、中間要素値のindex（添え字）の宣言、初期化
	int size = sizeof(arr) / sizeof(arr[0]);
	int low = 0, high = size - 1 , mid;
	//大きいか小さいかのフラグ
	int input = 0;

	printf("思い浮かべた数字(1~100)を入力してください\n");
	scanf_s("%d", &seachValue);


	while (low <= high) {
		mid = (low + high) / 2;

		if (arr[mid] == seachValue){
			printf("思い浮かべた数字は%dですね", arr[mid]);
			break;
		}
		printf("思い浮かべた数字は%dより大きいですか？(1:大きい, ０:小さい)\n", arr[mid]);
		scanf_s("%d", &input);

		if (arr[mid] < seachValue && input == 1 ){
			low = mid + 1;
		}
		if (arr[mid] > seachValue && input == 0){
			high = mid - 1;
		}
	}
    return 0;
}