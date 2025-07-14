#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "AL1_Debug";
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

typedef struct Vector2 {
  float x;
  float y;
} Vector2;

typedef struct Player {
  Vector2 pos;
  float width;
  float height;
  float speed;
  int shootCoolTime;
} Player;

typedef struct Bullet {
  Vector2 pos;
  float width;
  float height;
  float radius;
  float speed;
  int isShoot;
} Bullet;

typedef struct Enemy {
  Vector2 pos;
  float width;
  float height;
  float radius;
  float speed;
  int isAlive;
  int respawnTimer;
} Enemy;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  // ライブラリの初期化
  Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

  // キー入力結果を受け取る箱
  char keys[256] = {0};
  char preKeys[256] = {0};

  //================================================================
  // 変数と初期化
  //================================================================

  // 自機の変数
  Player player;
  player.pos.x = kWindowWidth / 2.0f;
  player.pos.y = kWindowHeight / 2.0f + 256.0f;
  player.width = 32.0f;
  player.height = 32.0f;
  player.speed = 4.0f;
  player.shootCoolTime = 10;

  // 弾の変数
  Bullet bullet[32];
  for (int i = 0; i < 32; i++) {
    bullet[i].pos.x = -128.0f;
    bullet[i].pos.y = -128.0f;
    bullet[i].width = 8.0f;
    bullet[i].height = 16.0f;
    bullet[i].radius = 8.0f;
    bullet[i].speed = 8.0f;
    bullet[i].isShoot = false;
  }

  // 敵の変数
  Enemy enemy[8];
  for (int i = 0; i < 8; i++) {
    enemy[i].pos.x = 128.0f + i * 64.0f;
    enemy[i].pos.y = 32.0f + i * 64.0f;
    enemy[i].width = 32.0f;
    enemy[i].height = 32.0f;
    enemy[i].radius = 16.0f;
    enemy[i].speed = 2.0f;
    enemy[i].isAlive = true;
    enemy[i].respawnTimer = 120;
  }

  // ウィンドウの×ボタンが押されるまでループ
  while (Novice::ProcessMessage() == 0) {
    // フレームの開始
    Novice::BeginFrame();

    // キー入力を受け取る
    memcpy(preKeys, keys, 256);
    Novice::GetHitKeyStateAll(keys);

    ///
    /// ↓更新処理ここから
    ///

    //================================================================
    // 自機の更新処理
    //================================================================

    // 自機の移動処理
    if (keys[DIK_W]) {
      if (player.pos.y > 0 + player.height / 2.0f) {
        player.pos.y -= player.speed;
      }
    }

    if (keys[DIK_S]) {
      if (player.pos.y < kWindowHeight - player.height / 2.0f) {
        player.pos.y += player.speed;
      }
    }

    if (keys[DIK_A]) {
      if (player.pos.x > 0 + player.width / 2.0f) {
        player.pos.x -= player.speed;
      }
    }

    if (keys[DIK_D]) {
      if (player.pos.x < kWindowWidth - player.width / 2.0f) {
        player.pos.x += player.speed;
      }
    }

    //================================================================
    // 弾の更新処理
    //================================================================

    // 弾の発射処理
    if (keys[DIK_SPACE]) {

      // 発射間隔の調整用クールタイムの計算
      if (player.shootCoolTime > 0) {
        player.shootCoolTime--;
      } else {
        player.shootCoolTime = 10;
      }

      // i番目の弾が撃たれていなかったら発射する
      if (player.shootCoolTime <= 0) {
        for (int i = 0; i < 32   ; i++) {
          if (!bullet[i].isShoot) {
            bullet[i].isShoot = true;
            bullet[i].pos.x = player.pos.x;
            bullet[i].pos.y = player.pos.y;
            break;
          }
        }
      }
    }



    // 弾道計算
    for (int i = 0; i < 32; i++) {
      if (bullet[i].isShoot) {
        // 上方向に進ませる
        bullet[i].pos.y -= bullet[i].speed;

        // 画面外に出たら発射フラグをFalseに変更する
        if (bullet[i].pos.y <= 0 - bullet[i].height / 2.0f) {
          bullet[i].isShoot = false;
        }
      }
    }

    //================================================================
    // 敵の更新処理
    //================================================================

    for (int i = 0; i < 8; i++) {
      if (enemy[i].isAlive) {
        enemy[i].pos.x += enemy[i].speed;

        // 壁際で反射
        if (enemy[i].pos.x <= 0 + enemy[i].width / 2.0f ||
            enemy[i].pos.x >= kWindowWidth - enemy[i].width / 2.0f) {
          enemy[i].speed *= -1.0f;
        }   
      } else {
        if (enemy[i].respawnTimer > 0) {
          enemy[i].respawnTimer--;
        } else {
          enemy[i].respawnTimer = 0;
          enemy[i].isAlive = true;
        }
      }
    }

    //================================================================
    // 当たり判定
    //================================================================

    for (int i = 0; i < 8; i++) {
      if (enemy[i].isAlive) {
        for (int j = 0; j < 32; j++) {
          if (bullet[j].isShoot) {
            if (enemy[i].radius + bullet[j].radius >=
                sqrtf(static_cast<float>(
                    pow(enemy[i].pos.x - bullet[j].pos.x, 2) +
                    pow(enemy[i].pos.y - bullet[j].pos.y, 2)))) {
               enemy[j].isAlive = false;
              bullet[i].isShoot = false;
            }
          }
        }
      }
    }
    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///

    //================================================================
    // 敵の描画処理
    //================================================================
    for (int i = 0; i < 8; i++) {
      if (enemy[i].isAlive) {
        Novice::DrawBox(
            static_cast<int>(enemy[i].pos.x - enemy[i].width / 2.0f),
            static_cast<int>(enemy[i].pos.y - enemy[i].height / 2.0f),
            static_cast<int>(enemy[i].width), static_cast<int>(enemy[i].height),
            0.0f, 0xFFDDDDFF, kFillModeSolid);
      } else {
        if (enemy[i].respawnTimer >= 90) {
          if (enemy[i].respawnTimer % 2 == 0) {
          Novice::DrawBox(
              static_cast<int>(enemy[i].pos.x - enemy[i].width / 2.0f),
              static_cast<int>(enemy[i].pos.y - enemy[i].height / 2.0f),
              static_cast<int>(enemy[i].width),
              static_cast<int>(enemy[i].height), 0.0f, 0xFFDDDDFF,
              kFillModeSolid);
          }
        }
      }
    }

    //================================================================
    // 弾の描画処理
    //================================================================
    for (int i = 0; i < 32; i++) {
      if (bullet[i].isShoot) {
        Novice::DrawTriangle(
            static_cast<int>(bullet[i].pos.x),
            static_cast<int>(bullet[i].pos.y - bullet[i].height / 2.0f),
            static_cast<int>(bullet[i].pos.x - bullet[i].width / 2.0f),
            static_cast<int>(bullet[i].pos.y + bullet[i].height / 2.0f),
            static_cast<int>(bullet[i].pos.x + bullet[i].width / 2.0f),
            static_cast<int>(bullet[i].pos.y + bullet[i].height / 2.0f),
            0xFFFFFFFF, kFillModeSolid);
      }
    }

    //================================================================
    // 自機の描画処理
    //================================================================
    Novice::DrawBox(static_cast<int>(player.pos.x - player.width / 2.0f),
                    static_cast<int>(player.pos.y - player.height / 2.0f),
                    static_cast<int>(player.width),
                    static_cast<int>(player.height), 0.0f, 0xDDDDFFFF,
                    kFillModeSolid);

    ///
    /// ↑描画処理ここまで
    ///

    // フレームの終了
    Novice::EndFrame();

    // ESCキーが押されたらループを抜ける
    if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
      break;
    }
  }

  // ライブラリの終了
  Novice::Finalize();
  return 0;
}
