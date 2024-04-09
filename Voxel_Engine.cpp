// Include glad, glm and glfw headers
#include <graphics_io.h>
#include <iostream>
#include <chrono>
int main()
{
    Graphics_Io Graphics_Interface;
    __int8* Voxels = new  __int8[32768];
    for (int i = 0; i < 32768; i++) {
        Voxels[i] = 0;
    }

    int Count = 0;
    int Voxel = 0;

    //timing 
    const long period_ms = 1000;
    std::chrono::milliseconds elapsed_milliseconds;
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();

    // Render loop
    while (!Graphics_Interface.Get_Window_Should_Close())
    {
        elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (elapsed_milliseconds.count() > period_ms) {
            start = std::chrono::steady_clock::now();
            Voxels[Voxel] = (__int8)1;
            Graphics_Interface.Send_Voxels(Voxels);
            Graphics_Interface.Update_Screen();
            Voxels[Voxel] = (__int8)0;
            Count = 0;
            Voxel++;
        }
        end = std::chrono::steady_clock::now();
    }
    return 0;
}
