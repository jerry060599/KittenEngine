KittenEngine, Jerry Hsu 2023

vcpkg dependencies:
assimp, eigen3, stb headers, glad, glfw, imgui[opengl3-glad-binding], glm

To install these packages:

1. Setup vcpkg https://vcpkg.io/en/getting-started.html

2. Run:

vcpkg.exe install glm:x64-windows
vcpkg.exe install eigen3:x64-windows
vcpkg.exe install assimp:x64-windows
vcpkg.exe install glad:x64-windows
vcpkg.exe install imgui[core,glfw-binding,opengl3-glad-binding]:x64-windows