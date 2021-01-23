#include "log.hpp"
#include <glm/glm.hpp>
#include <imgui.h>
#include <GL/glew.h>
#include <SDL.h>
#include <stdlib.h>
#include "app_context.hpp"
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

namespace app {

static struct {

    SDL_Window *window;
    SDL_GLContext gl_ctx;
    const char *glsl_version;
    bool is_running;

} ctx;

static void s_error_and_exit(const char *msg) {
    LOG_ERROR(msg);
    exit(-1);
}

void init_context() {
    init_input_modes();

    { // Prepare UI stuff with ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ImGuiStyle &style =ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.TabRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.0f);
        ImVec4* colors = style.Colors;
    }

    { // Initialise GLFW and GLEW
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
            s_error_and_exit("Failed to initialise SDL\n");

        // May need to change this for MACOS
        ctx.glsl_version = "#version 130";

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        ctx.window = SDL_CreateWindow("AVITool", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
        ctx.gl_ctx = SDL_GL_CreateContext(ctx.window);
        SDL_GL_MakeCurrent(ctx.window, ctx.gl_ctx);
        SDL_GL_SetSwapInterval(1);

        if (glewInit() != GLEW_OK)
            s_error_and_exit("Failed to initialise GLEW\n");
    }

    { // Initialise ImGUI for OpenGL context
        ImGui_ImplSDL2_InitForOpenGL(ctx.window, ctx.gl_ctx);
        ImGui_ImplOpenGL3_Init(ctx.glsl_version);
    }
 
    ctx.is_running = 1;
}

ImGuiID begin_frame() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
            ctx.is_running = 0;
        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_CLOSE &&
            event.window.windowID == SDL_GetWindowID(ctx.window))
            ctx.is_running = 0;
    }
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(ctx.window);

    ImGui::NewFrame();

    return render_and_tick_master();
}

void end_frame() {
    ImGui::Render();

    ImGuiIO &io = ImGui::GetIO();
    glViewport(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    glm::vec4 clear_color = glm::vec4(0.4f, 0.5f, 0.6f, 1.0f);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(ctx.window);
}

bool is_running() {
    return ctx.is_running;
}

}
