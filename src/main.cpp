/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** main
*/

#include <raylib.h>

int main()
{
    InitWindow(800, 600, "R-Type");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
