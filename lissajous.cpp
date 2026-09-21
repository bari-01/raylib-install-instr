#include <raylib.h>

#include <cmath>

struct Lissajous {
    long double ax = 1.0L;
    long double ay = 1.0L;

    long double fx = 3.0L;
    long double fy = 2.0L;

    long double phase = 0.0L;
};

int
main()
{
    constexpr int WIDTH = 1200;
    constexpr int HEIGHT = 800;

    InitWindow(WIDTH, HEIGHT, "Lissajous");
    SetTargetFPS(165);

    Lissajous lissajous;

    while (!WindowShouldClose()) {
        lissajous.phase += 0.001L;

        if (IsKeyPressed(KEY_UP)) lissajous.fx += 1.0L;

        if (IsKeyPressed(KEY_DOWN))
            lissajous.fx = std::max(1.0L, lissajous.fx - 1.0L);

        if (IsKeyPressed(KEY_RIGHT)) lissajous.fy += 1.0L;

        if (IsKeyPressed(KEY_LEFT))
            lissajous.fy = std::max(1.0L, lissajous.fy - 1.0L);

        if (IsKeyDown(KEY_A)) lissajous.phase += 0.005L;

        if (IsKeyDown(KEY_D)) lissajous.phase -= 0.005L;

        BeginDrawing();

        ClearBackground(BLACK);

        constexpr int samples = 10000;

        Vector2 previous{};

        for (int i = 0; i <= samples; ++i) {
            long double t = 2.0L * M_PI * i / samples;

            long double x =
                lissajous.ax * std::sin(lissajous.fx * t + lissajous.phase);

            long double y = lissajous.ay * std::sin(lissajous.fy * t);

            Vector2 point{WIDTH * 0.5f + (float)x * 300.0f,
                HEIGHT * 0.5f + (float)y * 300.0f};

            if (i > 0) DrawLineEx(previous, point, 2.0f, WHITE);

            previous = point;
        }

        DrawText("Lissajous figure", 20, 20, 28, WHITE);

        DrawText(TextFormat("X frequency: %d", (int)lissajous.fx), 20, 60, 18,
            WHITE);

        DrawText(TextFormat("Y frequency: %d", (int)lissajous.fy), 20, 85, 18,
            WHITE);

        DrawText("Arrow keys: frequencies", 20, 120, 18, WHITE);

        DrawText("A / D: phase", 20, 145, 18, WHITE);

        EndDrawing();
    }

    CloseWindow();
}
