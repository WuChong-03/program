#include <stdio.h>

int main() {
    int state = 0;

    // 各状態の番号を定義
    const int SLEEP = 1 << 1;   // 0001（眠り状態）
    const int STONE = 1 << 2;   // 0010（石化状態）
    const int POISON = 1 << 3;  // 0100（毒状態）
    const int DEAD = 1 << 4;    // 1000（戦闘不能状態）

    state = POISON | SLEEP;

    printf("state = %d\n", state);
    if (state & POISON) {
        printf("毒状態\n");
    }
    if (state & SLEEP) {
        printf("眠り状態\n");
    }
    if (state & STONE) {
        printf("石化状態\n");
    }
    if (state & DEAD) {
        printf("戦闘不能状態\n");
    }

    return 0;
}
