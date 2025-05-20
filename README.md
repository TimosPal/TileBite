# GameEngine

An ECS game engine written in C++
Currently supports only 2d rendering pipeline

# Depdendencies

GLFW, Glad, SpdLog, stb

## Build

```bash
git clone --recurse-submodules https://github.com/TimosPal/GameEngine.git
cd GameEngine
mkdir build && cd build
cmake ..
make