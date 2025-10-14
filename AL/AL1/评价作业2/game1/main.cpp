/****************************************************
 * Programming Survivor — 改造版 Part A
 * 变更点（本段已实现）：
 * 1) 重开问题修复：boss 触发/计时改全局，ResetGame 全量清理
 * 2) 数值体系 ×10：敌人 HP、武器伤害全面提档
 * 3) 敌人速度恒定；后期刷怪上限略降
 * 4) 武器：
 *    - Garlic（大蒜）新增：环形光环，有冷却（Lv1=60f → Lv8=18f）
 *    - King Bible（国王圣经）：数量+半径+角速度都随等级成长
 *    - Clock Lancet（柳叶刀）：长度/宽度/束数随等级成长，范围 Buff 生效
 *    - Magic Wand（魔杖=原 Tracking Dagger）：碰撞半径受范围 Buff
 *    - Bullet/Lightning 等也吃范围 Buff（半径/范围/长度）
 * 5) 范围 Buff 真实生效到各武器的“半径/长度/碰撞体积”
 * 6) 为 Part B 预留 Boss 强化与完整 Update/Draw
 ****************************************************/

#define NOMINMAX
#include <random>
#include <Novice.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>

constexpr char  kWindowTitle[] = "Programming Survivor";
constexpr int   kWindowWidth = 1280;
constexpr int   kWindowHeight = 720;
constexpr float kMapWidth = 4000.0f;
constexpr float kMapHeight = 4000.0f;

enum Scene { SCENE_TITLE = 0, SCENE_GAME, SCENE_RESULT };
char keys[256]{}, preKeys[256]{};

static inline bool CircleHit(float x1, float y1, float r1, float x2, float y2, float r2) {
	float dx = x1 - x2, dy = y1 - y2;
	float d2 = dx * dx + dy * dy;
	float rr = (r1 + r2) * (r1 + r2);
	return d2 <= rr;
}
static inline void AdvanceOutOfCircleAlongVelocity(
	float& bx, float& by, float& bvx, float& bvy, float br,
	float cx, float cy, float targetR)
{
	// 把 (bx,by) 这颗带半径 br 的投射物，沿自身速度(bvx,bvy)方向，
	// 推到与以(cx,cy)半径 targetR 的圆“刚好不相交”的位置，并再多推一点点（1.5像素）
	float vx = bvx, vy = bvy;
	float vlen = std::sqrt(vx * vx + vy * vy);
	if (vlen < 1e-5f) {
		float dx = bx - cx, dy = by - cy;
		float d = std::sqrt(dx * dx + dy * dy);
		float R = br + targetR;
		if (d < 1e-5f) { dx = 1.0f; dy = 0.0f; d = 1.0f; }
		float need = (R - d) + 1.5f;
		bx += (dx / d) * need;
		by += (dy / d) * need;
		return;
	}
	float iv = 1.0f / vlen;
	float ux = vx * iv, uy = vy * iv;
	float mx = bx - cx, my = by - cy;
	float R = br + targetR;

	float dot = ux * mx + uy * my;
	float c = mx * mx + my * my - R * R;
	float disc = dot * dot - c;
	if (disc < 0) disc = 0;
	float t_exit = -dot + std::sqrt(disc);

	float eps = 1.5f; // 安全余量
	bx += ux * (t_exit + eps);
	by += uy * (t_exit + eps);
}

static inline float ClampF(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
static inline float Lerp(float a, float b, float t) { if (t < 0)t = 0; if (t > 1)t = 1; return a + (b - a) * t; }
static inline bool InRect(float x, float y, float rx, float ry, float rw, float rh) { return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh; }

struct Player {
	float x, y, r;
	int hp, maxHP;
	float baseSpeed;
	float lastDirX, lastDirY;
	// --- 受击无敌帧（新增） ---
	int hurtIFrames;   // >0 时本帧不再掉血
};

enum EnemyTier { ET_T1 = 0, ET_T2, ET_T3, ET_T4, ET_T5, ET_COUNT };
struct Enemy {
	float x, y, r, speed;
	int hp, maxHP;
	EnemyTier tier;
	bool active;
	int slowTimer = 0, freezeTimer = 0;
	float slowFactor = 1.0f;
};

struct Bullet {
	float x, y, r, vx, vy;
	int   dmg;
	int   pierce;
	bool  active;

	int   lastHitId;        // 最近命中的敌人索引（-1=无，-9999=Boss）
	int   lastHitCooldown;  // 免疫帧数（防抖）
};

struct ExpOrb { float x, y, r; int value; bool active, attracted; };

struct DaggerProj {
	float x, y, vx, vy, r;
	int   dmg, life, pierce;
	bool  active;

	int   lastHitId;        // 最近命中的敌人索引（-1=无，-9999=Boss）
	int   lastHitCooldown;  // 免疫帧数（防抖）
};

struct LightningStrike { float tx, ty; int life, dmg; float radius; bool active; };
struct Mine { float x, y, r; int fuse; float triggerR; int dmg; bool active; };
struct Explosion { float x, y, r; int life, dmg; bool active; };

struct LaserBeam { // Clock Lancet（柳叶刀）射线
	float angle, len, halfWidth;
	int life, dmg, beamsId; // beamsId 仅用于在 UI 显示/调试
	bool active;
};

struct IceCone { // 已不再作为柳叶刀使用，可保留其他设计（如冻结扇形）——本版仅保留结构以兼容
	float angle, len, halfWidth;
	int life, lifeTotal, dps;
	float slowMul;
	bool canFreeze;
	bool active;
};

// === 新增：Garlic（大蒜）光环（有冷却的周期伤害） ===
struct GarlicAura {
	float radius;         // 光环半径（吃范围 Buff）
	int   tickCDFrames;   // 本次冷却（随等级缩短）
	int   timer;          // 计时器
	int   dmg;            // 每跳伤害（×10制）
	bool  active;
};

// === King Bible 依旧用“轨道球”方案（数量/半径/角速度成长） ===
struct OrbitalBook {
	// 仅使用参数化生成，不逐一存对象，运行时按公式计算位置
	// 这里留空结构是为了将来做可视化缓存；当前仅用作标记
	bool active;
};

// 额外占位武器
struct Rocket { float x, y, vx, vy, r; int dmg, life; bool active; };
constexpr int kMaxRockets = 120;

constexpr int maxEnemies = 900;
constexpr int maxBullets = 700;
constexpr int maxOrbs = 1400;

constexpr int kMaxDaggers = 200;
constexpr int kMaxStrikes = 120;
constexpr int kMaxMines = 120;
constexpr int kMaxExplos = 120;
constexpr int kMaxLasers = 64;
constexpr int kMaxIce = 64;

constexpr int kFPS = 60;

// —— 时长与刷怪参数（适度下调上限）——
constexpr int   kGameDurationFrames = 8 * 60 * kFPS; // 8分钟主关
constexpr int   kBossTimeLimit = 60 * kFPS;     // Boss 1分钟限时
constexpr float spawnIntervalStart = 60.0f;
constexpr float spawnIntervalEnd = 8.0f;          // 略放宽间隔
constexpr float spawnCountStart = 1.0f;
constexpr float spawnCountEnd = 5.0f;          // 下调峰值 (原 7)

// —— 速度：玩家恒定，敌人“恒定基准，不随时间翻倍”——
constexpr float playerBaseSpeed0 = 3.0f;
constexpr float enemyBaseSpeed0 = 0.8f; // 比原版略慢，给操作空间

constexpr float kMagnetRadiusMinBase = 60.0f;
constexpr float kMagnetRadiusMaxBase = 180.0f;
constexpr float kMagnetSpeed = 8.0f;

// ===== 全局状态 =====
// [PATCH-HURT-I-FRAME]
int gPlayerHurtCD = 0;           // 玩家受伤后的无敌冷却（帧）
constexpr int kPlayerHurtIFrame = 60; // 0.5秒 @60FPS

bool gInitialPickPending = false;
int   gLevel = 1, gXP = 0, gScore = 0, gEnemyAccum = 0, gGameFrame = 0;
bool  gWin = false, gPaused = false;
float dbg_spawnInterval = spawnIntervalStart;
int   dbg_spawnCount = (int)spawnCountStart;
float dbg_speedScale = 1.0f, dbg_hpScale = 1.0f;

// —— Buff ——（保持不变，只是让范围真的生效）
enum BuffType {
	BUFF_HP = 0, BUFF_SPEED, BUFF_MAGNET, BUFF_COOLDOWN, BUFF_DEFENSE,
	BUFF_CRIT, BUFF_PIERCE, BUFF_AREA, BUFF_XP, BUFF_LIFESTEAL, BUFF_COUNT
};
constexpr int MAX_LV_EACH = 5;
int gBuffLv[BUFF_COUNT]{};
int gHPRegenTimer = 0, gHPRegenCD = 5 * kFPS;

static inline float GetSpeedMul() { static const float p[6] = { 0,0.05f,0.10f,0.15f,0.25f,0.40f }; return 1.0f + p[std::clamp(gBuffLv[BUFF_SPEED], 0, 5)]; }
static inline float GetMagnetBonus() { int lv = gBuffLv[BUFF_MAGNET]; if (lv <= 0)return 0; if (lv == 1)return 20; if (lv == 2)return 40; if (lv == 3)return 80; if (lv == 4)return 140; return 10000; }
static inline float GetCooldownMul() {
	int lv = gBuffLv[BUFF_COOLDOWN];
	float s = (lv >= 1 ? 0.05f : 0) + (lv >= 2 ? 0.05f : 0) + (lv >= 3 ? 0.10f : 0) + (lv >= 4 ? 0.10f : 0) + (lv >= 5 ? 0.15f : 0);
	float m = 1.0f - s; return m < 0.3f ? 0.3f : m;
}
static inline float GetDefenseTakenMul() {
	int lv = gBuffLv[BUFF_DEFENSE];
	float s = (lv >= 1 ? 0.10f : 0) + (lv >= 2 ? 0.10f : 0) + (lv >= 3 ? 0.10f : 0) + (lv >= 4 ? 0.15f : 0) + (lv >= 5 ? 0.20f : 0);
	float m = 1.0f - s; return m < 0.2f ? 0.2f : m;
}
static inline int   GetBonusPierce() { int lv = gBuffLv[BUFF_PIERCE]; return lv <= 0 ? 0 : lv == 1 ? 1 : lv == 2 ? 2 : lv == 3 ? 4 : lv == 4 ? 4 : 7; }
static inline float GetAreaMul() {
	// 每级 +20%，Lv0=1.0, Lv5=2.0（你要的是“升满扩大100%”）
	return 1.0f + 0.2f * std::clamp(gBuffLv[BUFF_AREA], 0, 5);
}
// —— 范围实时倍率工具 ——
// 使用时乘；保证“升了范围，屏幕上立刻变大”
static inline float AreaR(float base) { return base * GetAreaMul(); } // 半径/长度等
static inline float AreaS() { return GetAreaMul(); }                  // 贴图缩放


static inline int ApplyCritToDamage(int base, bool& crit) {
	int lv = gBuffLv[BUFF_CRIT]; crit = false; if (lv <= 0)return base;
	static const int chance[6] = { 0,5,8,12,16,22 };
	static const float amp[6] = { 1,1.5f,1.6f,1.75f,2.0f,2.3f };
	if ((rand() % 100) < chance[lv]) { crit = true; int v = (int)std::ceil(base * amp[lv]); return std::max(v, base + 1); }
	return base;
}
static inline void TryLifestealOnHit(Player& plr) {
	int lv = gBuffLv[BUFF_LIFESTEAL]; if (lv <= 0)return;
	static const int prob[6] = { 0,1,2,3,4,5 };
	if ((rand() % 100) < prob[lv] && plr.hp < plr.maxHP) { plr.hp += 2; if (plr.hp > plr.maxHP)plr.hp = plr.maxHP; }
}

// —— 武器种类 ——
// 维持原顺序基础上新增 GARLIC；ORBITAL=King Bible；LASER=Clock Lancet
enum WeaponType {
	WEAPON_BULLET = 0,   // VS: Knife（飞刀）→ 前射扇形（仍保留）
	WEAPON_MINE,       // 地雷保留
	WEAPON_GARLIC,     // ★新增：Garlic 大蒜（光环）
	WEAPON_DAGGER,     // VS: Magic Wand（魔杖，自动追踪弹）
	WEAPON_LIGHTNING,  // VS: Lightning Ring（同名）
	WEAPON_LASER,      // VS: Clock Lancet（柳叶刀，冻结射线）
	WEAPON_ORBITAL,    // VS: King Bible（绕身旋转）
	WEAPON_ROCKET,     // 仍保留 / 可替换为 VS 另一武器（Part B 可扩展）
	WEAPON_COUNT
};
int gWeaponLv[WEAPON_COUNT]{};
// ===== 上限（开局 4，允许被“稀有选项”提升）=====
int gMaxWeaponCap = 4;   // HUD 第一行可放的武器格数（初始 4）
int gMaxBuffCap = 4;   // HUD 第二行可放的 Buff 格数（初始 4）
const int kCapHardLimit = 12; // 安全硬上限，避免无限增长

const char* WeaponNames[WEAPON_COUNT] = {
  "Knife","Mine","Garlic","MagicWand","Lightning","ClockLancet","KingBible","Rocket"
};
int gCD_Bullet = 0, gCD_Mine = 0, gCD_Garlic = 0, gCD_Dagger = 0, gCD_Lightning = 0, gCD_Laser = 0, gCD_Rocket = 0;

// === 武器参数 ===

// === Bullet（子弹）：冷却/同发数/速度/伤害/穿透/散射角 ===
static inline void GetBulletParams(int lv, int& baseInterval, int& shotNum, float& speedMul, int& dmg, int& pierce, float& spreadDeg) {
	lv = std::clamp(lv, 1, 8);
	static const int   cds[9] = { 0, 26,24,24,22,22,20,20,18 };       // 冷却(帧) ↓ at L2,L4,L6,L8  —— 4次
	static const int   shots[9] = { 0,  1, 2, 3, 4, 5, 6, 7, 8 };       // 同发数 +1 每级（L2~L8） —— 7次
	static const float spd[9] = { 0,1.00f,1.00f,1.15f,1.15f,1.15f,1.30f,1.30f,1.30f }; // 速度 at L3,L6 —— 2次
	static const int   dmgv[9] = { 0, 10,12,12,14,16,16,19,22 };       // 伤害 ×10，小幅：L2,L4,L5,L7,L8 —— 5次
	static const int   pier[9] = { 0,  0, 0, 1, 1, 2, 2, 3, 3 };       // 穿透 +1 at L3,L5,L7 —— 3次
	static const float spr[9] = { 0,  0,10,14,14,18,18,22,26 };       // 散射角(度) at L2,L3,L5,L7,L8 —— 5次
	baseInterval = cds[lv]; shotNum = shots[lv]; speedMul = spd[lv]; dmg = dmgv[lv]; pierce = pier[lv]; spreadDeg = spr[lv];
}
static inline void GetMineParams(int lv, int& cd, int& fuse, float& triggerR, float& radius, int& dmg, int& count) {
	lv = std::clamp(lv, 1, 8);
	static const int   fus[9] = { 0, 60, 60, 60, 60, 60, 60, 60, 60 };
	// 冷却：从 180f → 120f（3秒 → 2秒），增强 5 次
	static const int   cds[9] = { 0,180,170,160,150,140,120,110,100 };
	// 触发半径：从 40 → 80，增强 7 次（对标 Garlic 起点更小，满级≈接近 Garlic）
	static const float trig[9] = { 0, 40,45,50,55,60,65,72,80 };
	// 爆炸半径：从 60 → 100，增强 5 次（比 Garlic 半径略大，但没有超出太多）
	static const float rad[9] = { 0, 60,70,80,90,100,100,100,100 };
	// 伤害：从 15 → 60，增强 7 次（高于 Garlic 的 tick，但不是秒天秒地）
	static const int   dmgv[9] = { 0, 15,20,25,30,40,45,50,60 };
	// 数量：从 1 → 4，增强 3 次
	static const int   cnt[9] = { 0, 1,1,2,2,3,3,4,4 };
	cd = cds[lv]; fuse = fus[lv]; triggerR = trig[lv]; radius = rad[lv]; dmg = dmgv[lv]; count = cnt[lv];
}
// === Garlic（大蒜）：冷却/半径/伤害 ===
static inline void GetGarlicParams(int lv, int& tickCD, float& radius, int& dmg) {
	lv = std::clamp(lv, 1, 8);
	// 冷却 (tickCD)：从 48 帧 → 32 帧（0.8秒 → 0.53秒左右）
	static const int   cd[9] = { 0, 48,46,44,42,40,38,34,32 };
	// 半径：从 45 → 90 （翻倍，比较符合 VS Garlic 成长）
	static const float rad[9] = { 0, 45,50,56,63,70,78,84,90 };
	// 伤害 (×10)：从 8 → 20 （逐级小幅增强）
	static const int   dmgv[9] = { 0,  8,10,12,14,16,17,18,20 };

	tickCD = cd[lv];radius = rad[lv];dmg = dmgv[lv];
}

// === Magic Wand（魔杖）：冷却/数量/速度/伤害/穿透 ===
static inline void GetDaggerParams(int lv, int& cd, int& num, float& speed, int& dmg, int& pierce) {
	lv = std::clamp(lv, 1, 8);
	static const int   cds[9] = { 0, 60,50,40,35,30,24,20,18 };;
	static const float spd[9] = { 0,6.0f,6.6f,7.2f,7.8f,8.6f,9.4f,10.2f,11.0f };
	static const int   dmgv[9] = { 0,5,10,20,30,50,90,120,150 }; // ×10
	static const int   pier[9] = { 0,0,0,0,1,1,2,2,3 };
	static const int   nums[9] = { 0,1,1,2,2,3,3,4,5 };
	cd = cds[lv]; num = nums[lv]; speed = spd[lv]; dmg = dmgv[lv]; pierce = pier[lv];
}
static inline float GetDaggerBaseRadius(int lv) {
	lv = std::clamp(lv, 1, 8);
	static const float dr[9] = { 0, 6, 6, 7, 7, 8, 8.5f, 9.0f, 10.0f };
	return dr[lv];
}

// === Lightning Strike（闪电）：冷却/命中数/伤害/范围（范围 Buff 生效） ===
static inline void GetLightningParams(int lv, int& cd, int& hits, int& dmg, float& radius, int& showFrames) {
	lv = std::clamp(lv, 1, 8);
	static const int   cds[9] = { 0, 180,170,160,150,140,130,120,110 }; // 冷却(帧)：起步 3s，逐级缩短
	static const int   hit[9] = { 0,   1, 2, 3, 4, 5, 6, 7, 8 };        // 同时落雷数量
	static const int   dmgv[9] = { 0,  20,25,30,40,50,60,70,85 };        // 伤害 ×10：稳定逐级增加
	static const float rad[9] = { 0,  28,32,36,40,44,48,52,56 };        // 范围半径：逐渐增大

	cd = cds[lv]; hits = hit[lv]; dmg = dmgv[lv]; radius = rad[lv]; showFrames = 12;
}
// === Clock Lancet（柳叶刀）：更长、更粗、更多束（范围 Buff 生效） ===
static inline void GetLancetParams(int lv, int& cd, float& len, float& halfW, int& life, int& dmg, int& beams) {
	lv = std::clamp(lv, 1, 8);
	// 冷却(帧)：起步 2.0s，逐步提速到 ~1.25s
	static const int   cds[9] = { 0, 120,110,100, 95, 90, 85, 80, 75 };
	// 长度：总增幅 ~20%，满级≈320（屏幕 1/4）
	static const float lens[9] = { 0, 260,270,280,290,300,308,316,324 };
	// 半宽（画面厚度=2×此值）：从 3 → 15（整厚≈6 → 30）
	static const float hw[9] = { 0,   3,  4,  5,  7,  9, 11, 13, 15 };
	// 存在时间(帧)：5 → 20（更容易看清），伤害仍只在首帧结算一次
	static const int   lifeF[9] = { 0,   5,  7,  9, 11, 13, 15, 18, 20 };
	// 伤害×10：温和成长（Lancet 以控场为主，不靠爆伤）
	static const int   dmgv[9] = { 0,  30, 35, 40, 45, 50, 55, 60, 70 };
	// 同时束数：从 1 → 5（4 次 +1）
	// 增长点安排在 L2, L4, L5, L7（共 4 次递增，最终到 5 根）
	static const int   bms[9] = { 0,   1,  2,  2,  3,  4,  4,  5,  5 };
	cd = cds[lv]; len = lens[lv]; halfW = hw[lv]; life = lifeF[lv]; dmg = dmgv[lv]; beams = bms[lv];
}
// === King Bible（绕身旋转）：数量/半径/角速度/DPS 同长 ===
static inline void GetBibleParams(int lv, int& balls, float& radius, int& dps, float& angVel) {
	lv = std::clamp(lv, 1, 8);
	static const int   ballsT[9] = { 0, 1,1,2,2,3,3,4,5 };           // 数量：最多 5 本
	static const float radT[9] = { 0, 60,70,80,90,100,110,120,130 }; // 半径：从 60 到 130（吃范围BUFF后再放大）
	static const int   dpsT[9] = { 0,  8,10,12,14,16,18,20,24 };   // 伤害×10：小幅稳步增长
	static const float w[9] = { 0,0.05f,0.06f,0.07f,0.08f,0.09f,0.10f,0.11f,0.12f }; // 角速度：逐级加快
	balls = ballsT[lv]; radius = radT[lv]; dps = dpsT[lv]; angVel = w[lv];
}

// === Rocket（火箭）：冷却/速度/伤害/爆炸半径（范围 Buff 生效） ===
static inline void GetRocketParams(int lv, int& cd, float& speed, int& dmg, float& exRadius) {
	lv = std::clamp(lv, 1, 8);
	static const int   cds[9] = { 0, 160,150,140,130,120,110,100, 90 };       // 冷却(帧)：起步很慢，逐级缩短
	static const float spd[9] = { 0, 4.0f,4.2f,4.5f,4.8f,5.2f,5.6f,6.0f,6.5f }; // 飞行速度：逐渐加快
	static const int   dmgv[9] = { 0,   30, 40, 55, 70, 90, 115, 140, 170 };   // 伤害 ×10：大幅成长
	static const float er[9] = { 0,   28, 32, 36, 40, 44, 48, 52, 56 };      // 爆炸半径：中等成长

	cd = cds[lv]; speed = spd[lv]; dmg = dmgv[lv]; exRadius = er[lv];
}

// ===== 实例容器 =====
Player player;
Enemy enemies[maxEnemies]{};

Bullet bullets[maxBullets]{};
ExpOrb orbs[maxOrbs]{};

DaggerProj     gDaggers[kMaxDaggers]{};
LightningStrike gStrikes[kMaxStrikes]{};
Mine           gMines[kMaxMines]{};
Explosion      gExplos[kMaxExplos]{};
LaserBeam      gLasers[kMaxLasers]{};
IceCone        gIces[kMaxIce]{};
Rocket         gRockets[kMaxRockets]{};
GarlicAura     gGarlic{};           // ★新增：全局 1 个光环（参数随等级变化）
float          gBiblePhase = 0.0f;    // King Bible 自转相位（全局）
// King Bible：上一帧是否与敌人/Boss有接触（用于边沿触发一次伤害）
bool gBibleOverlapPrev[maxEnemies]{};
bool gBibleBossOverlapPrev = false;


// ===== Boss =====
struct Boss { float x, y, r, speed; int hp, maxHP; bool active; } gBoss{};
// —— 修复“重开问题”：把 boss 触发与计时器改成全局并在 ResetGame 清零 —— //
bool gBossTriggered = false;
int  gBossTimer = kBossTimeLimit;

// ===== 伤害数字 =====
struct DmgText { float x, y, vy; int life, val; bool crit; };
constexpr int kMaxDmgText = 512; DmgText gDmg[kMaxDmgText]{};
static inline void SpawnDmg(float x, float y, int v, bool crit) {
	for (int i = 0; i < kMaxDmgText; ++i) if (gDmg[i].life <= 0) { gDmg[i] = { x,y,-0.4f,30,v,crit }; break; }
}

// ===== 资源句柄（与原版一致） =====
int texTitleBG = 0, texTitleMoji = 0, texGameBG = 0, texResultBG = 0;
int texBullet = 0, texMine = 0, texIceBeam = 0, texDagger = 0, texLightning = 0, texLaser = 0;
constexpr int kTexBulletW = 12, kTexBulletH = 12,
kTexMineW = 16, kTexMineH = 16,
kTexDaggerW = 16, kTexDaggerH = 16,
kTexIceW = 192, kTexIceH = 8,
kTexLightningW = 24, kTexLightningH = 256,   // ← 改这里（竖直闪电）
kTexLaserW = 384, kTexLaserH = 12;
// === 角色/敌人/Boss 贴图句柄与像素尺寸（源文件像素大小，用于等比缩放） ===
int texPlayer = 0, texBoss = 0;
int texEnemy[ET_COUNT]{}; // ET_T1..ET_T5

// 建议源图尺寸（像素），你可以按美术资源改：保持正方形最安全
// === 角色/敌人/Boss 贴图句柄与像素尺寸（源文件像素大小，用于等比缩放） ===
constexpr int kTexPlayerW = 32, kTexPlayerH = 32;
// [PATCH-BOSS-TEXSIZE] boss 源图是 128×128
constexpr int kTexBossW = 128, kTexBossH = 128;

// 敌人 5 档（示例：28/32/36/44/48）
constexpr int kTexEnemyW[ET_COUNT] = { 28, 32, 36, 44, 48 };
constexpr int kTexEnemyH[ET_COUNT] = { 28, 32, 36, 44, 48 };

int texUIWeapon[WEAPON_COUNT]{}, texUIBuff[BUFF_COUNT]{};
constexpr int UI_ICON_SIZE = 32, UI_ICON_PAD = 8, UI_ROW_GAP = 10, HUD_X = 20, HUD_Y = 20;
// ===== BGM 句柄 =====
int   bgmHandle = -1;
float bgmVolume = 0.35f;
bool  bgmPlaying = false; // 若没有 IsPlayingAudio，用它做播放状态标记


// ===== 经验需求（四段 1~40 级）=====
constexpr int kLevelCap = 40;

// 四段曲线：快 → 稳 → 略升 → 收尾
static inline int NeedXP_4Bands(int lv) {
	if (lv < 1) lv = 1;
	if (lv >= kLevelCap) return 99999999; // 满级后不再升级

	// 你可以按手感调这些数
	if (lv <= 10)  return 8 + lv * 2;            // 1-10：快
	if (lv <= 20)  return 30 + (lv - 10) * 3;     // 11-20：稳
	if (lv <= 30)  return 60 + (lv - 20) * 4;     // 21-30：略升
	/*31-39*/      return 100 + (lv - 30) * 5;    // 31-39：收尾
}


// ===== 基础工具 =====
void SpawnOrb(float x, float y, int v) { for (int i = 0; i < maxOrbs; ++i) if (!orbs[i].active) { orbs[i] = { x,y,6.0f,v,true,false }; return; } }
int FindNearestEnemy(float x, float y) {
	int best = -1; float bestD2 = 1e9f;
	for (int i = 0; i < maxEnemies; ++i) if (enemies[i].active) {
		float dx = enemies[i].x - x, dy = enemies[i].y - y; float d2 = dx * dx + dy * dy;
		if (d2 < bestD2) { bestD2 = d2; best = i; }
	}
	return best;
}
// [REPLACE 1] —— 统一“朝向锁定”的飞镖生成（发射瞬间锁定，之后不再追踪）
void SpawnDagger(float sx, float sy, float speed, int dmg, int pierce) {
	// 选定基础方向：优先最近敌人；没有敌人就用玩家最近移动方向；都没有就默认朝上
	float dirx = 0.0f, diry = -1.0f;
	int idx = FindNearestEnemy(sx, sy);
	if (idx >= 0) {
		float dx = enemies[idx].x - sx, dy = enemies[idx].y - sy;
		float len = std::sqrt(dx * dx + dy * dy);
		if (len > 1e-6f) { dirx = dx / len; diry = dy / len; }
	}
	else {
		if (std::fabs(player.lastDirX) + std::fabs(player.lastDirY) > 1e-4f) {
			float len = std::sqrt(player.lastDirX * player.lastDirX + player.lastDirY * player.lastDirY);
			dirx = player.lastDirX / len; diry = player.lastDirY / len;
		}
	}

	for (int i = 0; i < kMaxDaggers; ++i) if (!gDaggers[i].active) {
		gDaggers[i].x = sx; gDaggers[i].y = sy;
		gDaggers[i].r = 6.0f;
		gDaggers[i].vx = dirx * speed; gDaggers[i].vy = diry * speed;
		gDaggers[i].dmg = dmg;
		gDaggers[i].life = 240;
		gDaggers[i].pierce = pierce;
		gDaggers[i].active = true;
		gDaggers[i].lastHitId = -1;
		gDaggers[i].lastHitCooldown = 0;
		break;
	}
}

void SpawnLightningAt(float tx, float ty, int dmg, float radius) {
	for (int i = 0; i < kMaxStrikes; ++i) if (!gStrikes[i].active) { gStrikes[i] = { tx,ty,12,dmg,radius,true }; break; }
}
void SpawnMine(float x, float y, int fuse, float triggerR, int dmg) {
	for (int i = 0; i < kMaxMines; ++i) if (!gMines[i].active) { gMines[i] = { x,y,6.0f,fuse,triggerR,dmg,true }; break; }
}
void SpawnExplosion(float x, float y, float r, int dmg) {
	for (int i = 0; i < kMaxExplos; ++i) if (!gExplos[i].active) { gExplos[i] = { x,y,r,12,dmg,true }; break; }
}
void SpawnLaser(float angle, float len, float halfW, int life, int dmg, int beamsId = 1) {
	for (int i = 0; i < kMaxLasers; ++i) if (!gLasers[i].active) { gLasers[i] = { angle,len,halfW,life,dmg,beamsId,true }; break; }
}
void SpawnRocket(float x, float y, float vx, float vy, int dmg) {
	for (int i = 0; i < kMaxRockets; ++i) if (!gRockets[i].active) { gRockets[i] = { x,y,vx,vy,6.0f,dmg,420,true }; break; }
}

// ===== 伤害结算（×10制） =====
bool DealDamageEnemy(Enemy& e, int base, Player& plr) {
	bool crit = false; int dmg = ApplyCritToDamage(base, crit);
	e.hp -= dmg; TryLifestealOnHit(plr); SpawnDmg(e.x, e.y, dmg, crit);
	if (e.hp <= 0) {
		e.active = false;
		int xp = (e.tier == ET_T1 ? 1 : e.tier == ET_T2 ? 2 : e.tier == ET_T3 ? 4 : e.tier == ET_T4 ? 6 : 10);
		SpawnOrb(e.x, e.y, xp); ++gScore; return true;
	}
	return false;
}
bool DealDamageBoss(Boss& b, int base, Player& plr) {
	bool crit = false; int dmg = ApplyCritToDamage(base, crit);
	b.hp -= dmg; TryLifestealOnHit(plr); SpawnDmg(b.x, b.y - b.r - 8, dmg, crit);
	if (b.hp <= 0) { b.active = false; return true; }
	return false;
}
// [PATCH-HURT-SCALE] 统一玩家受伤（输入是 ×10 制；内部换算到 ×1 制）
// [PATCH-HURT-SCALE-DEATH] 统一玩家受伤（输入×10制；内部换到×1制；扣到0立判死）
static inline bool TryHurtPlayer(Player& plr, int dmg10) {
	if (gPlayerHurtCD > 0) return false;

	float dmg1f = (dmg10 / 10.0f) * GetDefenseTakenMul();
	int dmg = (int)std::floor(dmg1f + 1e-4f);
	if (dmg < 1) dmg = 1;

	plr.hp -= dmg;
	if (plr.hp < 0) plr.hp = 0;
	gPlayerHurtCD = kPlayerHurtIFrame;

	return true;
}





bool CanSpawnHere(float sx, float sy, float r, Enemy* list, int n) {
	for (int i = 0; i < n; ++i) {
		if (!list[i].active)continue;
		if (CircleHit(sx, sy, r, list[i].x, list[i].y, list[i].r)) return false;
	}
	return true;
}
void SeparateEnemies(Enemy* list, int n) {
	const int maxFix = 12000; int fix = 0;
	for (int i = 0; i < n; ++i) {
		if (!list[i].active)continue;
		for (int j = i + 1; j < n; ++j) {
			if (!list[j].active)continue;
			float dx = list[j].x - list[i].x, dy = list[j].y - list[i].y;
			float R = list[i].r + list[j].r; float d2 = dx * dx + dy * dy;
			if (d2 < R * R) {
				float d = std::sqrt(d2);
				if (d <= 1e-4f) {
					float rx = ((rand() % 2001) - 1000) / 1000.0f, ry = ((rand() % 2001) - 1000) / 1000.0f;
					float len = std::sqrt(rx * rx + ry * ry); if (len < 1e-4f) { rx = 1; ry = 0; }
					else { rx /= len; ry /= len; }
					float m = list[i].r;
					list[i].x -= rx * m * 0.5f; list[i].y -= ry * m * 0.5f;
					list[j].x += rx * m * 0.5f; list[j].y += ry * m * 0.5f;
				}
				else {
					float ov = R - d; float nx = dx / d, ny = dy / d;
					list[i].x -= nx * (ov * 0.5f); list[i].y -= ny * (ov * 0.5f);
					list[j].x += nx * (ov * 0.5f); list[j].y += ny * (ov * 0.5f);
				}
				list[i].x = ClampF(list[i].x, list[i].r, kMapWidth - list[i].r);
				list[i].y = ClampF(list[i].y, list[i].r, kMapHeight - list[i].r);
				list[j].x = ClampF(list[j].x, list[j].r, kMapWidth - list[j].r);
				list[j].y = ClampF(list[j].y, list[j].r, kMapHeight - list[j].r);
				if (++fix >= maxFix)return;
			}
		}
	}
}

// ===== 敌人参数（HP×10；速度恒定基准） =====
static inline float RadiusForTier(EnemyTier t) { static const float r[ET_COUNT] = { 14,16,18,20,24 }; return r[(int)t]; }
static inline int   BaseHPForTier(EnemyTier t) {
	// ×10：紫色显著增强
	static const int hp[ET_COUNT] = { 20, 100, 300, 500, 3000 };
	return hp[(int)t];
}
static inline float SpeedMulForTier(EnemyTier t) {
	static const float m[ET_COUNT] = { 1.00f,1.05f,1.10f,1.12f,1.15f };
	return m[(int)t];
}
// [PATCH-ENEMY-HP-SCALE] scale enemy HP based on player level
static inline float EnemyHPScaleByPlayerLevel(int playerLv) {
	// Lv1 原样；Lv2~5 稍微变脆；Lv6+ 再脆一点
	if (playerLv < 2) return 1.00f;
	if (playerLv < 6) return 0.85f;   // -15%
	return 0.75f;                     // -25%
}

static inline unsigned ColorForTier(EnemyTier t) {
	static const unsigned c[ET_COUNT] = { 0xFF2222FF,0xFFAA22FF,0x22DD22FF,0x2288FFFF,0xAA22FFFF };
	return c[(int)t];
}
static inline void TierWeights(float t01, float w[ET_COUNT]) {
	for (int i = 0; i < ET_COUNT; ++i)w[i] = 0;
	if (t01 < 0.125f) { w[ET_T1] = 1; }
	else if (t01 < 0.375f) { float a = (t01 - 0.125f) / 0.25f; w[ET_T1] = 1.0f - a * 0.7f; w[ET_T2] = a * 0.7f; }
	else if (t01 < 0.625f) { float a = (t01 - 0.375f) / 0.25f; w[ET_T2] = 0.6f * (1 - a) + 0.2f * a; w[ET_T3] = 0.2f * (1 - a) + 0.6f * a; w[ET_T1] = 0.2f * (1 - a); }
	else if (t01 < 0.8125f) { float a = (t01 - 0.625f) / 0.1875f; w[ET_T3] = 0.6f * (1 - a) + 0.2f * a; w[ET_T4] = 0.2f * (1 - a) + 0.6f * a; w[ET_T2] = 0.2f * (1 - a); }
	else { float a = (t01 - 0.8125f) / 0.1875f; w[ET_T4] = 0.6f * (1 - a) + 0.2f * a; w[ET_T5] = 0.2f * (1 - a) + 0.8f * a; w[ET_T3] = 0.2f * (1 - a); }
	float s = 0; for (int i = 0; i < ET_COUNT; ++i)s += w[i]; if (s <= 1e-6f) { w[ET_T1] = 1; return; } for (int i = 0; i < ET_COUNT; ++i)w[i] /= s;
}
static inline EnemyTier SampleTier(float t01) {
	float w[ET_COUNT]; TierWeights(t01, w);
	float r = (float)rand() / RAND_MAX, acc = 0;
	for (int i = 0; i < ET_COUNT; ++i) { acc += w[i]; if (r <= acc) return (EnemyTier)i; }
	return ET_T5;
}
static inline void RandomViewportEdgePoint(float camX, float camY, float& outX, float& outY) {
	int side = rand() % 4;
	if (side == 0) { outX = camX + (float)(rand() % kWindowWidth); outY = camY - 1.0f; }
	else if (side == 1) { outX = camX + (float)(rand() % kWindowWidth); outY = camY + kWindowHeight + 1.0f; }
	else if (side == 2) { outX = camX - 1.0f; outY = camY + (float)(rand() % kWindowHeight); }
	else { outX = camX + kWindowWidth + 1.0f; outY = camY + (float)(rand() % kWindowHeight); }
	outX = ClampF(outX, 0.0f, kMapWidth); outY = ClampF(outY, 0.0f, kMapHeight);
}
static inline void RehomeEnemyToViewportEdge(Enemy& e, float camX, float camY) {
	float nx, ny; RandomViewportEdgePoint(camX, camY, nx, ny); e.x = nx; e.y = ny;
}

// ===== 升级面板（保持） =====
// ===== 拥有数统计/判断 =====
static inline int CountOwnedWeapons() {
	int c = 0;
	for (int w = 0; w < WEAPON_COUNT; ++w) if (gWeaponLv[w] > 0) ++c;
	return c;
}
static inline int CountOwnedBuffs() {
	int c = 0;
	for (int b = 0; b < BUFF_COUNT; ++b) if (gBuffLv[b] > 0) ++c;
	return c;
}
static inline bool OwnsWeapon(int w) { return (w >= 0 && w < WEAPON_COUNT && gWeaponLv[w] > 0); }
static inline bool OwnsBuff(int b) { return (b >= 0 && b < BUFF_COUNT && gBuffLv[b]  > 0); }

static const char* kBuffNameCN[BUFF_COUNT] = { "HP","Move","Magnet","Cooldown","Defense","Crit","Pierce","Area","XP","Lifesteal" };
enum UpgradeKind {
	UPGRADE_BUFF = 0,
	UPGRADE_WEAPON,
	UPGRADE_CAP_WEAPON,   // ★ 稀有：武器上限 +1
	UPGRADE_CAP_BUFF      // ★ 稀有：Buff 上限 +1
};

struct ChoicePanel { bool active{}; int pendingLevels{}; int kind[3]{}; int id[3]{}; std::string text[3]; };
// === 放在 BuffDescEN 之前：独立的武器说明函数 ===
// [PATCH-WEAPON-DESC] concise English per-level upgrade texts (Lv2~Lv8)
static inline std::string WeaponDesc(int w, int nextLv) {
	nextLv = std::clamp(nextLv, 1, 8);

	// Helper: return string for level; empty for Lv1/unknown
	auto pick = [&](const char* const table[9]) -> std::string {
		const char* s = table[nextLv];
		if (!s || !*s) return std::string("No change");
		return std::string(s);
		};

	// ---------- Bullet / Knife ----------
	static const char* BULLET[9] = {
		"", // 0 (unused)
		"", // Lv1 acquire
		"CD ↓, +1 shot, minor dmg ↑, spread ↑",                // Lv2
		"Speed ↑, +1 pierce, spread ↑",                        // Lv3
		"CD ↓, +1 shot, minor dmg ↑",                          // Lv4
		"+1 shot, minor dmg ↑, spread ↑",                      // Lv5
		"Speed ↑, CD ↓, +1 shot",                              // Lv6
		"+1 pierce, +1 shot, minor dmg ↑, spread ↑",           // Lv7
		"CD ↓, +1 shot, minor dmg ↑, spread ↑"                 // Lv8
	};

	// ---------- Mine ----------
	static const char* MINE[9] = {
		"",
		"",
		"CD ↓, trigger radius ↑, dmg ↑",                       // Lv2
		"+1 mine, trigger radius ↑, dmg ↑",                    // Lv3
		"CD ↓, explosion radius ↑, dmg ↑",                     // Lv4
		"Trigger radius ↑, explosion radius ↑, dmg ↑",         // Lv5
		"CD ↓, +1 mine, trigger radius ↑",                     // Lv6
		"Trigger radius ↑, explosion radius ↑, dmg ↑",         // Lv7
		"CD ↓, +1 mine, explosion radius ↑, dmg ↑"             // Lv8
	};

	// ---------- Garlic (aura) ----------
	static const char* GARLIC[9] = {
		"",
		"",
		"Aura radius ↑",                                       // Lv2
		"DPS ↑",                                               // Lv3
		"CD ↓",                                                // Lv4
		"Aura radius ↑",                                       // Lv5
		"DPS ↑",                                               // Lv6
		"CD ↓",                                                // Lv7
		"Aura radius ↑, DPS ↑"                                 // Lv8
	};

	// ---------- Magic Wand / Dagger (single-shot, on-fire direction lock) ----------
	static const char* DAGGER[9] = {
		"",
		"",
		"CD ↓, minor dmg ↑",                                   // Lv2
		"+1 pierce, speed ↑",                                  // Lv3
		"CD ↓, hitbox radius ↑",                               // Lv4 (projectile collision grows; also scales with Area)
		"Minor dmg ↑, +1 pierce",                              // Lv5
		"CD ↓, speed ↑",                                       // Lv6
		"+1 pierce, minor dmg ↑",                              // Lv7
		"CD ↓, hitbox radius ↑"                                // Lv8
	};

	// ---------- Lightning ----------
	static const char* LIGHTNING[9] = {
		"",
		"",
		"+1 strike, CD ↓, dmg ↑, radius ↑",                    // Lv2
		"+1 strike, CD ↓, dmg ↑, radius ↑",                    // Lv3
		"+1 strike, CD ↓, dmg ↑, radius ↑",                    // Lv4
		"+1 strike, CD ↓, dmg ↑, radius ↑",                    // Lv5
		"+1 strike, CD ↓, dmg ↑, radius ↑",                    // Lv6
		"+1 strike, CD ↓, dmg ↑, radius ↑",                    // Lv7
		"+1 strike, CD ↓, big dmg ↑, radius ↑"                 // Lv8
	};

	// ---------- Clock Lancet (laser beams; first-frame hit only) ----------
	static const char* LANCET[9] = {
		"",
		"",
		"Beam length ↑, width ↑, linger ↑",                    // Lv2 (e.g., 5f -> 7f)
		"+1 beam, length ↑",                                   // Lv3
		"Length ↑, width ↑, linger ↑",                         // Lv4
		"Width ↑, length ↑ (to ~1/5 screen)",                  // Lv5
		"+1 beam, linger ↑",                                   // Lv6
		"Length ↑, width ↑, linger ↑",                         // Lv7
		"+2 beams (→5), length ↑ (~1/4 screen), width ↑ (~30), linger ↑ (→20f)" // Lv8
	};

	// ---------- King Bible (orbital; edge-on contact deals once) ----------
	static const char* BIBLE[9] = {
		"",
		"",
		"Radius ↑, small dmg ↑, spin speed ↑",                 // Lv2
		"+1 book, radius ↑, spin speed ↑",                     // Lv3
		"Radius ↑, small dmg ↑, spin speed ↑",                 // Lv4
		"+1 book, radius ↑, spin speed ↑",                     // Lv5
		"Radius ↑, small dmg ↑, spin speed ↑",                 // Lv6
		"+1 book, radius ↑, spin speed ↑",                     // Lv7
		"+1 book, radius ↑, dmg ↑, spin speed ↑"               // Lv8
	};

	// ---------- Rocket (high dmg / long CD version) ----------
	static const char* ROCKET[9] = {
		"",
		"",
		"CD ↓, dmg ↑, speed ↑",                                // Lv2
		"CD ↓, dmg ↑, blast radius ↑",                         // Lv3
		"CD ↓, dmg ↑, speed ↑",                                // Lv4
		"CD ↓, dmg ↑, blast radius ↑",                         // Lv5
		"CD ↓, dmg ↑, speed ↑",                                // Lv6
		"CD ↓, dmg ↑, blast radius ↑",                         // Lv7
		"CD ↓, big dmg ↑, speed ↑"                             // Lv8
	};

	switch (w) {
	case WEAPON_BULLET:    return pick(BULLET);
	case WEAPON_MINE:      return pick(MINE);
	case WEAPON_GARLIC:    return pick(GARLIC);
	case WEAPON_DAGGER:    return pick(DAGGER);
	case WEAPON_LIGHTNING: return pick(LIGHTNING);
	case WEAPON_LASER:     return pick(LANCET);
	case WEAPON_ORBITAL:   return pick(BIBLE);
	case WEAPON_ROCKET:    return pick(ROCKET);
	default:               return std::string("Unknown");
	}
}


// === Buff 英文描述（保持原逻辑，注意不再嵌套函数） ===
static inline std::string BuffDescEN(int id, int nextLv) {
	nextLv = std::clamp(nextLv, 1, 5);
	char b[256];
	switch (id) {
	case BUFF_HP:        sprintf_s(b, "More HP (+2/+3; Lv5 regen)"); return b;
	case BUFF_SPEED: { static const int pct[6] = { 0,5,10,15,25,40 }; sprintf_s(b, "Move +%d%%", pct[nextLv]); return b; }
	case BUFF_MAGNET: { if (nextLv < 5) { static const int add[6] = { 0,20,40,80,140,0 }; sprintf_s(b, "Pickup +%dpx", add[nextLv]); } else sprintf_s(b, "Global vacuum"); return b; }
	case BUFF_COOLDOWN: { static const int red[6] = { 0,5,10,20,30,45 }; sprintf_s(b, "CD -%d%% (min 0.3x)", red[nextLv]); return b; }
	case BUFF_DEFENSE: { static const int red[6] = { 0,10,20,30,45,65 }; sprintf_s(b, "Taken -%d%% (min 0.2x)", red[nextLv]); return b; }
	case BUFF_CRIT: { static const int c[6] = { 0,5,8,12,16,22 }; static const int m[6] = { 0,150,160,175,200,230 }; sprintf_s(b, "Crit %d%% / %d%% dmg", c[nextLv], m[nextLv]); return b; }
	case BUFF_PIERCE: { static const int p[6] = { 0,1,2,4,4,7 }; sprintf_s(b, "+%d pierce", p[nextLv]); return b; }
	case BUFF_AREA: { static const int pct[6] = { 0,6,12,20,30,45 }; sprintf_s(b, "Area +%d%%", pct[nextLv]); return b; }
	case BUFF_XP: { static const int pct2[6] = { 0,6,12,20,30,45 }; sprintf_s(b, "XP +%d%%", pct2[nextLv]); return b; }
	case BUFF_LIFESTEAL: { static const int pct3[6] = { 0,2,3,5,7,10 }; sprintf_s(b, "On-hit heal %d%%", pct3[nextLv]); return b; }
	}
	return "Unknown";
}

// === 生成面板行（保持不变） ===
static inline std::string MakeBuffLine(int id) {
	int cur = gBuffLv[id];
	int nxt = std::min(cur + 1, MAX_LV_EACH);
	return std::string("Buff: ") + kBuffNameCN[id] + "  Lv " + std::to_string(cur)
		+ " -> " + std::to_string(nxt) + "  | EN: " + BuffDescEN(id, nxt);
}


// 仅用于开局：只给武器的三选一，不增加等级
void RollInitialWeaponPanel(ChoicePanel& panel) {
	// 收集“还没拥有”的武器
	int pool[WEAPON_COUNT], n = 0;
	for (int w = 0; w < WEAPON_COUNT; ++w) {
		if (gWeaponLv[w] <= 0) pool[n++] = w;
	}
	// 不足 3 个就全给；足够则随机抽 3 个
	std::shuffle(pool, pool + n, std::mt19937{ std::random_device{}() });

	int show = std::min(3, n);

	panel.active = true;
	panel.pendingLevels = 0;
	for (int i = 0; i < 3; ++i) {
		if (i < show) {
			panel.kind[i] = UPGRADE_WEAPON;
			panel.id[i] = pool[i];
			int lv = 0;
			panel.text[i] = std::string(WeaponNames[panel.id[i]]) + "  Lv " + std::to_string(lv) + " -> " + std::to_string(lv + 1);
		}
		else {
			panel.kind[i] = UPGRADE_BUFF;
			panel.id[i] = BUFF_HP;
			panel.text[i] = "No weapon";
		}
	}
}

void RollPanel(ChoicePanel& panel) {
	// 动态上限（由稀有选项提升）
	const int kMaxOwnedWeapons = gMaxWeaponCap;
	const int kMaxOwnedBuffs = gMaxBuffCap;

	const int ownedW = CountOwnedWeapons();
	const int ownedB = CountOwnedBuffs();

	// 候选池
	int  poolK[256];
	int  poolID[256];
	int  poolN = 0;

	// ===== 常规候选：武器 =====
	for (int w = 0; w < WEAPON_COUNT; ++w) {
		const bool maxed = (gWeaponLv[w] >= 8);
		if (maxed) continue;

		if (ownedW < kMaxOwnedWeapons) {
			// 没到上限：允许“新武器”和“已有武器升等级”
			poolK[poolN] = UPGRADE_WEAPON; poolID[poolN] = w; ++poolN;
		}
		else {
			// 达到上限：只允许“已有武器升等级”
			if (OwnsWeapon(w)) { poolK[poolN] = UPGRADE_WEAPON; poolID[poolN] = w; ++poolN; }
		}
	}

	// ===== 常规候选：Buff =====
	for (int b = 0; b < BUFF_COUNT; ++b) {
		const bool maxed = (gBuffLv[b] >= MAX_LV_EACH);
		if (maxed) continue;

		if (ownedB < kMaxOwnedBuffs) {
			poolK[poolN] = UPGRADE_BUFF; poolID[poolN] = b; ++poolN;
		}
		else {
			if (OwnsBuff(b)) { poolK[poolN] = UPGRADE_BUFF; poolID[poolN] = b; ++poolN; }
		}
	}

	// ===== 稀有候选：上限 +1（低概率注入 0~1 个到候选池）=====
	bool capInjected = false;
	auto tryInjectCap = [&](int kind) {
		if (capInjected) return;
		if (kind == UPGRADE_CAP_WEAPON && gMaxWeaponCap < kCapHardLimit) {
			if ((rand() % 100) < 10) { // 10% 概率，可改 5
				poolK[poolN] = UPGRADE_CAP_WEAPON; poolID[poolN] = 0; ++poolN; capInjected = true;
			}
		}
		if (kind == UPGRADE_CAP_BUFF && gMaxBuffCap < kCapHardLimit) {
			if ((rand() % 100) < 10) {
				poolK[poolN] = UPGRADE_CAP_BUFF; poolID[poolN] = 0; ++poolN; capInjected = true;
			}
		}
		};
	// 两者最多注入一个
	tryInjectCap(UPGRADE_CAP_WEAPON);
	tryInjectCap(UPGRADE_CAP_BUFF);

	panel.active = true;

	if (poolN <= 0) {
		for (int i = 0; i < 3; ++i) {
			panel.kind[i] = UPGRADE_BUFF; panel.id[i] = BUFF_HP; panel.text[i] = "No upgrades";
		}
		return;
	}

	// 不放回抽三项（去重）
	for (int i = 0; i < 3; ++i) {
		if (poolN <= 0) {
			panel.kind[i] = UPGRADE_BUFF; panel.id[i] = BUFF_HP; panel.text[i] = "No upgrades";
			continue;
		}
		int tryCount = 0;
		while (true) {
			int pick = rand() % poolN;
			int k = poolK[pick], id = poolID[pick];

			// 与前面已选去重
			bool dup = false;
			for (int j = 0; j < i; ++j) {
				if (panel.kind[j] == k && panel.id[j] == id) { dup = true; break; }
			}
			if (!dup || ++tryCount > 20) {
				panel.kind[i] = k; panel.id[i] = id;

				// 移除（修正下标）
				poolK[pick] = poolK[poolN - 1];
				poolID[pick] = poolID[poolN - 1];
				--poolN;
				break;
			}
		}

		// 文案
		if (panel.kind[i] == UPGRADE_WEAPON) {
			int lv = gWeaponLv[panel.id[i]];
			panel.text[i] = std::string(WeaponNames[panel.id[i]]) + "  Lv " + std::to_string(lv) + " -> " + std::to_string(lv + 1);
		}
		else if (panel.kind[i] == UPGRADE_BUFF) {
			panel.text[i] = MakeBuffLine(panel.id[i]);
		}
		else if (panel.kind[i] == UPGRADE_CAP_WEAPON) {
			panel.text[i] = "[Rare] Weapon slots +1";
		}
		else if (panel.kind[i] == UPGRADE_CAP_BUFF) {
			panel.text[i] = "[Rare] Buff slots +1";
		}
	}
}


void ApplyUpgrade(int kind, int id, Player& plr) {
	const int kMaxOwnedWeapons = gMaxWeaponCap;
	const int kMaxOwnedBuffs = gMaxBuffCap;

	if (kind == UPGRADE_CAP_WEAPON) {
		if (gMaxWeaponCap < kCapHardLimit) ++gMaxWeaponCap;  // HUD 会自动多一个空框
		return;
	}
	if (kind == UPGRADE_CAP_BUFF) {
		if (gMaxBuffCap < kCapHardLimit) ++gMaxBuffCap;      // HUD 会自动多一个空框
		return;
	}

	if (kind == UPGRADE_BUFF) {
		// 新增 Buff 时检查动态上限
		if (gBuffLv[id] <= 0 && CountOwnedBuffs() >= kMaxOwnedBuffs) return;

		if (gBuffLv[id] >= MAX_LV_EACH) return;
		++gBuffLv[id];

		if (id == BUFF_HP) {
			int inc = (gBuffLv[BUFF_HP] <= 2 ? 2 : 3);
			plr.maxHP += inc; plr.hp += inc; if (plr.hp > plr.maxHP) plr.hp = plr.maxHP;
			if (gBuffLv[BUFF_HP] >= 5) gHPRegenTimer = 0;
		}
	}
	else if (kind == UPGRADE_WEAPON) {
		// 新增武器时检查动态上限
		if (gWeaponLv[id] <= 0 && CountOwnedWeapons() >= kMaxOwnedWeapons) return;

		if (gWeaponLv[id] >= 8) return;
		++gWeaponLv[id];

		if (id == WEAPON_GARLIC) { gCD_Garlic = 0; }
		if (id == WEAPON_DAGGER) { gCD_Dagger = 0; } // [PATCH] 首次获得立刻可发
	}
}



// ===== 绘制工具 =====
// 按圆半径 r 画 Sprite：自动把 r 对齐到纹理宽的一半
// 以“底部居中”为锚点画 Sprite：让 (worldX, worldY) 对齐贴图底边中心
static inline void DrawSpriteBottomCenter(
	int tex, int texW, int texH,
	float worldX, float worldY,
	float scaleX, float scaleY,
	float angle, unsigned color,
	float camX, float camY)
{
	if (tex <= 0) return;
	int sx = (int)std::round(worldX - camX - (texW * scaleX * 0.5f));
	int sy = (int)std::round(worldY - camY - (texH * scaleY)); // 顶点在冲击点上方
	Novice::DrawSprite(sx, sy, tex, scaleX, scaleY, angle, color);
}

static inline void DrawSpriteByRadius(
	int tex, int texW, int texH,
	float worldX, float worldY, float r,
	float angle, unsigned color,
	float camX, float camY)
{
	if (tex <= 0 || r <= 0) return;
	float scale = (r * 2.0f) / (float)texW;          // 让贴图宽度 = 直径
	int sx = (int)(worldX - camX - (texW * scale * 0.5f));
	int sy = (int)(worldY - camY - (texH * scale * 0.5f));
	Novice::DrawSprite(sx, sy, tex, scale, scale, angle, color);
}

static inline void DrawBeamTiled(int tex, int texW, int texH, float sx, float sy, float angle, float length, float scaleY, float camX, float camY, unsigned color) {
	if (length <= 0) return;
	int tiles = (int)std::ceil(length / (float)texW);
	float dirx = std::cos(angle), diry = std::sin(angle);
	float x = sx - camX, y = sy - camY;
	for (int i = 0; i < tiles; ++i) {
		float px = x + dirx * (i * texW), py = y + diry * (i * texW);
		Novice::DrawSprite((int)px, (int)(py - texH * scaleY / 2), tex, 1.0f, scaleY, angle, color);
	}
}
// 带边框且仅在拥有时显示
static inline void DrawIconWithLv(int tex, int x, int y, int lv, bool /*dimWhenZero*/ = false, const char* fallback = nullptr) {
	if (lv <= 0) return; // 未拥有不显示

	// 白色细边框（包一圈）
	Novice::DrawBox(x - 2, y - 2, UI_ICON_SIZE + 4, UI_ICON_SIZE + 4, 0, 0xFFFFFFFF, kFillModeWireFrame);

	if (tex > 0) {
		Novice::DrawSprite(x, y, tex, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
	}
	else {
		// 无贴图占位
		Novice::DrawBox(x, y, UI_ICON_SIZE, UI_ICON_SIZE, 0, 0xFFFFFF22, kFillModeSolid);
		Novice::DrawBox(x, y, UI_ICON_SIZE, UI_ICON_SIZE, 0, 0xFFFFFFFF, kFillModeWireFrame);
		if (fallback) Novice::ScreenPrintf(x + 6, y + 10, "%s", fallback);
	}

	// 右下角等级角标
	int badgeW = 20, badgeH = 12;
	int bx = x + UI_ICON_SIZE - badgeW - 2, by = y + UI_ICON_SIZE - badgeH - 2;
	Novice::DrawBox(bx, by, badgeW, badgeH, 0, 0x000000AA, kFillModeSolid);
	char b[16]; sprintf_s(b, "L%d", lv);
	Novice::ScreenPrintf(bx + 4, by + 1, "%s", b);
}

// === 基础粗线段绘制（用小圆叠出粗线，免贴图） ===
static inline void DrawThickLinePrimitive(float sx, float sy, float angle, float length, float thickness,

	float camX, float camY, unsigned color) {
	if (length <= 0 || thickness <= 0) return;
	float dx = std::cos(angle), dy = std::sin(angle);
	float step = std::max(4.0f, thickness * 0.6f); // 步长小一点线更连续
	int   steps = (int)std::ceil(length / step);

	float cx0 = sx - camX, cy0 = sy - camY; // 屏幕空间
	int   rad = (int)std::round(thickness * 0.5f);
	for (int i = 0; i <= steps; ++i) {
		float t = std::min(length, i * step);
		float px = cx0 + dx * t;
		float py = cy0 + dy * t;
		Novice::DrawEllipse((int)px, (int)py, rad, rad, 0, color, kFillModeSolid);
	}
}
// [PATCH] 胶囊体（线段+半径）与圆的相交 —— 用于柳叶刀命中
static inline bool CapsuleCircleHit(
	float sx, float sy, float ex, float ey, float radius,
	float cx, float cy, float cr) {
	float vx = ex - sx, vy = ey - sy;
	float wx = cx - sx, wy = cy - sy;
	float vv = vx * vx + vy * vy;
	float t = (vv > 1e-6f) ? (vx * wx + vy * wy) / vv : 0.0f;
	if (t < 0) t = 0; else if (t > 1) t = 1;
	float px = sx + vx * t, py = sy + vy * t;
	float dx = cx - px, dy = cy - py;
	float rr = (radius + cr) * (radius + cr);
	return (dx * dx + dy * dy) <= rr;
}

// ===== Reset（重开关键：把一切清干净） =====
void ResetGame(Player& plr, float& cameraX, float& cameraY, ChoicePanel& panel) {
	plr.x = kMapWidth / 2; plr.y = kMapHeight / 2; plr.r = 16;
	plr.maxHP = 10; plr.hp = plr.maxHP; plr.baseSpeed = playerBaseSpeed0; plr.lastDirX = 0; plr.lastDirY = -1;

	// 清空容器
	for (int i = 0; i < maxEnemies; ++i) enemies[i].active = false;
	for (int i = 0; i < maxBullets; ++i) bullets[i].active = false;
	for (int i = 0; i < maxOrbs; ++i)    orbs[i].active = false;
	for (int i = 0; i < kMaxDaggers; ++i)gDaggers[i].active = false;
	for (int i = 0; i < kMaxStrikes; ++i)gStrikes[i].active = false;
	for (int i = 0; i < kMaxMines; ++i)  gMines[i].active = false;
	for (int i = 0; i < kMaxExplos; ++i) gExplos[i].active = false;
	for (int i = 0; i < kMaxLasers; ++i) gLasers[i].active = false;
	for (int i = 0; i < kMaxIce; ++i)    gIces[i].active = false;
	for (int i = 0; i < kMaxRockets; ++i)gRockets[i].active = false;
	for (int i = 0; i < kMaxDmgText; ++i)gDmg[i].life = 0;

	// 全局状态
	gLevel = 1; gXP = 0; gScore = 0; gEnemyAccum = 0; gGameFrame = 0;
	gWin = false; gPaused = false;
	// 动态上限重置
	gMaxWeaponCap = 4;
	gMaxBuffCap = 4;


	// 冷却器
	gCD_Bullet = gCD_Mine = gCD_Garlic = gCD_Dagger = gCD_Lightning = gCD_Laser = gCD_Rocket = 0;

	// Buff/Weapon 等级
	for (int i = 0; i < BUFF_COUNT; ++i)   gBuffLv[i] = 0;
	for (int i = 0; i < WEAPON_COUNT; ++i) gWeaponLv[i] = 0;
	gGarlic = { 60.0f, 60, 0, 8, false }; // 将由 Update 按等级刷新
	gBiblePhase = 0.0f;

	// Boss 全面重置（修复重开问题）
	gBoss = { 0,0,80,0.9f,0,0,false };
	gBossTriggered = false;
	gBossTimer = kBossTimeLimit;

	gHPRegenTimer = 0; panel.active = false; panel.pendingLevels = 0;

	cameraX = ClampF(plr.x - kWindowWidth / 2.0f, 0.0f, kMapWidth - kWindowWidth);
	cameraY = ClampF(plr.y - kWindowHeight / 2.0f, 0.0f, kMapHeight - kWindowHeight);

	dbg_spawnInterval = spawnIntervalStart; dbg_spawnCount = (int)spawnCountStart;
	dbg_speedScale = 1.0f; dbg_hpScale = 1.0f;

	// King Bible 碰撞判定状态清零
	for (int i = 0; i < maxEnemies; ++i) {
		gBibleOverlapPrev[i] = false;
	}
	gBibleBossOverlapPrev = false;
	gInitialPickPending = true;           // 进游戏先弹初始武器选择
	gPlayerHurtCD = 0; // [PATCH] 初始化无敌冷却
	plr.hurtIFrames = 0;

}


// ====== WinMain 入口（主循环与渲染放在 Part B） ======
/****************************************************
 * Programming Survivor — 改造版 Part B
 * 说明：将本段直接**替换 Part A 里 WinMain 的占位实现**
 * （也就是把 Part A 末尾那个只有“Initialize/LoadTexture 然后 return 0;”
 * 的 WinMain 删掉，用下面这个完整 WinMain 覆盖。）
 ****************************************************/

int WINAPI WinMain(_In_ HINSTANCE /*hInstance*/,
	_In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPSTR /*lpCmdLine*/,
	_In_ int /*nShowCmd*/) {

	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// 资源加载（与 Part A 一致）
	texTitleBG = Novice::LoadTexture("./Resources/title.png");
	texTitleMoji = Novice::LoadTexture("./Resources/titleMoji.png");
	texGameBG = Novice::LoadTexture("./Resources/background.png");
	texResultBG = Novice::LoadTexture("./Resources/result.png");
	texBullet = Novice::LoadTexture("./Resources/bullet.png");
	texMine = Novice::LoadTexture("./Resources/mine.png");
	texIceBeam = Novice::LoadTexture("./Resources/IceBeam.png");
	texDagger = Novice::LoadTexture("./Resources/TrackingDagger.png");
	texLightning = Novice::LoadTexture("./Resources/Lightning.png");
	texLaser = Novice::LoadTexture("./Resources/Laser.png");

	// === 角色/敌人/Boss 贴图加载（放到 ./Resources/ 下）===
	texPlayer = Novice::LoadTexture("./Resources/player.png");
	texBoss = Novice::LoadTexture("./Resources/boss.png");
	texEnemy[ET_T1] = Novice::LoadTexture("./Resources/enemy_t1.png");
	texEnemy[ET_T2] = Novice::LoadTexture("./Resources/enemy_t2.png");
	texEnemy[ET_T3] = Novice::LoadTexture("./Resources/enemy_t3.png");
	texEnemy[ET_T4] = Novice::LoadTexture("./Resources/enemy_t4.png");
	texEnemy[ET_T5] = Novice::LoadTexture("./Resources/enemy_t5.png");


	texUIWeapon[WEAPON_BULLET] = Novice::LoadTexture("./Resources/ui_weapon_bullet.png");
	texUIWeapon[WEAPON_MINE] = Novice::LoadTexture("./Resources/ui_weapon_mine.png");
	texUIWeapon[WEAPON_GARLIC] = Novice::LoadTexture("./Resources/ui_weapon_garlic.png");
	texUIWeapon[WEAPON_DAGGER] = Novice::LoadTexture("./Resources/ui_weapon_dagger.png");
	texUIWeapon[WEAPON_LIGHTNING] = Novice::LoadTexture("./Resources/ui_weapon_lightning.png");
	texUIWeapon[WEAPON_LASER] = Novice::LoadTexture("./Resources/ui_weapon_laser.png");
	texUIWeapon[WEAPON_ORBITAL] = Novice::LoadTexture("./Resources/ui_weapon_icebeam.png");
	texUIWeapon[WEAPON_ROCKET] = Novice::LoadTexture("./Resources/ui_weapon_rocket.png");
	texUIBuff[0] = Novice::LoadTexture("./Resources/ui_buff_hp.png");
	texUIBuff[1] = Novice::LoadTexture("./Resources/ui_buff_move.png");
	texUIBuff[2] = Novice::LoadTexture("./Resources/ui_buff_magnet.png");
	texUIBuff[3] = Novice::LoadTexture("./Resources/ui_buff_cooldown.png");
	texUIBuff[4] = Novice::LoadTexture("./Resources/ui_buff_defense.png");
	texUIBuff[5] = Novice::LoadTexture("./Resources/ui_buff_crit.png");
	texUIBuff[6] = Novice::LoadTexture("./Resources/ui_buff_pierce.png");
	texUIBuff[7] = Novice::LoadTexture("./Resources/ui_buff_area.png");
	texUIBuff[8] = Novice::LoadTexture("./Resources/ui_buff_xp.png");
	texUIBuff[9] = Novice::LoadTexture("./Resources/ui_buff_lifesteal.png");
	// ===== 加载并准备 BGM（文件放在 ./Resources/ 下）=====
	bgmHandle = Novice::LoadAudio("./Resources/edm-gaming-music-335408.mp3");


	Scene scene = SCENE_TITLE;
	float cameraX = 0, cameraY = 0;
	player = { kMapWidth / 2, kMapHeight / 2, 16, 10, 10, playerBaseSpeed0, 0, -1 };
	ChoicePanel panel{};

	auto spawnEnemiesTick = [&](float time01) {
		// 刷怪曲线：间隔线性收敛、数量线性上升（但峰值降低）
		float currentInterval = Lerp(spawnIntervalStart, spawnIntervalEnd, time01);
		int   currentCount = (int)(Lerp(spawnCountStart, spawnCountEnd, time01) + 0.5f);
		currentCount = std::max(1, currentCount);
		dbg_spawnInterval = currentInterval;
		dbg_spawnCount = currentCount;

		gEnemyAccum++;
		if (gEnemyAccum >= (int)currentInterval) {
			for (int k = 0; k < currentCount; ++k) {
				int idx = -1; for (int i = 0; i < maxEnemies; ++i) if (!enemies[i].active) { idx = i; break; }
				if (idx < 0) break;
				EnemyTier tierSel = SampleTier(time01);
				int baseHP = BaseHPForTier(tierSel);                  // ×10
				float hpMul = EnemyHPScaleByPlayerLevel(gLevel);      // [PATCH-ENEMY-HP-SCALE]
				int   hp = std::max(1, (int)std::round(baseHP * hpMul));
				float speed = enemyBaseSpeed0 * SpeedMulForTier(tierSel);

				Enemy e{}; e.r = RadiusForTier(tierSel); e.speed = speed; e.maxHP = hp; e.hp = hp; e.tier = tierSel; e.active = false; e.slowFactor = 1.0f;
				// 在视口边缘生成并避免重叠
				bool ok = false; for (int tr = 0; tr < 20; ++tr) {
					float sx, sy; RandomViewportEdgePoint(cameraX, cameraY, sx, sy);
					if (CanSpawnHere(sx, sy, e.r, enemies, maxEnemies)) { e.x = sx; e.y = sy; ok = true; break; }
				}
				if (!ok) continue;
				enemies[idx] = e; enemies[idx].active = true;
			}
			gEnemyAccum = 0;
		}
		};

	auto updateBoss = [&]() {
		if (!gBossTriggered && gGameFrame >= kGameDurationFrames) {
			// 触发 Boss：超高 HP、重置计时器，清场
			gBossTriggered = true;
			gBoss.active = true;
			gBoss.r = 64.0f;              // 初始直径≈贴图原始大小
			gBoss.speed = 2.0f;
			gBoss.maxHP = 100000 + (int)(gLevel * 400) + 5000; // “非常非常高”
			gBoss.hp = gBoss.maxHP;
			float bx, by; RandomViewportEdgePoint(cameraX, cameraY, bx, by);
			gBoss.x = bx; gBoss.y = by;
			for (int i = 0; i < maxEnemies; ++i) enemies[i].active = false;
			gBossTimer = kBossTimeLimit;
		}

		if (gBossTriggered && gBoss.active && !gPaused) {
			// [PATCH-BOSS-DASH-DIRLOCK]
			// 调速基准
			// [PATCH-BOSS-SLOWER-THAN-PLAYER]
			// 玩家当前实际速度（含移动BUFF等）
			float playerCurrSpeed = player.baseSpeed * GetSpeedMul();

			// Boss 速度永远低于玩家当前速度
			// 平时 75%；冲刺 95%（仍然比玩家略慢，避免“没碰到就被杀”）
			float bossBase = playerCurrSpeed * 0.75f;
			float bossDash = playerCurrSpeed * 0.95f;

			// 安全下限（避免极端情况下变得太慢/卡住，可按手感调或删）
			bossBase = std::max(bossBase, 1.2f);
			bossDash = std::max(std::min(bossDash, playerCurrSpeed - 0.01f), bossBase + 0.1f);


			// 冲刺状态
			static int   dashCD = 180;
			static int   dashTime = 0;
			static float dashDirX = 0.0f, dashDirY = -1.0f; // 冲刺锁定方向

			// 是否开新冲刺（先决策后移动）
			if (dashTime <= 0) {
				gBoss.speed = bossBase;

				// 计时到点 → 进入冲刺，并锁定当前方向
				if (--dashCD <= 0) {
					dashTime = 50;           // 冲刺持续帧数（可调）
					dashCD = 140;          // 下一次冲刺冷却（可调）

					// 以当前帧玩家位置锁定冲刺方向
					float vx = player.x - gBoss.x;
					float vy = player.y - gBoss.y;
					float L = std::sqrt(vx * vx + vy * vy);
					if (L < 1e-5f) { dashDirX = 0.0f; dashDirY = -1.0f; }
					else { dashDirX = vx / L; dashDirY = vy / L; }
				}
			}
			else {
				// 冲刺中：保持固定方向与速度（不再逐帧追踪）
				--dashTime;
				gBoss.speed = bossDash;
			}

			// 计算本帧移动方向
			float dirx, diry;
			if (dashTime > 0) {
				// 冲刺：用锁定方向
				dirx = dashDirX; diry = dashDirY;
			}
			else {
				// 非冲刺：平时缓慢追踪
				float vx = player.x - gBoss.x;
				float vy = player.y - gBoss.y;
				float L = std::sqrt(vx * vx + vy * vy);
				if (L < 1e-5f) { dirx = 0.0f; diry = -1.0f; }
				else { dirx = vx / L; diry = vy / L; }
			}

			// 实际移动
			gBoss.x += dirx * gBoss.speed;
			gBoss.y += diry * gBoss.speed;

			// [体型随时间缓增]（大小与碰撞体积一致，见锚点 B 上限值）
			static const float kBossRMax = 160.0f;
			if ((gGameFrame % 30) == 0) {
				gBoss.r = std::min(kBossRMax, gBoss.r + 1.0f);
			}

			// 限时失败
			if (--gBossTimer <= 0 && gBoss.active) {
				scene = SCENE_RESULT; gWin = false;
			}
		}


		if (gBossTriggered && !gBoss.active) { scene = SCENE_RESULT; gWin = true; }
		};

	// ========= 主循环 =========
	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256); Novice::GetHitKeyStateAll(keys);

		// 全局：ESC 退出
		if (keys[DIK_ESCAPE]) { break; }

		// 游戏中：P 暂停，R 回标题
		if (scene == SCENE_GAME) {
			if (!preKeys[DIK_P] && keys[DIK_P]) gPaused = !gPaused;
			if (!preKeys[DIK_R] && keys[DIK_R]) { scene = SCENE_TITLE; }
			// ---- 调试：按下 O 直接触发一次升级（等效“升一级”） ----
// 规则：如果当前没有升级面板，就立即 RollPanel 弹出选择；
//       如果已经有升级面板在显示，就把 pendingLevels 累加一次（堆叠多次升级）。
			if (!preKeys[DIK_O] && keys[DIK_O]) {
				if (!panel.active) {
					RollPanel(panel);          // 打开升级面板
				}
				else {
					++panel.pendingLevels;     // 叠一层待处理的升级
				}
				// 可选：给个小提示（也可以删掉）
				// Novice::ScreenPrintf(20, kWindowHeight - 60, "[DEV] Forced Level Up (O)");
			}
			// ---- 调试：按下 I 直接跳到 8 分钟 Boss 战 ----
			if (!preKeys[DIK_I] && keys[DIK_I]) {
				gGameFrame = kGameDurationFrames;  // 直接推进到 8 分钟
				gBossTriggered = false;            // 确保 Boss 触发逻辑重新执行
				updateBoss();                      // 立即调用一次，立刻生成 Boss
			}


		}
		// ===== BGM 状态管理（标题/游戏播放，结果界面停止）=====
		auto ensureBGMPlay = [&]() {
#ifdef NOVICE_HAS_ISPLAYING
			if (bgmHandle >= 0 && !Novice::IsPlayingAudio(bgmHandle)) {
				Novice::PlayAudio(bgmHandle, true, bgmVolume); // true=loop
			}
#else
			if (bgmHandle >= 0 && !bgmPlaying) {
				Novice::PlayAudio(bgmHandle, true, bgmVolume);
				bgmPlaying = true;
			}
#endif
			};
		auto stopBGM = [&]() {
			if (bgmHandle >= 0) {
				Novice::StopAudio(bgmHandle);
				bgmPlaying = false;
			}
			};

		// 标题 & 游戏：保证在播；结果界面：停止
		if (scene == SCENE_TITLE || scene == SCENE_GAME) {
			ensureBGMPlay();
		}
		else if (scene == SCENE_RESULT) {
			stopBGM();
		}


		switch (scene) {
		case SCENE_TITLE: {
			Novice::DrawSprite(0, 0, texTitleBG, 1, 1, 0, 0xFFFFFFFF);
			const int mojiW = 1024, mojiH = 374;
			int tx = kWindowWidth / 2 - mojiW / 2, ty = kWindowHeight / 2 - mojiH / 2;
			Novice::DrawSprite(tx, ty, texTitleMoji, 1, 1, 0, 0xFFFFFFFF);
			Novice::ScreenPrintf(440, 600, "ENTER: Start   P: Pause   R: Title (in-game)");
			if (!preKeys[DIK_RETURN] && keys[DIK_RETURN]) {
				ResetGame(player, cameraX, cameraY, panel); // ← 修复重开：每次开始都彻底清空
				scene = SCENE_GAME;
			}
		}break;

		case SCENE_GAME: {
			// [PATCH] 玩家受伤无敌帧递减
// [PATCH-HURT-TICK] 只有在进行战斗更新时才消耗无敌帧
			if (!gPaused && !panel.active) {
				if (gPlayerHurtCD > 0) --gPlayerHurtCD;
			}

			if (!panel.active && !gPaused) ++gGameFrame;

			float time01 = (float)std::min(gGameFrame, kGameDurationFrames) / (float)kGameDurationFrames;

			// ===== 输入移动 =====
			if (!gPaused && !panel.active) {
				float mx = 0, my = 0; if (keys[DIK_W])my -= 1; if (keys[DIK_S])my += 1; if (keys[DIK_A])mx -= 1; if (keys[DIK_D])mx += 1;
				if (mx != 0 || my != 0) {
					float len = std::sqrt(mx * mx + my * my);
					player.lastDirX = mx / len; player.lastDirY = my / len;
					float v = player.baseSpeed * GetSpeedMul(); // 敌人速度恒定，玩家可提速
					player.x += player.lastDirX * v;
					player.y += player.lastDirY * v;
				}
				player.x = ClampF(player.x, player.r, kMapWidth - player.r);
				player.y = ClampF(player.y, player.r, kMapHeight - player.r);
			}

			cameraX = ClampF(player.x - kWindowWidth / 2.0f, 0.0f, kMapWidth - kWindowWidth);
			cameraY = ClampF(player.y - kWindowHeight / 2.0f, 0.0f, kMapHeight - kWindowHeight);


			// ===== Boss 流程 =====
			updateBoss();
			if (scene == SCENE_RESULT) break; // 立即跳出本场景逻辑渲染结果

			// ===== 升级面板（阻塞更新） =====
			// 开局：没有面板、且需要初始选择 → 给武器三选一（不加等级）
			if (!panel.active && gInitialPickPending) {
				RollInitialWeaponPanel(panel);
			}

			if (panel.active) {
				if (!gPaused) {
					bool p1 = (!preKeys[DIK_1] && keys[DIK_1]);
					bool p2 = (!preKeys[DIK_2] && keys[DIK_2]);
					bool p3 = (!preKeys[DIK_3] && keys[DIK_3]);
					if (p1 || p2 || p3) {
						int sel = p1 ? 0 : (p2 ? 1 : 2);

						if (gInitialPickPending) {
							// 初始面板：只授予武器 Lv1，不加等级、不给经验
							int wid = panel.id[sel];
							if (panel.kind[sel] == UPGRADE_WEAPON && gWeaponLv[wid] <= 0) {
								gWeaponLv[wid] = 1;
								// 对应武器冷却清零（举例：飞镖/大蒜/火箭等有需要就加）
								if (wid == WEAPON_DAGGER)  gCD_Dagger = 0;
								if (wid == WEAPON_GARLIC)  gCD_Garlic = 0;
								if (wid == WEAPON_ROCKET)  gCD_Rocket = 0;
								if (wid == WEAPON_LASER)   gCD_Laser = 0;
								if (wid == WEAPON_LIGHTNING) gCD_Lightning = 0;
								if (wid == WEAPON_MINE)    gCD_Mine = 0;
								if (wid == WEAPON_BULLET)  gCD_Bullet = 0;
							}
							gInitialPickPending = false;
							panel.active = false;
							// 初始选择结束后，如果此刻刚好又升级了（极少见），自动弹正常升级
							if (panel.pendingLevels > 0) { --panel.pendingLevels; RollPanel(panel); }
						}
						else {
							// 正常升级流程（带加等级；注意封顶）
							if (gLevel < kLevelCap) {
								ApplyUpgrade(panel.kind[sel], panel.id[sel], player);
								++gLevel;
							}
							panel.active = false;
							if (panel.pendingLevels > 0) { --panel.pendingLevels; RollPanel(panel); }
						}
					}

				}
				// 跳过战斗更新，下面专门画 UI
			}
			else if (!gPaused) {
				// ===== 武器&系统更新 =====

				// --- Knife（Bullet 前向弹） ---
				if (gWeaponLv[WEAPON_BULLET] > 0) {
					static int timer = 0; --timer;
					int baseInt, shotNum, dmg, pierce; float spMul, spread;
					GetBulletParams(gWeaponLv[WEAPON_BULLET], baseInt, shotNum, spMul, dmg, pierce, spread);
					int cd = (int)std::max(1, (int)(baseInt * GetCooldownMul()));
					pierce += GetBonusPierce();
					if (timer <= 0) {
						float baseAng = std::atan2(player.lastDirY, player.lastDirX);
						if (std::fabs(player.lastDirX) + std::fabs(player.lastDirY) < 0.01f) baseAng = -3.14159f / 2.0f;
						for (int s = 0; s < shotNum; ++s) {
							int bi = -1; for (int i = 0; i < maxBullets; ++i) if (!bullets[i].active) { bi = i; break; } if (bi < 0)break;
							bullets[bi].x = player.x; bullets[bi].y = player.y;
							bullets[bi].r = 5.0f;
							float ang = baseAng;
							if (shotNum > 1) {
								float rad = spread * 3.14159f / 180.0f;
								ang = baseAng + ((s - (shotNum - 1) * 0.5f) * rad / std::max(1, shotNum - 1));
							}
							float dirx = std::cos(ang), diry = std::sin(ang);
							float speed = 12.0f * spMul;
							bullets[bi].vx = dirx * speed; bullets[bi].vy = diry * speed;
							bullets[bi].dmg = dmg; bullets[bi].pierce = pierce; bullets[bi].active = true;
							bullets[bi].lastHitId = -1;
							bullets[bi].lastHitCooldown = 0;

						}
						timer = cd;
					}
				}
				for (int i = 0; i < maxBullets; ++i) if (bullets[i].active) {
					if (bullets[i].lastHitCooldown > 0) --bullets[i].lastHitCooldown;
					bullets[i].x += bullets[i].vx; bullets[i].y += bullets[i].vy;
					if (bullets[i].x<0 || bullets[i].x>kMapWidth || bullets[i].y<0 || bullets[i].y>kMapHeight) bullets[i].active = false;
				}

				// --- Mine ---
				if (gWeaponLv[WEAPON_MINE] > 0) {
					--gCD_Mine; int cd, fuse, dmg, count; float triggerR, rad;
					GetMineParams(gWeaponLv[WEAPON_MINE], cd, fuse, triggerR, rad, dmg, count);
					cd = (int)std::max(1, (int)(cd * GetCooldownMul())); rad *= GetAreaMul();
					if (gCD_Mine <= 0) {
						for (int c = 0; c < count; ++c) SpawnMine(player.x, player.y, fuse, triggerR, dmg);
						gCD_Mine = cd;
					}
				}
				for (int i = 0; i < kMaxMines; ++i) if (gMines[i].active) {
					bool det = false;
					float trigEff = AreaR(gMines[i].triggerR);
					for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
						float dx = enemies[ei].x - gMines[i].x, dy = enemies[ei].y - gMines[i].y;
						if (dx * dx + dy * dy <= trigEff * trigEff) { det = true; break; }
					}

					if (det) {
						int cd, fuse, dmg, count; float trig, rad; GetMineParams(std::max(1, gWeaponLv[WEAPON_MINE]), cd, fuse, trig, rad, dmg, count);
						float areaR = rad * GetAreaMul() * 0.5f; // 爆炸范围 -50%
						SpawnExplosion(gMines[i].x, gMines[i].y, areaR, gMines[i].dmg);

						gMines[i].active = false;
					}
				}
				for (int i = 0; i < kMaxExplos; ++i) if (gExplos[i].active) {
					// life 初始为 12（见 SpawnExplosion），用它当“首帧伤害”的触发器
					if (gExplos[i].life == 12) {
						// 对所有敌人做 AoE 一跳
						for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
							if (CircleHit(gExplos[i].x, gExplos[i].y, gExplos[i].r, enemies[ei].x, enemies[ei].y, enemies[ei].r)) {
								DealDamageEnemy(enemies[ei], gExplos[i].dmg, player);
							}
						}
						// Boss 也吃伤害
						if (gBoss.active) {
							if (CircleHit(gExplos[i].x, gExplos[i].y, gExplos[i].r, gBoss.x, gBoss.y, gBoss.r)) {
								DealDamageBoss(gBoss, gExplos[i].dmg, player);
							}
						}
						// 如果你希望“只显示特效不再多跳”，就不需要任何标记；
						// 我们只在 life==12 的首帧结算一次，所以不会重复伤害。
					}

					if (--gExplos[i].life <= 0) gExplos[i].active = false;
				}

				// --- Garlic（大蒜，周期伤害） ---
				if (gWeaponLv[WEAPON_GARLIC] > 0) {
					int tickCD; float rad; int dmg;
					GetGarlicParams(gWeaponLv[WEAPON_GARLIC], tickCD, rad, dmg);
					// rad 保持基础；命中 CircleHit & 可视圈绘制都改用 AreaR(rad)


					// 冷却计时
					if (--gCD_Garlic <= 0) {
						float rEff = AreaR(rad);
						for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
							if (CircleHit(player.x, player.y, rEff, enemies[ei].x, enemies[ei].y, enemies[ei].r)) {
								DealDamageEnemy(enemies[ei], dmg, player);
							}
						}
						if (gBoss.active && CircleHit(player.x, player.y, rEff, gBoss.x, gBoss.y, gBoss.r)) {
							DealDamageBoss(gBoss, dmg, player);
						}
						gCD_Garlic = (int)std::max(1, (int)(tickCD * GetCooldownMul()));
					}


					gGarlic.active = true;
					gGarlic.radius = rad;
					gGarlic.dmg = dmg;
				}
				else {
					gGarlic.active = false;
				}


				// // --- Magic Wand（飞镖：发射瞬间锁定最近敌人，之后直线飞行） ---

				// [REPLACE 2] —— MagicWand（飞镖）发射：CD 到点 → 一次性发多发，轻微扩散
				// [REPLACE 2] —— MagicWand（飞镖）发射：单发直线 + 最小冷却下限
				if (gWeaponLv[WEAPON_DAGGER] > 0) {
					int cdBase, num, dmg, pierce; float spd;
					GetDaggerParams(gWeaponLv[WEAPON_DAGGER], cdBase, num, spd, dmg, pierce);

					// 强制单发
					num = 1;

					// 冷却：应用冷却BUFF后，再夹一个最小冷却下限（比如 10 帧）
					const int kMinDaggerCD = 10; // 你也可调成 8~12 之间
					int cd = (int)std::max((float)kMinDaggerCD, cdBase * GetCooldownMul());

					// 额外穿透来自全局 Buff
					pierce += GetBonusPierce();

					// CD 递减（仅在 !gPaused && !panel.active 分支里执行）
					--gCD_Dagger;

					if (gCD_Dagger <= 0) {
						// 计算基础朝向：优先最近敌人；没有则用最近移动方向；都没有则向上
						float dirx = 0.0f, diry = -1.0f;
						int idx = FindNearestEnemy(player.x, player.y);
						if (idx >= 0) {
							float dx = enemies[idx].x - player.x, dy = enemies[idx].y - player.y;
							float len = std::sqrt(dx * dx + dy * dy);
							if (len > 1e-6f) { dirx = dx / len; diry = dy / len; }
						}
						else if (std::fabs(player.lastDirX) + std::fabs(player.lastDirY) > 1e-4f) {
							float len = std::sqrt(player.lastDirX * player.lastDirX + player.lastDirY * player.lastDirY);
							dirx = player.lastDirX / len; diry = player.lastDirY / len;
						}

						// 发射一枚
						for (int i = 0; i < kMaxDaggers; ++i) if (!gDaggers[i].active) {
							gDaggers[i].x = player.x; gDaggers[i].y = player.y;
							// [PATCH-DAGGER-SET-RADIUS]
							gDaggers[i].r = GetDaggerBaseRadius(gWeaponLv[WEAPON_DAGGER]);
							gDaggers[i].vx = dirx * spd; gDaggers[i].vy = diry * spd;
							gDaggers[i].dmg = dmg;
							gDaggers[i].life = 240;
							gDaggers[i].pierce = pierce;
							gDaggers[i].active = true;
							gDaggers[i].lastHitId = -1;
							gDaggers[i].lastHitCooldown = 0;
							break;
						}

						// 重置冷却（带最小CD约束）
						gCD_Dagger = cd;
					}
				}


				// [REPLACE 3] —— 飞镖更新 / 碰撞 / 生命周期
				for (int i = 0; i < kMaxDaggers; ++i) if (gDaggers[i].active) {
					// 移动
					gDaggers[i].x += gDaggers[i].vx;
					gDaggers[i].y += gDaggers[i].vy;

					// 减免“同一目标连击”的抖动
					if (gDaggers[i].lastHitCooldown > 0) --gDaggers[i].lastHitCooldown;

					// 出界/寿命到期
					if (--gDaggers[i].life <= 0 ||
						gDaggers[i].x < 0 || gDaggers[i].x > kMapWidth ||
						gDaggers[i].y < 0 || gDaggers[i].y > kMapHeight) {
						gDaggers[i].active = false;
						continue;
					}

					bool consumed = false;
					float drEff = AreaR(gDaggers[i].r);  // 半径受范围 Buff

					// 敌人碰撞
					for (int ei = 0; ei < maxEnemies && !consumed; ++ei) if (enemies[ei].active) {
						if (!CircleHit(gDaggers[i].x, gDaggers[i].y, drEff, enemies[ei].x, enemies[ei].y, enemies[ei].r)) continue;

						// 防抖：最近命中过且在冷却中则跳过
						if (gDaggers[i].lastHitId == ei && gDaggers[i].lastHitCooldown > 0) continue;

						DealDamageEnemy(enemies[ei], gDaggers[i].dmg, player);

						if (gDaggers[i].pierce > 0) {
							gDaggers[i].pierce--;
							gDaggers[i].lastHitId = ei;
							gDaggers[i].lastHitCooldown = 2;
							// 把飞镖“沿速度方向”推出敌体外，避免同帧多次相交
							AdvanceOutOfCircleAlongVelocity(
								gDaggers[i].x, gDaggers[i].y,
								gDaggers[i].vx, gDaggers[i].vy, gDaggers[i].r,
								enemies[ei].x, enemies[ei].y, enemies[ei].r
							);
						}
						else {
							consumed = true;
						}
					}

					// Boss 碰撞
					if (!consumed && gBoss.active) {
						if (CircleHit(gDaggers[i].x, gDaggers[i].y, drEff, gBoss.x, gBoss.y, gBoss.r)) {
							if (!(gDaggers[i].lastHitId == -9999 && gDaggers[i].lastHitCooldown > 0)) {
								DealDamageBoss(gBoss, gDaggers[i].dmg, player);

								if (gDaggers[i].pierce > 0) {
									gDaggers[i].pierce--;
									gDaggers[i].lastHitId = -9999;
									gDaggers[i].lastHitCooldown = 2;

									AdvanceOutOfCircleAlongVelocity(
										gDaggers[i].x, gDaggers[i].y,
										gDaggers[i].vx, gDaggers[i].vy, gDaggers[i].r,
										gBoss.x, gBoss.y, gBoss.r
									);
								}
								else {
									consumed = true;
								}
							}
						}
					}

					if (consumed) gDaggers[i].active = false;
				}


				// --- Lightning Ring（随机落雷，半径吃范围） ---
				if (gWeaponLv[WEAPON_LIGHTNING] > 0) {
					--gCD_Lightning; int cd, hits, dmg, show; float rad;
					GetLightningParams(gWeaponLv[WEAPON_LIGHTNING], cd, hits, dmg, rad, show);
					cd = (int)std::max(1, (int)(cd * GetCooldownMul())); // rad 保持基础值；用时再 AreaR(rad)

					if (gCD_Lightning <= 0) {
						for (int h = 0; h < hits; ++h) {
							int pick = -1, tries = 80;
							while (tries-- > 0) { int i = rand() % maxEnemies; if (enemies[i].active) { pick = i; break; } }
							if (pick >= 0) {
								SpawnLightningAt(enemies[pick].x, enemies[pick].y, dmg, rad);
								float rEff = AreaR(rad);
								for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
									float dx = enemies[ei].x - enemies[pick].x, dy = enemies[ei].y - enemies[pick].y;
									if (dx * dx + dy * dy <= rEff * rEff) DealDamageEnemy(enemies[ei], dmg, player);
								}

							}
						}
						gCD_Lightning = cd;
					}
				}
				for (int i = 0; i < kMaxStrikes; ++i) if (gStrikes[i].active) if (--gStrikes[i].life <= 0) gStrikes[i].active = false;

				// --- Clock Lancet（柳叶刀 射线，长度/宽度/束数×范围） ---
				if (gWeaponLv[WEAPON_LASER] > 0) {
					--gCD_Laser; int cd, life, dmg, beams; float len, hw;
					GetLancetParams(gWeaponLv[WEAPON_LASER], cd, len, hw, life, dmg, beams);
					cd = (int)std::max(1, (int)(cd * GetCooldownMul())); // len/hw 保持基础；绘制/判定处用 AreaR()

					if (gCD_Laser <= 0) {
						float baseAng = std::atan2(player.lastDirY, player.lastDirX);
						if (std::fabs(player.lastDirX) + std::fabs(player.lastDirY) < 0.01f) baseAng = -3.14159f / 2.0f;
						float step = 0.12f; // 每束约6.9°
						for (int b = 0; b < beams; ++b) {
							float off = (b - (beams - 1) * 0.5f) * step;
							SpawnLaser(baseAng + off, len, hw, life, dmg, beams);
						}
						gCD_Laser = cd;
					}
				}
				// [PATCH] Lancet 伤害：激光出现的首帧结一次
				{
					int cdParam, lifeParam, dmgParam, beamsParam;
					float lenParam, hwParam;
					GetLancetParams(std::max(1, gWeaponLv[WEAPON_LASER]),
						cdParam, lenParam, hwParam, lifeParam, dmgParam, beamsParam);

					for (int i = 0; i < kMaxLasers; ++i) if (gLasers[i].active) {
						if (gLasers[i].life == lifeParam) { // 刚生成的第一帧
							float area = GetAreaMul();
							float len = gLasers[i].len * area;
							float radius = std::max(2.0f, gLasers[i].halfWidth * area);
							float ang = gLasers[i].angle;

							float dirx = std::cos(ang), diry = std::sin(ang);
							float sx = player.x, sy = player.y;
							float ex = sx + dirx * len, ey = sy + diry * len;

							// 敌人
							for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
								if (CapsuleCircleHit(sx, sy, ex, ey, radius,
									enemies[ei].x, enemies[ei].y, enemies[ei].r)) {
									DealDamageEnemy(enemies[ei], gLasers[i].dmg, player);
								}
							}
							// Boss
							if (gBoss.active) {
								if (CapsuleCircleHit(sx, sy, ex, ey, radius, gBoss.x, gBoss.y, gBoss.r)) {
									DealDamageBoss(gBoss, gLasers[i].dmg, player);
								}
							}
						}
					}
				}

				// --- King Bible（绕身旋转；接触边沿触发一次伤害） ---
				if (gWeaponLv[WEAPON_ORBITAL] > 0) {
					int balls, dps; float radius, angVel;
					GetBibleParams(gWeaponLv[WEAPON_ORBITAL], balls, radius, dps, angVel);
					radius *= GetAreaMul();                    // 范围 Buff
					gBiblePhase += angVel;

					// 先累计“本帧是否接触”
					static bool hitNow[maxEnemies];            // 900 个以内，栈上 OK；也可改为 static
					std::fill(hitNow, hitNow + maxEnemies, false);
					bool bossHitNow = false;

					for (int b = 0; b < balls; ++b) {
						float ang = gBiblePhase + (2.0f * 3.1415926f * b) / std::max(1, balls);
						float ox = player.x + std::cos(ang) * radius;
						float oy = player.y + std::sin(ang) * radius;
						float rr = 10.0f; // 书页碰撞半径（保持原值）

						for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
							if (CircleHit(ox, oy, rr, enemies[ei].x, enemies[ei].y, enemies[ei].r)) {
								hitNow[ei] = true;             // 记录“本帧有接触”
							}
						}
						if (gBoss.active && CircleHit(ox, oy, rr, gBoss.x, gBoss.y, gBoss.r)) {
							bossHitNow = true;
						}
					}

					// 边沿触发：只有“本帧接触 && 上一帧不接触”才结算一次
					for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
						if (hitNow[ei] && !gBibleOverlapPrev[ei]) {
							DealDamageEnemy(enemies[ei], dps, player);
						}
						// 更新“上一帧接触”状态
						gBibleOverlapPrev[ei] = hitNow[ei];
					}
					if (gBoss.active) {
						if (bossHitNow && !gBibleBossOverlapPrev) {
							DealDamageBoss(gBoss, dps, player);
						}
						gBibleBossOverlapPrev = bossHitNow;
					}
				}


				// --- Rocket（保留，吃范围 for 爆炸） ---
				if (gWeaponLv[WEAPON_ROCKET] > 0) {
					--gCD_Rocket; int cd, dmg; float spd, exR; GetRocketParams(gWeaponLv[WEAPON_ROCKET], cd, spd, dmg, exR);
					cd = (int)std::max(1, (int)(cd * GetCooldownMul()));
					if (gCD_Rocket <= 0) {
						int idx = FindNearestEnemy(player.x, player.y);
						float vx = 0, vy = -1;
						if (idx >= 0) {
							float dx = enemies[idx].x - player.x, dy = enemies[idx].y - player.y;
							float len = std::sqrt(dx * dx + dy * dy); if (len > 0) { vx = dx / len; vy = dy / len; }
						}
						SpawnRocket(player.x, player.y, vx * spd, vy * spd, dmg); gCD_Rocket = cd;
					}
				}
				for (int i = 0; i < kMaxRockets; ++i) if (gRockets[i].active) {
					int idx = FindNearestEnemy(gRockets[i].x, gRockets[i].y);
					if (idx >= 0) {
						float dx = enemies[idx].x - gRockets[i].x, dy = enemies[idx].y - gRockets[i].y;
						float len = std::sqrt(dx * dx + dy * dy); if (len > 0) {
							float dirx = dx / len, diry = dy / len;
							float spd = std::sqrt(gRockets[i].vx * gRockets[i].vx + gRockets[i].vy * gRockets[i].vy);
							gRockets[i].vx = gRockets[i].vx * 0.9f + dirx * spd * 0.1f;
							gRockets[i].vy = gRockets[i].vy * 0.9f + diry * spd * 0.1f;
						}
					}
					gRockets[i].x += gRockets[i].vx; gRockets[i].y += gRockets[i].vy;
					bool boom = false;
					for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
						if (CircleHit(gRockets[i].x, gRockets[i].y, 6, enemies[ei].x, enemies[ei].y, enemies[ei].r)) { boom = true; break; }
					}
					if (gBoss.active && CircleHit(gRockets[i].x, gRockets[i].y, 6, gBoss.x, gBoss.y, gBoss.r)) boom = true;
					if (boom || --gRockets[i].life <= 0) {
						int cd; float spd, exR; int dmg; GetRocketParams(std::max(1, gWeaponLv[WEAPON_ROCKET]), cd, spd, dmg, exR);
						float areaR = exR * GetAreaMul() * 0.5f;    // 范围 Buff 后再减半
						SpawnExplosion(gRockets[i].x, gRockets[i].y, areaR, dmg);




						gRockets[i].active = false;
					}
					if (gRockets[i].x<0 || gRockets[i].x>kMapWidth || gRockets[i].y<0 || gRockets[i].y>kMapHeight) gRockets[i].active = false;
				}

				// ===== 敌人移动/重定位 =====
				for (int i = 0; i < maxEnemies; ++i) if (enemies[i].active) {
					if ((gGameFrame % 30) == (i % 30)) {
						float margin = 160.0f;
						if (!InRect(enemies[i].x, enemies[i].y, cameraX - margin, cameraY - margin, kWindowWidth + margin * 2, kWindowHeight + margin * 2))
							RehomeEnemyToViewportEdge(enemies[i], cameraX, cameraY);
					}
					if (enemies[i].freezeTimer > 0) { enemies[i].freezeTimer--; continue; }
					float mul = 1.0f;
					if (enemies[i].slowTimer > 0) { enemies[i].slowTimer--; mul = enemies[i].slowFactor; if (enemies[i].slowTimer <= 0) enemies[i].slowFactor = 1.0f; }
					float dx = player.x - enemies[i].x, dy = player.y - enemies[i].y; float len = std::sqrt(dx * dx + dy * dy);
					if (len > 0) { enemies[i].x += (dx / len) * enemies[i].speed * mul; enemies[i].y += (dy / len) * enemies[i].speed * mul; }
				}
				SeparateEnemies(enemies, maxEnemies);

				// ===== 碰撞：子弹/飞杖 命中 =====

   // ---------- 子弹（Bullet） ----------
				for (int bi = 0; bi < maxBullets; ++bi) if (bullets[bi].active) {
					bool consumed = false;

					// 敌人命中
					for (int ei = 0; ei < maxEnemies && !consumed; ++ei) if (enemies[ei].active) {
						// 如果子弹半径也吃范围 Buff，就用 AreaR(bullets[bi].r)；否则就用原值。
						float brEff = AreaR(bullets[bi].r);
						if (!CircleHit(bullets[bi].x, bullets[bi].y, brEff, enemies[ei].x, enemies[ei].y, enemies[ei].r)) continue;

						// 防抖（同飞镖的 lastHit 机制）
						if (bullets[bi].lastHitId == ei && bullets[bi].lastHitCooldown > 0) continue;

						DealDamageEnemy(enemies[ei], bullets[bi].dmg, player);

						if (bullets[bi].pierce > 0) {
							bullets[bi].pierce--;
							bullets[bi].lastHitId = ei;
							bullets[bi].lastHitCooldown = 2;

							// 把子弹沿速度方向推出去，避免同一帧再次重叠
							AdvanceOutOfCircleAlongVelocity(
								bullets[bi].x, bullets[bi].y,
								bullets[bi].vx, bullets[bi].vy, bullets[bi].r,
								enemies[ei].x, enemies[ei].y, enemies[ei].r
							);
						}
						else {
							consumed = true;
						}
					}

					// Boss 命中
					if (gBoss.active && !consumed) {
						float brEff = AreaR(bullets[bi].r);
						if (CircleHit(bullets[bi].x, bullets[bi].y, brEff, gBoss.x, gBoss.y, gBoss.r)) {
							if (!(bullets[bi].lastHitId == -9999 && bullets[bi].lastHitCooldown > 0)) {
								DealDamageBoss(gBoss, bullets[bi].dmg, player);

								if (bullets[bi].pierce > 0) {
									bullets[bi].pierce--;
									bullets[bi].lastHitId = -9999;
									bullets[bi].lastHitCooldown = 2;

									AdvanceOutOfCircleAlongVelocity(
										bullets[bi].x, bullets[bi].y,
										bullets[bi].vx, bullets[bi].vy, bullets[bi].r,
										gBoss.x, gBoss.y, gBoss.r
									);
								}
								else {
									consumed = true;
								}
							}
						}
					}

					if (consumed) bullets[bi].active = false;
					if (bullets[bi].lastHitCooldown > 0) --bullets[bi].lastHitCooldown;
				}


				// ===== 玩家被撞 =====
				// 敌人碰撞伤害（同一帧至多掉一次血）
				{
					bool hurtOnce = false;
					for (int ei = 0; ei < maxEnemies; ++ei) if (enemies[ei].active) {
						if (CircleHit(player.x, player.y, player.r, enemies[ei].x, enemies[ei].y, enemies[ei].r)) {

							// 碰到就把这只怪移除（保留你原逻辑）
							enemies[ei].active = false;

							// 本帧还没掉过血 → 尝试扣血
							if (!hurtOnce) {
								if (TryHurtPlayer(player, /*敌人基准×10*/20)) {
									hurtOnce = true;

									// 轻微把玩家从碰撞里推出来，防止 i-frame 结束立刻再次重叠
									float dx = player.x - enemies[ei].x, dy = player.y - enemies[ei].y;
									float len = std::sqrt(dx * dx + dy * dy);
									if (len < 1e-4f) { dx = 1.0f; dy = 0.0f; len = 1.0f; }
									float push = 6.0f; // 推开像素，可按手感调 4~10
									player.x += dx / len * push;
									player.y += dy / len * push;
								}
							}
						}
					}
				}


				// Boss 碰撞伤害
				if (gBoss.active && CircleHit(player.x, player.y, player.r, gBoss.x, gBoss.y, gBoss.r)) {
					if (TryHurtPlayer(player, /*Boss 基准×10*/60)) {
						// 同样轻微推开玩家
						float dx = player.x - gBoss.x, dy = player.y - gBoss.y;
						float len = std::sqrt(dx * dx + dy * dy);
						if (len < 1e-4f) { dx = 1.0f; dy = 0.0f; len = 1.0f; }
						float push = 8.0f; // Boss 推开更大一点
						player.x += dx / len * push;
						player.y += dy / len * push;
					}
				}



				// ===== 经验球 =====
				float baseR = Lerp(kMagnetRadiusMinBase, kMagnetRadiusMaxBase, time01);
				float magnetR = baseR + GetMagnetBonus();
				for (int oi = 0; oi < maxOrbs; ++oi) if (orbs[oi].active) {
					float dx = player.x - orbs[oi].x, dy = player.y - orbs[oi].y; float d2 = dx * dx + dy * dy;
					if (!orbs[oi].attracted && d2 <= magnetR * magnetR) orbs[oi].attracted = true;
					if (orbs[oi].attracted) {
						float d = std::sqrt(d2); if (d > 0) { orbs[oi].x += (dx / d) * kMagnetSpeed; orbs[oi].y += (dy / d) * kMagnetSpeed; }
					}
					float adx = player.x - orbs[oi].x, ady = player.y - orbs[oi].y;
					if (adx * adx + ady * ady <= (player.r + orbs[oi].r) * (player.r + orbs[oi].r)) {
						// [PATCH-XP] 经验Buff按倍率累计（含小数累进，保证不丢失）
						static float s_xpFrac = 0.0f;                 // 小数仓库（静态，跨拾取累计）

						int   base = orbs[oi].value;                 // 经验球基础值
						int   lvXP = std::clamp(gBuffLv[BUFF_XP], 0, 5);
						static const float xpMul[6] = { 1.0f, 1.06f, 1.12f, 1.20f, 1.30f, 1.45f };

						float total = base * xpMul[lvXP];             // 实际应得（带小数）
						int   add = (int)total;                     // 先取整部分
						s_xpFrac += (total - add);                  // 小数累到仓库

						// 小数凑够 1 就再加 1，并扣掉
						while (s_xpFrac >= 1.0f) {
							++add;
							s_xpFrac -= 1.0f;
						}

						if (add < 1) add = 1;                         // 至少+1（防极端情况）
						gXP += add;

						while (true) {
							int vLv = gLevel + (panel.active ? 1 : 0) + panel.pendingLevels;
							if (vLv > kLevelCap) vLv = kLevelCap;

							int need = NeedXP_4Bands(vLv);
							if (gLevel >= kLevelCap) { gXP = 0; break; }  // 满级后不再累计

							if (gXP >= need) {
								gXP -= need;
								if (!panel.active) RollPanel(panel);      // 弹一次升级
								else ++panel.pendingLevels;               // 已在面板中：叠待处理
							}
							else break;
						}

						orbs[oi].active = false;
					}
				}

				// ===== HP 自然回复（Hp Lv5） =====
				if (gBuffLv[BUFF_HP] >= 5 && player.hp > 0 && player.hp < player.maxHP) {
					if (++gHPRegenTimer >= gHPRegenCD) { ++player.hp; if (player.hp > player.maxHP)player.hp = player.maxHP; gHPRegenTimer = 0; }
				}

				// ===== 刷怪（Boss 阶段停止） =====
				if (!gBoss.active) spawnEnemiesTick(time01);
			}

			// ======== 绘制（Game） ========
			// 背景
			Novice::DrawSprite((int)(-cameraX), (int)(-cameraY), texGameBG, 1, 1, 0, 0xFFFFFFFF);

			// 顶部计时/倒计时
			if (!gBoss.active) {
				int framesLeft = std::max(0, kGameDurationFrames - std::min(gGameFrame, kGameDurationFrames));
				int sec = framesLeft / kFPS, mm = sec / 60, ss = sec % 60;
				Novice::ScreenPrintf(kWindowWidth / 2 - 150, 8, "TIME LEFT %02d:%02d   (P:Pause  R:Title)", mm, ss);
			}
			else {
				int sec = gBossTimer / kFPS; int ss = gBossTimer % kFPS;
				Novice::ScreenPrintf(kWindowWidth / 2 - 60, 8, "BOSS %02d:%02d", sec, ss);
			}

			// 经验球
			for (int oi = 0; oi < maxOrbs; ++oi) if (orbs[oi].active)
				Novice::DrawEllipse((int)(orbs[oi].x - cameraX), (int)(orbs[oi].y - cameraY), (int)orbs[oi].r, (int)orbs[oi].r, 0, 0xFFFF00FF, kFillModeSolid);

			// 玩家
			// 玩家（贴图替换，尺寸按半径自动缩放到直径）
			

			// 大蒜光环（可视化）：半透明红色实心圆
			if (gGarlic.active) {
				int rVis = (int)AreaR(gGarlic.radius);
				Novice::DrawEllipse((int)(player.x - cameraX), (int)(player.y - cameraY),
					rVis, rVis, 0, 0xFF000044, kFillModeSolid);

			}



			// 子弹/地雷/爆炸/飞杖/落雷/柳叶刀/火箭
			for (int i = 0; i < maxBullets; ++i) if (bullets[i].active) {
				// bullets[i].r 已经按 Area Buff 放大了，这里用它来同步视觉尺寸
				DrawSpriteByRadius(texBullet, kTexBulletW, kTexBulletH,
					bullets[i].x, bullets[i].y, AreaR(bullets[i].r),
					0.0f, 0xFFFFFFFF, cameraX, cameraY);

			}

			for (int i = 0; i < kMaxMines; ++i) if (gMines[i].active) {
				// 让地雷贴图看起来随“范围”变大：
				// 这里用“触发半径 triggerR”作为可视半径的基准（更直观）；也可以改用 gMines[i].r
// 可视贴图大小 = 固定，不吃范围 Buff
				float visR = gMines[i].r; // 保持图标大小稳定
				DrawSpriteByRadius(
					texMine, kTexMineW, kTexMineH,
					gMines[i].x, gMines[i].y, visR,
					0.0f, 0xFFFFFFFF,
					cameraX, cameraY
				);

				// 触发圈：显示真实范围（吃 Area Buff），方便玩家判断
				Novice::DrawEllipse(
					(int)(gMines[i].x - cameraX), (int)(gMines[i].y - cameraY),
					(int)(gMines[i].triggerR * GetAreaMul()), (int)(gMines[i].triggerR * GetAreaMul()),
					0, 0xFFFFFF44, kFillModeWireFrame
				);

			}

			for (int i = 0; i < kMaxExplos; ++i) if (gExplos[i].active) {
				int rVis = (int)AreaR(gExplos[i].r); // 存基础 -> 画时放大
				Novice::DrawEllipse((int)(gExplos[i].x - cameraX), (int)(gExplos[i].y - cameraY),
					rVis, rVis, 0, 0x55AAFFFF, kFillModeWireFrame);

			}
			// [REPLACE 4] —— 飞镖绘制（纹理缺失时兜底小圆点）
			for (int i = 0; i < kMaxDaggers; ++i) if (gDaggers[i].active) {
				float ang = std::atan2(gDaggers[i].vy, gDaggers[i].vx);
				if (texDagger > 0) {
					DrawSpriteByRadius(
						texDagger, kTexDaggerW, kTexDaggerH,
						gDaggers[i].x, gDaggers[i].y, AreaR(gDaggers[i].r),
						ang, 0xFFFFFFFF, cameraX, cameraY
					);
				}
				else {
					int rr = (int)AreaR(gDaggers[i].r);
					Novice::DrawEllipse((int)(gDaggers[i].x - cameraX), (int)(gDaggers[i].y - cameraY),
						rr, rr, 0, 0xFFFFFFFF, kFillModeSolid);
				}
			}

			for (int i = 0; i < kMaxStrikes; ++i) if (gStrikes[i].active) {
				// 世界坐标（统一再减相机，避免取整误差）
				float wx = gStrikes[i].tx;
				float wy = gStrikes[i].ty;

				// （可选）底端向下微调像素，贴地感更强；不需要就设 0
				float groundNudge = 0.0f; // 试试 0 / +2 / +4
				wy += groundNudge;

				// 1) 贴图：底部居中对齐到冲击点
// 用范围乘子放大闪电贴图（宽高一起放大；也可以只放大高度，看你美术偏好）
				float lightningScale = GetAreaMul();
				DrawSpriteBottomCenter(
					texLightning, kTexLightningW, kTexLightningH,
					wx, wy, lightningScale, lightningScale, 0.0f, 0xFFFFFFFF,
					cameraX, cameraY
				);


				// 2) 爆炸范围：以同一个冲击点为圆心
				int cx = (int)std::round(wx - cameraX);
				int cy = (int)std::round(wy - cameraY);
				int rVis = (int)AreaR(gStrikes[i].radius);
				Novice::DrawEllipse(
					cx, cy,
					rVis, rVis,
					0, 0xFFFFFFFF, kFillModeWireFrame
				);

			}

			for (int i = 0; i < kMaxLasers; ++i) if (gLasers[i].active) {
				float ang = gLasers[i].angle;
				float area = GetAreaMul();
				float thickness = std::max(2.0f, gLasers[i].halfWidth * 2.0f * area); // 粗细吃范围
				float length = gLasers[i].len * area;                               // 长度吃范围
				unsigned col = 0xFF2222FF;
				DrawThickLinePrimitive(player.x, player.y, ang, length, thickness, cameraX, cameraY, col);


			}

			for (int i = 0; i < kMaxRockets; ++i) if (gRockets[i].active) {
				int cx = (int)(gRockets[i].x - cameraX), cy = (int)(gRockets[i].y - cameraY);
				Novice::DrawEllipse(cx, cy, 4, 4, 0, 0xFF8844FF, kFillModeSolid);
			}
			// King Bible 可视化（小圆点表示书页，半径吃 Area）
			if (gWeaponLv[WEAPON_ORBITAL] > 0) {
				int balls, dps; float radius, angVel;
				GetBibleParams(gWeaponLv[WEAPON_ORBITAL], balls, radius, dps, angVel);
				radius *= GetAreaMul();
				float rr = 10.0f * GetAreaMul(); // 与判定同步放大
				for (int b = 0; b < balls; ++b) {
					float ang = gBiblePhase + (2.0f * 3.1415926f * b) / std::max(1, balls);
					float ox = player.x + std::cos(ang) * radius;
					float oy = player.y + std::sin(ang) * radius;
					Novice::DrawEllipse((int)(ox - cameraX), (int)(oy - cameraY),
						(int)rr, (int)rr, 0, 0x88FFFFFF, kFillModeSolid);
				}
			}

			// 敌人
			for (int j = 0; j < maxEnemies; ++j) if (enemies[j].active) {
				EnemyTier t = enemies[j].tier;
				int tex = texEnemy[(int)t];
				int tw = kTexEnemyW[(int)t];
				int th = kTexEnemyH[(int)t];

				// 1) 敌人贴图（按半径等比缩放到直径）
				DrawSpriteByRadius(
					tex, tw, th,
					enemies[j].x, enemies[j].y, enemies[j].r,
					0.0f, 0xFFFFFFFF, cameraX, cameraY
				);

				// 2) 血条
				float er = (float)enemies[j].hp / (float)enemies[j].maxHP; er = ClampF(er, 0, 1);
				int ew = (int)(enemies[j].r * 2);
				int ex = (int)(enemies[j].x - enemies[j].r - cameraX);
				int ey = (int)(enemies[j].y - enemies[j].r - cameraY);
				int bw = ew < 20 ? 20 : ew, bh = 4; int bx = ex, by = ey - 8;
				Novice::DrawBox(bx, by, bw, bh, 0, 0x222222AA, kFillModeSolid);
				Novice::DrawBox(bx, by, (int)(bw * er), bh, 0, 0xFF4444FF, kFillModeSolid);
			}


			// Boss
			// [PATCH-BOSS-DRAW] 用贴图按半径绘制
			if (gBoss.active) {
				DrawSpriteByRadius(
					texBoss, kTexBossW, kTexBossH,
					gBoss.x, gBoss.y, gBoss.r,
					0.0f, 0xFFFFFFFF, cameraX, cameraY
				);

				// 顶部大血条（保留）
				int barW = kWindowWidth - 200, barH = 16, barX = 100, barY = 40;
				Novice::DrawBox(barX, barY, barW, barH, 0, 0x222222AA, kFillModeSolid);
				float rr = (float)gBoss.hp / (float)std::max(1, gBoss.maxHP); rr = ClampF(rr, 0, 1);
				Novice::DrawBox(barX, barY, (int)(barW * rr), barH, 0, 0xAA22FFFF, kFillModeSolid);
				Novice::ScreenPrintf(barX + barW / 2 - 40, barY - 18, "BOSS HP");
			}

			{
				DrawSpriteByRadius(
					texPlayer, kTexPlayerW, kTexPlayerH,
					player.x, player.y, player.r,
					0.0f, 0xFFFFFFFF, cameraX, cameraY
				);

				// 保留原有血条
				int w = (int)(player.r * 2), h = w;
				int barW = 64, barH = 6;
				int bx = (int)(player.x - cameraX) - barW / 2;
				int by = (int)(player.y - cameraY) - (h / 2) - 12;
				float ratio = (float)player.hp / (float)player.maxHP; ratio = ClampF(ratio, 0, 1);
				Novice::DrawBox(bx, by, barW, barH, 0, 0x222222AA, kFillModeSolid);
				Novice::DrawBox(bx, by, (int)(barW * ratio), barH, 0, 0x00FF00FF, kFillModeSolid);
			}
			// 左上 HUD
 // ======================================
 // 行1：武器（先画空框 -> 再填已拥有）
 // 行2：Buff  （先画空框 -> 再填已拥有）
 // ======================================
			Novice::ScreenPrintf(HUD_X, HUD_Y, "SCORE: %d", gScore);


			// 小工具：画空框
			auto DrawEmptySlot = [&](int x, int y) {
				Novice::DrawBox(x - 2, y - 2, UI_ICON_SIZE + 4, UI_ICON_SIZE + 4, 0, 0xFFFFFFFF, kFillModeWireFrame); // 白色边
				Novice::DrawBox(x, y, UI_ICON_SIZE, UI_ICON_SIZE, 0, 0x00000044, kFillModeSolid);                       // 半透明底
				};

			int row1_y = HUD_Y + 22;
			int row2_y = row1_y + UI_ICON_SIZE + UI_ROW_GAP;
			int row_x0 = HUD_X;

			// ---- 行1：武器空框 ----
			for (int i = 0; i < gMaxWeaponCap; ++i) {
				int x = row_x0 + i * (UI_ICON_SIZE + UI_ICON_PAD);
				DrawEmptySlot(x, row1_y);
			}
			// ---- 行2：Buff 空框 ----
			for (int i = 0; i < gMaxBuffCap; ++i) {
				int x = row_x0 + i * (UI_ICON_SIZE + UI_ICON_PAD);
				DrawEmptySlot(x, row2_y);
			}

			// ---- 回填“已拥有的武器图标”（从左至右，不超过上限个数）----
			{
				int col = 0;
				for (int w = 0; w < WEAPON_COUNT && col < gMaxWeaponCap; ++w) {
					if (gWeaponLv[w] <= 0) continue;
					int x = row_x0 + col * (UI_ICON_SIZE + UI_ICON_PAD);
					const char* fallback = WeaponNames[w]; // 无贴图时用文字缩写
					DrawIconWithLv(texUIWeapon[w], x, row1_y, gWeaponLv[w], false, fallback);
					++col;
				}
			}

			// ---- 回填“已拥有的 Buff 图标”（从左至右，不超过上限个数）----
			{
				int col = 0;
				for (int b = 0; b < BUFF_COUNT && col < gMaxBuffCap; ++b) {
					if (gBuffLv[b] <= 0) continue;
					int x = row_x0 + col * (UI_ICON_SIZE + UI_ICON_PAD);
					DrawIconWithLv(texUIBuff[b], x, row2_y, gBuffLv[b], false, kBuffNameCN[b]);
					++col;
				}
			}


			// 底部经验条
			{
				int barMX = 40, barMY = 24, barX = barMX, barW = kWindowWidth - barMX * 2, barH = 20, barY = kWindowHeight - barMY - barH;
				Novice::DrawBox(barX, barY, barW, barH, 0, 0x202020AA, kFillModeSolid);
				int need = NeedXP_4Bands(gLevel);
				float ratio = (gLevel >= kLevelCap) ? 1.0f : (float)gXP / (float)need;
				Novice::DrawBox(barX, barY, (int)(barW * ratio), barH, 0, 0x00CCFFFF, kFillModeSolid);
				int percent = (int)(ratio * 100.0f + 0.5f);
				Novice::ScreenPrintf(barX, barY - 18, "LV %d   %d%%   XP %d / %d   (P:Pause / R:Title)", gLevel, percent, gXP, need);
			}

			// 升级面板
			if (panel.active) {
				int panelW = 560, panelH = 300, panelX = (kWindowWidth - panelW) / 2, panelY = (kWindowHeight - panelH) / 2;
				Novice::DrawBox(panelX, panelY, panelW, panelH, 0, 0x000000AA, kFillModeSolid);
				Novice::DrawBox(panelX, panelY, panelW, panelH, 0, 0xFFFFFFFF, kFillModeWireFrame);
				Novice::ScreenPrintf(panelX + 20, panelY + 20, "LEVEL UP! Choose (1/2/3)");
				for (int i = 0; i < 3; ++i) {
					int rowY = panelY + 60 + i * 60; Novice::ScreenPrintf(panelX + 20, rowY + 8, "%d)", i + 1);
					int iconTex = (panel.kind[i] == UPGRADE_WEAPON ? texUIWeapon[panel.id[i]] : texUIBuff[panel.id[i]]);
					if (panel.kind[i] == UPGRADE_WEAPON && iconTex == 0)
						DrawIconWithLv(0, panelX + 50, rowY, gWeaponLv[panel.id[i]], false, WeaponNames[panel.id[i]]);
					else
						Novice::DrawSprite(panelX + 50, rowY, iconTex, 1, 1, 0, 0xFFFFFFFF);

					if (panel.kind[i] == UPGRADE_WEAPON) {
						int cur = gWeaponLv[panel.id[i]], nxt = std::min(cur + 1, 8);
						Novice::ScreenPrintf(panelX + 50 + UI_ICON_SIZE + 12, rowY + 6, "%s  Lv %d -> %d", WeaponNames[panel.id[i]], cur, nxt);
					}
					else {
						int cur = gBuffLv[panel.id[i]], nxt = std::min(cur + 1, MAX_LV_EACH);
						Novice::ScreenPrintf(panelX + 50 + UI_ICON_SIZE + 12, rowY + 0, "Buff: %s  Lv %d -> %d", kBuffNameCN[panel.id[i]], cur, nxt);
						std::string en = BuffDescEN(panel.id[i], nxt);
						Novice::ScreenPrintf(panelX + 50 + UI_ICON_SIZE + 12, rowY + 18, "EN: %s", en.c_str());
					}
					if (panel.kind[i] == UPGRADE_WEAPON) {
						int cur = gWeaponLv[panel.id[i]], nxt = std::min(cur + 1, 8);
						Novice::ScreenPrintf(panelX + 50 + UI_ICON_SIZE + 12, rowY + 6, "%s  Lv %d -> %d", WeaponNames[panel.id[i]], cur, nxt);
						std::string wdesc = WeaponDesc(panel.id[i], nxt);
						Novice::ScreenPrintf(panelX + 50 + UI_ICON_SIZE + 12, rowY + 18, "%s", wdesc.c_str());  // ← 新增：描述
					}
				}
			}

			// 伤害数字
			for (int i = 0; i < kMaxDmgText; ++i) if (gDmg[i].life > 0) {
				int sx = (int)(gDmg[i].x - cameraX), sy = (int)(gDmg[i].y - cameraY);
				if (gDmg[i].crit) Novice::ScreenPrintf(sx - 6, sy, "%d!", gDmg[i].val);
				else             Novice::ScreenPrintf(sx - 6, sy, "%d", gDmg[i].val);
				gDmg[i].y += gDmg[i].vy; --gDmg[i].life;
			}

			// 暂停覆盖
			if (gPaused) {
				Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0, 0x00000088, kFillModeSolid);
				Novice::ScreenPrintf(kWindowWidth / 2 - 30, kWindowHeight / 2 - 10, "PAUSED");
			}

			// 帧末：Lancet/Laser 生命周期递减
			for (int i = 0; i < kMaxLasers; ++i) if (gLasers[i].active) { if (--gLasers[i].life <= 0) gLasers[i].active = false; }
			if (scene == SCENE_GAME && player.hp <= 0) {
				scene = SCENE_RESULT;
				gWin = false;
			}

		}break;

		case SCENE_RESULT: {
			Novice::DrawSprite(0, 0, texResultBG, 1, 1, 0, 0xFFFFFFFF);
			Novice::ScreenPrintf(460, 560, gWin ? "RESULT: YOU SURVIVED (BOSS DOWN)" : "RESULT: YOU FAILED");
			Novice::ScreenPrintf(560, 600, "FINAL SCORE: %d", gScore);
			Novice::ScreenPrintf(480, 640, "ENTER: Title");
			if (!preKeys[DIK_RETURN] && keys[DIK_RETURN]) { scene = SCENE_TITLE; }
		}break;
		}

		Novice::EndFrame();
	}

	Novice::Finalize();
	return 0;
}

