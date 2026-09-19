1. Check if you have g++/clang. Linux: probably has them already, Windows users might need mingw-gcc or wsl
2. Download the raylib tar from https://github.com/raysan5/raylib/releases/latest. (linux-amd64 for x86_64, win32_mingw-w64 for mingw on windows, win32_msvc16.zip if using visual studio)
3. Make a new folder for the workshop, and extract the raylib archive there. Copy the images.hpp from here to the workshop folder.
4. Add raylib/lib to LD_LIBRARY_PATH `export LD_LIBRARY_PATH=raylib/lib` on linux/`export DYLD_LIBRARY_PATH=raylib/lib` onmacos, `set PATH=raylib/lib;%PATH` on cmd
5. The g++ command to compile program is `g++ -std=c++20 mandelbrot.cpp -I include/raylib/include -L include/raylib/lib -lraylib -lm -lpthread -ldl -lrt -lX11 -o mandelbrot`, where mandelbrot.cpp is the main program
