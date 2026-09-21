#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <thread>
#include <vector>

struct Source {
    long double x;
    long double y;
    long double amplitude;
    long double frequency;
    long double phase;
};

struct PaletteStop {
    long double position;
    Color color;
};

static constexpr PaletteStop palette[] = {
    {0.00L, {0, 2, 8, 255}},
    {0.12L, {3, 15, 30, 255}},
    {0.25L, {8, 45, 65, 255}},
    {0.38L, {20, 90, 95, 255}},
    {0.50L, {75, 135, 125, 255}},
    {0.62L, {170, 195, 170, 255}},
    {0.72L, {225, 225, 200, 255}},
    {0.80L, {80, 130, 125, 255}},
    {0.90L, {15, 55, 70, 255}},
    {1.00L, {0, 2, 8, 255}},
};

static Color
colorize(long double value)
{
    constexpr int paletteSize = sizeof(palette) / sizeof(palette[0]);

    long double t = value;
    t -= std::floor(t);

    for (int i = 0; i < paletteSize - 1; ++i) {
        if (t >= palette[i].position && t <= palette[i + 1].position) {
            long double u = (t - palette[i].position) /
                            (palette[i + 1].position - palette[i].position);

            auto lerp = [](unsigned char a, unsigned char b, long double t) {
                return (unsigned char)(a + (b - a) * t);
            };

            return {lerp(palette[i].color.r, palette[i + 1].color.r, u),
                lerp(palette[i].color.g, palette[i + 1].color.g, u),
                lerp(palette[i].color.b, palette[i + 1].color.b, u), 255};
        }
    }

    return palette[paletteSize - 1].color;
}

static long double
field(long double x, long double y)
{
    static constexpr Source sources[] = {
        {-0.95L, 0.00L, 1.00L, 22.0L, 0.0L},
        {-0.62L, -0.25L, 0.85L, 27.0L, 0.7L},
        {-0.35L, 0.30L, 0.90L, 24.0L, 1.3L},
        {0.05L, -0.10L, 1.00L, 20.0L, 2.0L},
        {0.55L, 0.25L, 1.10L, 15.0L, 0.2L},
        {0.90L, -0.35L, 0.75L, 18.0L, 1.8L},
    };

    long double result = 0.0L;

    for (const Source &s : sources) {
        long double dx = x - s.x;
        long double dy = y - s.y;

        long double r = std::sqrt(dx * dx + dy * dy);
        result += s.amplitude * std::sin(s.frequency * r + s.phase);
    }

    return result;
}

static void
render(std::vector<Color> &pixels, int width, int height)
{
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 4;
    threadCount = std::min(threadCount, (unsigned int)height);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    int rowsPerThread = height / threadCount;

    auto worker = [&](int yStart, int yEnd) {
        constexpr long double worldWidth = 4.0L;

        long double scale = width / worldWidth;

        long double centerX = 0.0L;
        long double centerY = 0.0L;

        for (int py = yStart; py < yEnd; ++py) {
            for (int px = 0; px < width; ++px) {

                long double x = centerX + (px - width * 0.5L) / scale;
                long double y = centerY + (py - height * 0.5L) / scale;

                long double f = field(x, y);
                constexpr long double cycles = 2.0L;
                long double t = f * cycles;

                t -= std::floor(t);
                pixels[py * width + px] = colorize(t);
            }
        }
    };

    for (unsigned int i = 0; i < threadCount; ++i) {
        int yStart = i * rowsPerThread;
        int yEnd = (i == threadCount - 1) ? height : yStart + rowsPerThread;
        threads.emplace_back(worker, yStart, yEnd);
    }

    for (std::thread &thread : threads)
        thread.join();
}

int
main()
{
    constexpr int WIDTH = 1200;
    constexpr int HEIGHT = 800;

    InitWindow(WIDTH, HEIGHT, "Wave Field");
    SetTargetFPS(165);

    std::vector<Color> pixels(WIDTH * HEIGHT, BLACK);

    Image image = GenImageColor(WIDTH, HEIGHT, BLACK);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    render(pixels, WIDTH, HEIGHT);
    UpdateTexture(texture, pixels.data());

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        DrawText("Wave interference field", 20, 15, 28, WHITE);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
