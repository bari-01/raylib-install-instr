#include <raylib.h>
#include <vector>

int
main()
{
    int WIDTH = 1200;
    int HEIGHT = 800;
    InitWindow(WIDTH, HEIGHT, "Example Window");
    SetTargetFPS(165);
    std::vector<Color> pixels(WIDTH * HEIGHT, BLACK);

    Image image = GenImageColor(WIDTH, HEIGHT, BLACK);
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    while (!WindowShouldClose()) {
        UpdateTexture(texture, pixels.data());
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture, 0, 0, WHITE);
        DrawText("Example", 20, 15, 28, WHITE);
        EndDrawing();
    }
    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
