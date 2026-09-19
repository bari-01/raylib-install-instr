#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <thread>
#include <vector>

struct FractalCamera {
    long double centerX = -0.5;
    long double centerY = 0.0;
    long double scale = 4.0;
};

// static Color
// colorize(long double iteration, int maxIterations)
//{
//     if (iteration >= maxIterations) return BLACK;
//
//     long double t = iteration / maxIterations;
//
//     float hue = 185.0f + 100.0f * std::sin((float)t * 6.2831853f * 1.5f);
//     float saturation = 0.35f + 0.65f * std::sin((float)t * 3.14159265f);
//     float value = 0.45f + 0.55f * std::sin((float)t * 3.14159265f);
//
//     return ColorFromHSV(hue, saturation, value);
// }
struct PaletteStop {
    long double position;
    Color color;
};

static constexpr PaletteStop palette[] = {
    {0.00L, {0, 20, 80, 255}},
    {0.12L, {0, 90, 180, 255}},
    {0.25L, {0, 210, 240, 255}},
    {0.38L, {80, 240, 255, 255}},
    {0.50L, {230, 250, 255, 255}},
    {0.60L, {255, 255, 255, 255}},
    {0.70L, {240, 150, 255, 255}},
    {0.80L, {190, 50, 240, 255}},
    {0.90L, {50, 20, 180, 255}},
    {1.00L, {0, 20, 80, 255}},
};

static Color
colorize(long double iteration, int maxIterations)
{
    if (iteration >= maxIterations) return BLACK;

    constexpr long double frequency = 0.035L;
    long double t = iteration * frequency;

    t -= std::floor(t);

    constexpr int paletteSize = sizeof(palette) / sizeof(palette[0]);

    for (int i = 0; i < paletteSize - 1; ++i) {
        if (t >= palette[i].position && t <= palette[i + 1].position) {
            long double local = (t - palette[i].position) /
                                (palette[i + 1].position - palette[i].position);

            auto lerp = [](unsigned char a, unsigned char b, long double t) {
                return (unsigned char)(a + (b - a) * t);
            };

            return {lerp(palette[i].color.r, palette[i + 1].color.r, local),
                lerp(palette[i].color.g, palette[i + 1].color.g, local),
                lerp(palette[i].color.b, palette[i + 1].color.b, local),
                255};
        }
    }

    return palette[paletteSize - 1].color;
}

template <typename F>
static void
parallelForRows(int height, F &&worker)
{
    unsigned int threadCount = std::thread::hardware_concurrency() * 1.5;
    if (threadCount == 0) threadCount = 4;
    threadCount = std::min(threadCount, (unsigned int)height);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    int rowsPerThread = height / threadCount;

    for (unsigned int i = 0; i < threadCount; ++i) {
        int yStart = i * rowsPerThread;
        int yEnd = (i == threadCount - 1) ? height : yStart + rowsPerThread;
        threads.emplace_back(std::forward<F>(worker), yStart, yEnd);
    }

    for (std::thread &thread : threads)
        thread.join();
}
static void
parallelRender(std::vector<Color> &pixels, int width, int height,
    const std::function<Color(int, int)> &pixel)
{
    parallelForRows(height, [&](int yStart, int yEnd) {
        for (int py = yStart; py < yEnd; ++py) {
            for (int px = 0; px < width; ++px) {
                pixels[py * width + px] = pixel(px, py);
            }
        }
    });
}

static void
zoomAt(FractalCamera &camera, long double mouseX, long double mouseY, int width,
    int height, long double factor)
{
    // current coordinate
    long double beforeX =
        camera.centerX + (mouseX - width * 0.5) * camera.scale / width;

    long double beforeY =
        camera.centerY + (mouseY - height * 0.5) * camera.scale / width;

    camera.scale *= factor;

    // move camera to look at same place
    long double afterX =
        camera.centerX + (mouseX - width * 0.5) * camera.scale / width;

    long double afterY =
        camera.centerY + (mouseY - height * 0.5) * camera.scale / width;

    camera.centerX += beforeX - afterX;
    camera.centerY += beforeY - afterY;
}
