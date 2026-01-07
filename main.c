#include "raylib.h"
#include "player.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Real Kendo Ippon Match");
    SetTargetFPS(60);

    Player p1, p2;
    InitPlayer(&p1, 1);
    InitPlayer(&p2, 0);

    float gameTime = 60.0f; // 1 minute
    int gameOver = 0; // 0: Playing, 1: P1 Win, 2: P2 Win, 3: Draw

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Update
        if (!gameOver) {
            gameTime -= dt;
            if (gameTime <= 0) {
                gameTime = 0;
                gameOver = 3; // Draw
            }

            UpdatePlayer(&p1, dt);
            UpdatePlayer(&p2, dt);
            
            ResolveCollision(&p1, &p2);

            if (p1.score > 0) gameOver = 1;
            if (p2.score > 0) gameOver = 2;
        } else {
             // Restart Logic
             if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 Vector2 mousePos = GetMousePosition();
                 Rectangle restartBtn = { SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 200, 50 };
                 if (CheckCollisionPointRec(mousePos, restartBtn)) {
                     InitPlayer(&p1, 1);
                     InitPlayer(&p2, 0);
                     gameTime = 60.0f;
                     gameOver = 0;
                 }
             }
        }

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            // Draw Arena Floor
            DrawRectangle(0, 380, SCREEN_WIDTH, SCREEN_HEIGHT - 380, DARKGRAY);

            DrawPlayer(&p1);
            DrawPlayer(&p2);

            // UI
            DrawText("P1 Stamina:", 10, 10, 20, BLACK);
            DrawRectangle(130, 10, (int)p1.stamina * 2, 20, BLUE);
            
            DrawText("P2 Stamina:", SCREEN_WIDTH - 250, 10, 20, BLACK);
            DrawRectangle(SCREEN_WIDTH - 130, 10, (int)p2.stamina * 2, 20, RED);

            // Timer
            int minutes = (int)gameTime / 60;
            int seconds = (int)gameTime % 60;
            DrawText(TextFormat("%02d:%02d", minutes, seconds), SCREEN_WIDTH/2 - 40, 10, 40, BLACK);

            if (gameOver) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));
                
                if (gameOver == 1) {
                    DrawText("BLUE WIN", SCREEN_WIDTH/2 - MeasureText("BLUE WIN", 50)/2, SCREEN_HEIGHT/2 - 50, 50, BLUE);
                } else if (gameOver == 2) {
                    DrawText("RED WIN", SCREEN_WIDTH/2 - MeasureText("RED WIN", 50)/2, SCREEN_HEIGHT/2 - 50, 50, RED);
                } else {
                    DrawText("DRAW", SCREEN_WIDTH/2 - MeasureText("DRAW", 50)/2, SCREEN_HEIGHT/2 - 50, 50, LIGHTGRAY);
                }
                
                Rectangle restartBtn = { SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 200, 50 };
                DrawRectangleRec(restartBtn, LIGHTGRAY);
                
                // Button Hover Effect
                if (CheckCollisionPointRec(GetMousePosition(), restartBtn)) {
                    DrawRectangleLinesEx(restartBtn, 2, YELLOW);
                }

                const char* btnText = "RESTART GAME";
                int textWidth = MeasureText(btnText, 20);
                DrawText(btnText, restartBtn.x + (restartBtn.width - textWidth)/2, restartBtn.y + 15, 20, BLACK);
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
