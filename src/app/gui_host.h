#pragma once
#include <functional>
#include <imgui.h>

#include <GLFW/glfw3.h>
#include <functional>

class GuiHost {
public:
    GuiHost(int width, int height, const char* title);
    ~GuiHost();

    void Run(std::function<void()> render_callback);
    void RenderFrame() const;

private:
    GLFWwindow* window;
    ImVec4 clear_color;
    std::function<void()> render_callback;
};
