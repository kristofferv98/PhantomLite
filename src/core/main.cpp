/// main.cpp — engine bootstrap & event loop
#include <raylib.h>
#include <fmt/core.h>

int main() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1280, 720, "PhantomLite — black canvas");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
} 