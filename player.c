#include "player.h"
#include <math.h>

void InitPlayer(Player *player, int isPlayer1) {
    if (isPlayer1) {
        player->position = (Vector2){ 200, 300 };
        player->color = BLUE;
        player->direction = 1;
        player->keyLeft = KEY_LEFT;
        player->keyRight = KEY_RIGHT;
        player->keyDash = KEY_ENTER; // Example alternate mapping or similar
        player->keyAttackMen = KEY_Z;
        player->keyAttackKote = KEY_X;
        player->keyAttackDo = KEY_C;
        player->keyBlock = KEY_DOWN;
    } else {
        player->position = (Vector2){ 600, 300 };
        player->color = RED;
        player->direction = -1;
        player->keyLeft = KEY_A;
        player->keyRight = KEY_D;
        player->keyDash = KEY_LEFT_SHIFT;
        player->keyAttackMen = KEY_R;
        player->keyAttackKote = KEY_T;
        player->keyAttackDo = KEY_Y;
        player->keyBlock = KEY_S;
    }
    
    player->velocity = (Vector2){ 0, 0 };
    player->speed = MOVE_SPEED;
    player->maxStamina = MAX_STAMINA;
    player->stamina = MAX_STAMINA;
    player->score = 0;
    player->state = STATE_IDLE;
    player->stateTimer = 0.0f;
    
    player->bodyHitbox = (Rectangle){ player->position.x, player->position.y, PLAYER_WIDTH, PLAYER_HEIGHT };
    player->weaponHitbox = (Rectangle){ 0, 0, 0, 0 };
    player->attackOrigin = (Vector2){ 0, 0 };
}

void UpdatePlayer(Player *player, float delta) {
    // Stamina Regeneration (only if not blocking or attacking)
    if (player->state == STATE_IDLE || player->state == STATE_MOVING_FORWARD || player->state == STATE_MOVING_BACKWARD) {
        if (player->stamina < player->maxStamina) {
            player->stamina += STAMINA_REGEN * delta;
            if (player->stamina > player->maxStamina) player->stamina = player->maxStamina;
        }
    }

    // State Machine
    switch (player->state) {
        case STATE_IDLE:
        case STATE_MOVING_FORWARD:
        case STATE_MOVING_BACKWARD:
            player->state = STATE_IDLE; // Convert moving back to idle to re-evaluate input
            
            // Movement
            if (IsKeyDown(player->keyRight)) {
                player->position.x += player->speed * delta;
                if (player->direction == 1) player->state = STATE_MOVING_FORWARD;
                else player->state = STATE_MOVING_BACKWARD;
            } else if (IsKeyDown(player->keyLeft)) {
                player->position.x -= player->speed * delta;
                if (player->direction == -1) player->state = STATE_MOVING_FORWARD;
                else player->state = STATE_MOVING_BACKWARD;
            }

            // Boundary Check
            if (player->position.x < 0) player->position.x = 0;
            if (player->position.x > SCREEN_WIDTH - PLAYER_WIDTH) player->position.x = SCREEN_WIDTH - PLAYER_WIDTH;

            // Actions
            if (IsKeyPressed(player->keyDash) && player->stamina >= STAMINA_COST_DASH) {
                player->state = STATE_DASHING;
                player->stateTimer = 0.0f;
                player->stamina -= STAMINA_COST_DASH;
            } else if (IsKeyPressed(player->keyAttackMen) && player->stamina >= STAMINA_COST_ATTACK) {
                player->state = STATE_ATTACKING_MEN;
                player->stateTimer = 0.0f;
                player->attackOrigin = player->position;
                player->stamina -= STAMINA_COST_ATTACK;
            } else if (IsKeyPressed(player->keyAttackKote) && player->stamina >= STAMINA_COST_ATTACK) {
                player->state = STATE_ATTACKING_KOTE;
                player->stateTimer = 0.0f;
                player->attackOrigin = player->position;
                player->stamina -= STAMINA_COST_ATTACK;
            } else if (IsKeyPressed(player->keyAttackDo) && player->stamina >= STAMINA_COST_ATTACK) {
                player->state = STATE_ATTACKING_DO;
                player->stateTimer = 0.0f;
                player->attackOrigin = player->position;
                player->stamina -= STAMINA_COST_ATTACK;
            } else if (IsKeyDown(player->keyBlock) && player->stamina > 0) {
                player->state = STATE_BLOCKING;
            }
            break;

        case STATE_BLOCKING:
            player->stamina -= STAMINA_COST_BLOCK * delta;
            if (!IsKeyDown(player->keyBlock) || player->stamina <= 0) {
                player->state = STATE_IDLE;
            }
            break;

        case STATE_DASHING:
            player->position.x += player->direction * DASH_SPEED * delta;
            player->stateTimer += delta;
            if (player->stateTimer > 0.2f) { // Dash duration
                player->state = STATE_IDLE;
            }
            // Boundary Check during dash
            if (player->position.x < 0) player->position.x = 0;
            if (player->position.x > SCREEN_WIDTH - PLAYER_WIDTH) player->position.x = SCREEN_WIDTH - PLAYER_WIDTH;
            break;

        case STATE_ATTACKING_MEN:
        case STATE_ATTACKING_KOTE:
        case STATE_ATTACKING_DO:
            player->stateTimer += delta;
            // Display for exactly 0.5s
            if (player->stateTimer <= 0.5f) {
                // Set hitbox based on attack type and accumulated origin
                float reach = 60.0f;
                float heightOffset = 0;
                float height = 20.0f;

                if (player->state == STATE_ATTACKING_MEN) { heightOffset = 0; height = 20; }
                else if (player->state == STATE_ATTACKING_KOTE) { heightOffset = 30; height = 15; }
                else if (player->state == STATE_ATTACKING_DO) { heightOffset = 50; height = 20; }

                player->weaponHitbox.x = player->attackOrigin.x + (player->direction == 1 ? PLAYER_WIDTH : -reach);
                player->weaponHitbox.y = player->attackOrigin.y + heightOffset;
                player->weaponHitbox.width = reach;
                player->weaponHitbox.height = height;
            } else {
                player->weaponHitbox = (Rectangle){0, 0, 0, 0};
                player->state = STATE_IDLE; // Immediately return to non-attacking state
                player->stateTimer = 0.0f;
            }
            break;

        case STATE_COOLDOWN:
            player->stateTimer += delta;
            if (player->stateTimer > 0.2f) {
                player->state = STATE_IDLE;
            }
            break;

        case STATE_RECOILING:
            player->stateTimer += delta;
            // Push back slightly?
            if (player->stateTimer > 0.5f) {
                player->state = STATE_IDLE;
            }
            break;
    }
    

    // Update Body Hitbox (Position synced)
    player->bodyHitbox.x = player->position.x;
    player->bodyHitbox.y = player->position.y;

    // Safety: Ensure weapon hitbox is cleared if not attacking
    // This prevents "sticky" hitboxes if state changes abruptly (e.g. interruption/damage)
    if (player->state != STATE_ATTACKING_MEN && 
        player->state != STATE_ATTACKING_KOTE && 
        player->state != STATE_ATTACKING_DO) {
        player->weaponHitbox = (Rectangle){0, 0, 0, 0};
    }
}

void DrawPlayer(Player *player) {
    // Visual indicators for states
    Color drawColor = player->color;
    if (player->state == STATE_DASHING) drawColor = SKYBLUE;
    else if (player->state == STATE_BLOCKING) drawColor = ORANGE;
    else if (player->state == STATE_COOLDOWN) drawColor = GRAY;
    else if (player->state == STATE_RECOILING) drawColor = VIOLET;

    DrawRectangleRec(player->bodyHitbox, drawColor);
    
    // Indicate facing
    if (player->direction == 1) {
        DrawRectangle(player->position.x + PLAYER_WIDTH - 5, player->position.y + 5, 5, 5, BLACK); // Eye/Face indicator
    } else {
        DrawRectangle(player->position.x, player->position.y + 5, 5, 5, BLACK);
    }
    
    // Draw Weapon Hitbox if active
    if (player->weaponHitbox.width > 0) {
        DrawRectangleRec(player->weaponHitbox, RED); // "Spirit" sword visual
    }
}

void ResolveCollision(Player *p1, Player *p2) {
    // 1. Detection: Check if P1 Hit P2
    if (p1->weaponHitbox.width > 0) {
        if (CheckCollisionRecs(p1->weaponHitbox, p2->bodyHitbox)) {
            if (p2->state != STATE_BLOCKING && p2->state != STATE_RECOILING && p2->state != STATE_DASHING) {
                // Point Scored or Hit Registered
                // For "Real Kendo", getting hit is a big deal.
                // Reset positions or state? For now, add score and recoil.
                p1->score++;
                p2->state = STATE_RECOILING;
                p2->stateTimer = 0;
                p1->state = STATE_COOLDOWN; // Hit stop/impact
                p1->stateTimer = 0;
            } else if (p2->state == STATE_BLOCKING) {
                // Attack Blocked
                p1->state = STATE_RECOILING; // Attacker recoils on block
                p1->stateTimer = 0;
            }
        }
    }

    // 2. Detection: Check if P2 Hit P1
    if (p2->weaponHitbox.width > 0) {
        if (CheckCollisionRecs(p2->weaponHitbox, p1->bodyHitbox)) {
            if (p1->state != STATE_BLOCKING && p1->state != STATE_RECOILING && p1->state != STATE_DASHING) {
                p2->score++;
                p1->state = STATE_RECOILING;
                p1->stateTimer = 0;
                p2->state = STATE_COOLDOWN;
                p2->stateTimer = 0;
            } else if (p1->state == STATE_BLOCKING) {
                p2->state = STATE_RECOILING;
                p2->stateTimer = 0;
            }
        }
    }
    
    // Body Collision (Pushing)
    if (CheckCollisionRecs(p1->bodyHitbox, p2->bodyHitbox)) {
        // Simple separation
        float overlap = 0;
        if (p1->position.x < p2->position.x) {
            overlap = (p1->position.x + PLAYER_WIDTH) - p2->position.x;
            p1->position.x -= overlap / 2;
            p2->position.x += overlap / 2;
        } else {
            overlap = (p2->position.x + PLAYER_WIDTH) - p1->position.x;
            p2->position.x -= overlap / 2;
            p1->position.x += overlap / 2;
        }
    }
}
