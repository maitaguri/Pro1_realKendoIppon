#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

// Game Constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 80
#define MOVE_SPEED 200.0f
#define DASH_SPEED 600.0f
#define MAX_STAMINA 100.0f
#define STAMINA_REGEN 15.0f
#define STAMINA_COST_DASH 30.0f
#define STAMINA_COST_ATTACK 20.0f
#define STAMINA_COST_BLOCK 10.0f

typedef enum {
    STATE_IDLE,
    STATE_MOVING_FORWARD,
    STATE_MOVING_BACKWARD,
    STATE_DASHING,
    STATE_ATTACKING_MEN,
    STATE_ATTACKING_KOTE,
    STATE_ATTACKING_DO,
    STATE_BLOCKING,
    STATE_RECOILING,
    STATE_COOLDOWN
} PlayerState;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    int direction; // 1 for right, -1 for left
    float speed;
    float stamina; 
    float maxStamina;
    int score;
    PlayerState state;
    float stateTimer; // For tracking duration of attacks/dashes
    Rectangle bodyHitbox;
    Rectangle weaponHitbox; // Active only during attack frames
    Vector2 attackOrigin; // Position where attack was initiated
    Color color;
    
    // Input mapping
    KeyboardKey keyLeft;
    KeyboardKey keyRight;
    KeyboardKey keyDash;
    KeyboardKey keyAttackMen;
    KeyboardKey keyAttackKote;
    KeyboardKey keyAttackDo;
    KeyboardKey keyBlock;
} Player;

// Function Prototypes
void InitPlayer(Player *player, int isPlayer1);
void UpdatePlayer(Player *player, float delta);
void DrawPlayer(Player *player);
void ResolveCollision(Player *p1, Player *p2);

#endif
