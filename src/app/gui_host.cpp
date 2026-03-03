#include "gui_host.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <app/fonts/IconFontAwesome5.h>


#include <stdio.h>

namespace {
    void FramebufferSizeCallback(GLFWwindow* w, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void WindowRefreshCallback(GLFWwindow* w) {
        auto* self = static_cast<GuiHost*>(glfwGetWindowUserPointer(w));
        self->RenderFrame();
        glfwSwapBuffers(w);
        glFinish();
    }

    void ErrorCallback(int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }
}

GuiHost::GuiHost(int width, int height, const char* title) {
    glfwSetErrorCallback(ErrorCallback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(int(width * main_scale), int(height * main_scale), title, nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetWindowRefreshCallback(window, WindowRefreshCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);

    io.Fonts->AddFontDefault();
    constexpr float baseFontSize = 13.0f;

    constexpr float iconFontSize = 18.0f;
    ImFontConfig iconConfig;
    iconConfig.MergeMode = true;
    iconConfig.GlyphMinAdvanceX = iconFontSize;
    iconConfig.GlyphOffset.y = 3.2f;

    static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFont* fontIcons = io.Fonts->AddFontFromFileTTF(
        "fonts/fa-solid-900.ttf",
        iconFontSize,
        &iconConfig,
        iconRanges
    );

    if (!fontIcons) {
        printf("Failed to load icon font!\n");
    }

    ImFontConfig cyrrilicConfig;
    cyrrilicConfig.MergeMode = true;
    static const ImWchar cyrrilicRanges[] = { 0x0400, 0x04FF, 0 }; // Cyrillic Unicode range
    ImFont* cyrrilicFont = io.Fonts->AddFontFromFileTTF(
        "fonts/EpilepsySans/EpilepsySans.ttf",
        baseFontSize,
        &cyrrilicConfig,
        cyrrilicRanges
    );

    if (!cyrrilicFont) {
        printf("Failed to load icon font!\n");
    }
}

GuiHost::~GuiHost() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GuiHost::Run(std::function<void()> callback) {
    render_callback = callback;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        RenderFrame();
        glfwSwapBuffers(window);
    }
}

void GuiHost::RenderFrame() const{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (render_callback)
        render_callback();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w,
        clear_color.y * clear_color.w,
        clear_color.z * clear_color.w,
        clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}