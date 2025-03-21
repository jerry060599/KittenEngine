# Kitten Engine
**[Jerry Hsu](https://jerryhsu.io), 2025**

A collection of OpenGL, CUDA, mathematics, and algorithm utilities built up over the years by yours truely.
Built for use as a project template in my physically based animations research. 
Released standalone here for open sourcing research projects which rely on this.

## Build
Configured for Windows and Visual Studios. 
The main library is compiled through **llvm** for performance and the app is compiled through **msvc** to simplify linking.

**Dependencies using vcpkg**: assimp, eigen3, stb headers, glad, glfw, imgui[opengl3-glad-binding], glm, jsoncpp

To install these packages:

1. Setup vcpkg https://vcpkg.io/en/getting-started.html

2. Run:
```
vcpkg.exe install glm:x64-windows
vcpkg.exe install eigen3:x64-windows
vcpkg.exe install assimp:x64-windows
vcpkg.exe install glad:x64-windows
vcpkg.exe install freetype:x64-windows
vcpkg.exe install jsoncpp:x64-windows
vcpkg.exe install imgui[core,glfw-binding,opengl3-binding]:x64-windows
```

**DO NOT INSTALL GLM DIRECTLY FROM THE CURRENT WORKING BRANCH.**
Only install versions taged as stable releases or through vcpkg. 
When in doubt, use glm version ```1.0.1#3```. 

## Usage
This project was built as a bare-bones template for use in my personal research. 
Fork it. Change it. Do whatever you want with it. 

This library handles:
* Recursive directory based asset loading and management
* Loading of textures, meshes, tetgen meshes, fonts, and shaders
* Shader #includes pre-processing and automatic shader linking 
* OpenGL/CUDA resource allocation and rendering
* Forward rendering pipeline with per light shadow maps
* Gizmos for rendering formated text, lines, arrows, translation widget ... etc 
* Integration with Dear ImGui and CUDA
* Many small utilities for debugging, timing, mathematics, statistics ... etc

See main.cpp for a bare-bones demo.

## License
Unless otherwise stated in the file header, the contents of this repository are provided under the following license. Files that specify a different copyright are governed by the terms indicated therein.

Copyright 2025 Jerry Hsu

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
