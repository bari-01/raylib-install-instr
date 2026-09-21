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

static void
treeRecursive(Point start, long double length, long double angle, int depth,
    long double angleDelta, long double scale, std::vector<Segment> &segments)
{
    if (depth == 0) return;

    Point end{
        start.x + length * std::cos(angle), start.y + length * std::sin(angle)};

    segments.push_back({start, end});

    treeRecursive(end, length * scale, angle - angleDelta, depth - 1,
        angleDelta, scale, segments);

    treeRecursive(end, length * scale, angle + angleDelta, depth - 1,
        angleDelta, scale, segments);
}

static std::vector<Segment>
fractalTree(int depth)
{
    std::vector<Segment> segments;

    treeRecursive(
        {0.0L, 1.5L}, 1.0L, -M_PI / 2.0L, depth, 0.5L, 0.7L, segments);

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

    std::vector<Segment> segments = fractalTree(depth);

    bool dragging = false;
    Vector2 lastMouse{};

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
            ++depth;
            segments = fractalTree(depth);
        }

        if (IsKeyPressed(KEY_LEFT_BRACKET) && depth > 0) {
            --depth;
            segments = fractalTree(depth);
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

            segments = fractalTree(depth);
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
