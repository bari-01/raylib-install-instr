#include "image.hpp"

static Color
shipPixel(int px, int py, int width, int height, const FractalCamera &camera,
    int maxIterations)
{
    double x0 =
        camera.centerX + ((double)px - width * 0.5) * camera.scale / width;
    double y0 =
        camera.centerY + ((double)py - height * 0.5) * camera.scale / width;

    double x = 0.0;
    double y = 0.0;
    int iteration = 0;
    for (; x*x + y*y <= 4.0 && iteration < maxIterations; ++iteration) {
        x = std::abs(x);
        y = std::abs(y);
        double xx = x*x - y*y + x0;
        double yy = 2.0 * x*y + y0;

        x = xx;
        y = yy;
    }

    if (iteration == maxIterations) return BLACK;

    double magnitude = std::sqrt(x * x + y * y);
    double smoothIteration = iteration + 1.0 - std::log2(std::log2(magnitude));
    return colorize(smoothIteration, maxIterations);
}
static void
rendership(std::vector<Color> &pixels, int width, int height,
    const FractalCamera &camera, int maxIterations)
{
    parallelRender(pixels, width, height, [&](int px, int py) {
        return shipPixel(px, py, width, height, camera, maxIterations);
    });
}

int
main()
{
    constexpr int WIDTH = 1200;
    constexpr int HEIGHT = 800;

    InitWindow(WIDTH, HEIGHT, "ship Set");
    SetTargetFPS(165);

    FractalCamera camera;
    FractalCamera initialCamera = camera;

    int maxIterations = 200;

    std::vector<Color> pixels(WIDTH * HEIGHT, BLACK);

    Image image = GenImageColor(WIDTH, HEIGHT, BLACK);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    bool dragging = false;
    Vector2 lastMouse = {};

    bool needsRender = true;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_R)) {
            camera = initialCamera;
            maxIterations = 200;
            needsRender = true;
        }

        if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
            Vector2 mouse = GetMousePosition();
            zoomAt(camera, mouse.x, mouse.y, WIDTH, HEIGHT, 0.5);
            needsRender = true;
        }

        if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
            Vector2 mouse = GetMousePosition();
            zoomAt(camera, mouse.x, mouse.y, WIDTH, HEIGHT, 2.0);
            needsRender = true;
        }

        if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
            maxIterations += 100;
            needsRender = true;
        }

        if (IsKeyPressed(KEY_LEFT_BRACKET)) {
            maxIterations = std::max(50, maxIterations - 100);
            needsRender = true;
        }

        float wheel = GetMouseWheelMove();

        if (wheel != 0.0f) {
            Vector2 mouse = GetMousePosition();
            double factor = std::pow(0.75, (double)wheel);
            zoomAt(camera, mouse.x, mouse.y, WIDTH, HEIGHT, factor);
            needsRender = true;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragging = true;
            lastMouse = GetMousePosition();
        }

        if (dragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetMousePosition();
            double dx = mouse.x - lastMouse.x;
            double dy = mouse.y - lastMouse.y;
            camera.centerX -= dx * camera.scale / WIDTH;
            camera.centerY -= dy * camera.scale / WIDTH;
            lastMouse = mouse;
            needsRender = true;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { dragging = false; }

        if (needsRender) {
            rendership(pixels, WIDTH, HEIGHT, camera, maxIterations);
            UpdateTexture(texture, pixels.data());
            needsRender = false;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        DrawText("Burning Ship Set", 20, 15, 28, WHITE);
        DrawText("Wheel / +/- : zoom", 20, 55, 18, WHITE);
        DrawText("Left drag : pan", 20, 78, 18, WHITE);
        DrawText("R : reset", 20, 101, 18, WHITE);
        DrawText("[ / ] : iterations", 20, 124, 18, WHITE);
        DrawText(TextFormat("Iterations: %d", maxIterations), 20, HEIGHT - 65,
            18, WHITE);
        DrawText(
            TextFormat("Center: %.12f, %.12f", camera.centerX, camera.centerY),
            20, HEIGHT - 42, 18, WHITE);
        DrawText(TextFormat("Scale: %.12g", camera.scale), 20, HEIGHT - 19, 18,
            WHITE);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
