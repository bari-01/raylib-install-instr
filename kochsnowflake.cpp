#include <raylib.h>

#include <cmath>
#include <vector>

struct Point {
    long double x;
    long double y;
};

struct Segment {
    Point a;
    Point b;
};

static Point
lerp(Point a, Point b, long double t)
{ return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t}; }

static void
kochRecursive(Point a, Point b, int depth, std::vector<Segment> &segments)
{
    if (depth == 0) {
        segments.push_back({a, b});
        return;
    }

    Point p = lerp(a, b, 1.0L / 3.0L);
    Point q = lerp(a, b, 2.0L / 3.0L);

    long double dx = q.x - p.x;
    long double dy = q.y - p.y;

    constexpr long double SQRT3 = 1.7320508075688772935L;

    Point r{p.x + 0.5L * dx + (SQRT3 / 2.0L) * dy,
        p.y - (SQRT3 / 2.0L) * dx + 0.5L * dy};

    kochRecursive(a, p, depth - 1, segments);
    kochRecursive(p, r, depth - 1, segments);
    kochRecursive(r, q, depth - 1, segments);
    kochRecursive(q, b, depth - 1, segments);
}
static std::vector<Segment>
kochSnowflake(int depth)
{
    std::vector<Segment> segments;

    constexpr long double SQRT3 = 1.7320508075688772935L;

    Point a{-1.0L, -0.577350269L};
    Point b{1.0L, -0.577350269L};
    Point c{0.0L, 1.154700538L};

    kochRecursive(a, b, depth, segments);
    kochRecursive(b, c, depth, segments);
    kochRecursive(c, a, depth, segments);

    return segments;
}

static Point
worldToScreen(Point p, int width, int height, long double centerX,
    long double centerY, long double scale)
{
    return {(p.x - centerX) * scale + width * 0.5L,
        (p.y - centerY) * scale + height * 0.5L};
}

int
main()
{
    constexpr int WIDTH = 1200;
    constexpr int HEIGHT = 800;

    InitWindow(WIDTH, HEIGHT, "Koch");
    SetTargetFPS(165);

    int depth = 0;

    long double centerX = 0.0L;
    long double centerY = 0.0L;
    long double scale = 250.0L;

    std::vector<Segment> segments = kochSnowflake(depth);

    bool dragging = false;
    Vector2 lastMouse{};

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
            ++depth;
            segments = kochSnowflake(depth);
        }

        if (IsKeyPressed(KEY_LEFT_BRACKET) && depth > 0) {
            --depth;
            segments = kochSnowflake(depth);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            Vector2 mouse = GetMousePosition();

            long double beforeX = centerX + (mouse.x - WIDTH * 0.5L) / scale;
            long double beforeY = centerY + (mouse.y - HEIGHT * 0.5L) / scale;

            scale *= std::pow(1.25L, (long double)wheel);

            long double afterX = centerX + (mouse.x - WIDTH * 0.5L) / scale;
            long double afterY = centerY + (mouse.y - HEIGHT * 0.5L) / scale;

            centerX += beforeX - afterX;
            centerY += beforeY - afterY;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragging = true;
            lastMouse = GetMousePosition();
        }

        if (dragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();
            centerX -= (mouse.x - lastMouse.x) / scale;
            centerY -= (mouse.y - lastMouse.y) / scale;
            lastMouse = mouse;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) dragging = false;

        if (IsKeyPressed(KEY_R)) {
            depth = 0;
            centerX = 0.0L;
            centerY = 0.0L;
            scale = 250.0L;

            segments = kochSnowflake(depth);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (const Segment &segment : segments) {
            Point a = worldToScreen(
                segment.a, WIDTH, HEIGHT, centerX, centerY, scale);
            Point b = worldToScreen(
                segment.b, WIDTH, HEIGHT, centerX, centerY, scale);

            DrawLineEx({(float)a.x, (float)a.y}, {(float)b.x, (float)b.y}, 2.0f,
                WHITE);
        }

        DrawText("Koch curve", 20, 15, 28, WHITE);
        DrawText("[ / ] : recursion depth", 20, 55, 18, WHITE);
        DrawText("Wheel : zoom", 20, 78, 18, WHITE);
        DrawText("Left drag : pan", 20, 101, 18, WHITE);
        DrawText("R : reset", 20, 124, 18, WHITE);

        DrawText(TextFormat("Depth: %d", depth), 20, HEIGHT - 30, 18, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
