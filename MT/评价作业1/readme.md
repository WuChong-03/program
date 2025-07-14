# 評価課題１ Markdown
## playerについて説明
### 回転
    local Position を中心 (0, 0) から設定（矩形の四隅）

    加法定理（cos, sin）を使って Rotated Position を計算

    world Position = centerPosition + Rotated Position によって回転後の座標を画面上に移動

### 移動
    同じスピードで 8 方向移動可能（WASD + 斜め）

    theta によって移動方向をラジアンで決定

    通常移動：playerSpeed = 5.0f

### ダッシュ：
    スペースキーで発動、dashSpeed = 10.0f、20フレーム継続

    移動後は playerRadius による境界判定で画面外に出ないように制限

### 描画
    Novice::DrawQuad を使って回転した四角形（矩形）を描画

    回転後の4頂点（world位置）を使用

---


## enemy初期化（配列とFORで）
### 配列構造
    敵の数：5体（enemyCount = 5）

    配列で位置 (enemyPositionX/Y)、速度 (enemySpeed)、方向 (enemyMoveX/Y)、色 (enemyColor) を個別管理

    色は WHITE, BLUE, RED からランダム選出

### 乱数初期化
    srand(time(NULL)) による乱数シード設定

    位置と速度を rand() でランダムに生成（画面外に出ないよう調整）

### 生死・復活管理
    各enemyに isEnemyAlive スイッチ

    自然死亡：disappearTimer が0になると死亡

    復活：reviveTimer が0になると再生成（位置・速度・色を再ランダム）

    プレイヤーとの衝突時：距離を計算 → 当たったらスコア加算＆死亡

    WHITE: +10点

    BLUE: +100点

    RED: +1000点

---

## enemy挙動について

    enemySpeed × enemyMoveX/Y によって移動

    毎フレーム座標更新

### 反射（画面の境界判定）
    画面の端（x: 0–1280, y: 0–720）に当たると、進行方向を反転

    enemyPosition >= 画面端 ± radius で検出し、enemyMove *= -1

### 描画
    Novice::DrawEllipse で円（敵）を描画

    isEnemyAlive == true の敵のみ描画される

---

## UI・デバッグ出力
    Novice::ScreenPrintf で画面上に各種情報を出力

    スコア

    プレイヤーの座標・速度・当たり判定半径

    ダッシュタイマー状態

    各enemyの位置・移動方向・色・生死状態・タイマーなど

