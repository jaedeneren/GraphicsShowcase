# Real-Time Shader Showcase

An interactive C++ and OpenGL application that brings together a series of real-time graphics studies in one multi-window showcase. The project explores procedural rendering, lighting, environment mapping, post-processing, portals, and deferred rendering through GLSL shaders.

Developed by **Jaeden Wong** for CSD2151 at DigiPen Institute of Technology Singapore, using a course framework by **Vadim Surov**.

## Highlights

- Interactive procedural 2D shader studies
- Analytic ray casting and a procedural solar system
- Blinn-Phong lighting and material controls
- Environment mapping with reflection and refraction
- HDR and night-sky cubemaps
- Multi-pass cartoon rendering with Gaussian blur and Sobel edge detection
- An interactive projected-texture portal
- Deferred shading with a G-buffer and multiple lights
- Resizable child windows, fullscreen mode, and screenshot capture

## Tech stack

- C++
- OpenGL 4.6
- GLSL
- GLFW
- GLEW
- GLM
- Visual Studio

## Building on Windows

1. Install Visual Studio with the **Desktop development with C++** workload.
2. Install GLFW, GLEW, and GLM.
3. Update the dependency paths in `Shaders/Shaders.vcxproj` if your libraries are not installed under `C:\glfw`, `C:\glew`, and `C:\glm`.
4. Open `Shaders.sln` in Visual Studio.
5. Select the `x64` configuration and build the solution.

The current project targets OpenGL 4.6 and therefore requires a compatible graphics driver.

## Controls

Controls vary between demonstrations and are printed to the console when a window receives focus. Common controls include:

- `W`, `A`, `S`, `D`: move the camera in supported scenes
- Left mouse drag: rotate or interact with the scene
- `Tab`: switch modes in supported demonstrations
- `Space`: toggle fullscreen
- `Enter`: open a child demonstration in its own window
- `P`: capture a screenshot
- `Esc`: exit

## Attribution

The application was produced as coursework. The framework source identifies Vadim Surov as its author and Jaeden Wong as co-author. Third-party image and model assets remain the property of their respective owners.

