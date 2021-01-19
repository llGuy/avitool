#include <imgui.h>
#include "input_mode.hpp"
#include <SDL.h>
#include <malloc.h>
#include "ui.hpp"
#include <imfilebrowser.h>

#include "controller.hpp"
#include "video_viewer.hpp"

#include <TextEditor.h>

static ImGui::FileBrowser file_dialog;

static TextEditor editor;

constexpr uint32_t MAX_CHARS_IN_CONTROLLER_OUTPUT = 10000;

static struct {
    char cmdbuf[1000] = {};

    uint32_t char_pointer = 0;
    char *cmd_window = NULL;
    const char *prompt = "> ";
} controller_panel;

void prepare_imgui() {
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

    controller_panel.cmd_window = new char[MAX_CHARS_IN_CONTROLLER_OUTPUT];
    memset(controller_panel.cmd_window, 0, sizeof(char) * MAX_CHARS_IN_CONTROLLER_OUTPUT);

    memcpy(controller_panel.cmd_window, controller_panel.prompt, strlen(controller_panel.prompt));
    controller_panel.char_pointer += strlen(controller_panel.prompt);


    editor.SetText("");
    editor.SetShowWhitespaces(0);
    editor.SetColorizerEnable(0);
}

void print_to_controller_output(const char *str) {
    uint32_t len = strlen(str);

    if (len + controller_panel.char_pointer < MAX_CHARS_IN_CONTROLLER_OUTPUT) {
        memcpy(controller_panel.cmd_window + controller_panel.char_pointer, str, strlen(str));
        controller_panel.char_pointer += len;
    }
    else {
        controller_panel.char_pointer = 0;
        memset(controller_panel.cmd_window, 0, sizeof(char) * MAX_CHARS_IN_CONTROLLER_OUTPUT);
    }
}

void begin_controller_cmd(const char *cmd, bool print) {
    if (print) {
        print_to_controller_output(cmd);
        print_to_controller_output("\n");
    }

    submit_cmdstr(cmd);
}

void finish_controller_cmd(bool print) {
    if (print)
        print_to_controller_output("\n> ");
}

static ImGuiID s_tick_panel_master() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    uint32_t flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_MenuBar;

    ImGuiID dock_space_id = 0;

    ImGui::Begin("Master", NULL, flags);

    bool open_axes_popup = 0;
    bool open_info_selection_popup = 0;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                file_dialog.Open();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Begin record", "Ctrl+r")) {
                begin_controller_cmd("begin_record()");
                finish_controller_cmd();
            }
            if (ImGui::MenuItem("End record", "Ctrl+e")) {
                begin_controller_cmd("end_record()");
                finish_controller_cmd();
            }
            if (ImGui::MenuItem("Make axes", "Ctrl+m")) {
                open_axes_popup = 1;
            }
            if (ImGui::MenuItem("Choose record information", "Ctrl+i")) {
                open_info_selection_popup = 1;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    dock_space_id = ImGui::GetID("HUD_DockSpace");
    ImGui::DockSpace(dock_space_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    if (open_axes_popup)
        ImGui::OpenPopup("Axes");

    if (open_info_selection_popup)
        ImGui::OpenPopup("Choose record information");

    if (ImGui::BeginPopupModal("Axes")) {
        static char dist_str[10] = {};

        if (ImGui::InputText("Distance", dist_str, 10, ImGuiInputTextFlags_EnterReturnsTrue)) {
            char cmd_str[40] = {};
            sprintf(cmd_str, "make_axes(%s)", dist_str);
            begin_controller_cmd(cmd_str);
            finish_controller_cmd();
            
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("OK")) {
            char cmd_str[40] = {};
            sprintf(cmd_str, "make_axes(%s)", dist_str);
            begin_controller_cmd(cmd_str);
            finish_controller_cmd();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Choose record information")) {
        ImGui::Checkbox("Frame ID", &g_record_settings.frame_id);
        ImGui::Checkbox("Time", &g_record_settings.btime);
        ImGui::Checkbox("X Coordinate", &g_record_settings.xcoord);
        ImGui::Checkbox("Y Coordinate", &g_record_settings.ycoord);

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    return dock_space_id;
}

static void s_tick_panel_controller(ImGuiID master) {
    ImGui::SetNextWindowDockID(master, ImGuiCond_FirstUseEver);
    ImGui::Begin("Controller");

    if (ImGui::InputText("Command", controller_panel.cmdbuf, 1000, ImGuiInputTextFlags_EnterReturnsTrue)) {
        begin_controller_cmd(controller_panel.cmdbuf);

        memset(controller_panel.cmdbuf, 0, sizeof(controller_panel.cmdbuf));

        finish_controller_cmd();
    }

    ImGui::BeginChild("Controller_OutputWindow", {0, 0}, false, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    ImGui::TextUnformatted(controller_panel.cmd_window);
    ImGui::EndChild();

    ImGui::End();
}

static void s_tick_panel_file_browser() {
    file_dialog.Display();

    if (file_dialog.HasSelected()) {
        //printf("%s\n", file_dialog.GetSelected().string().c_str());
        static char cmdbuf[150] = {};
        sprintf(cmdbuf, "load_file(\"%s\")", file_dialog.GetSelected().string().c_str());
        for (uint32_t i = 0; i < strlen(cmdbuf); ++i) {
            if (cmdbuf[i] == '\\')
                cmdbuf[i] = '/';
        }

        begin_controller_cmd(cmdbuf);
        finish_controller_cmd();

        file_dialog.ClearSelected();
    }
}

void add_line_to_editor(const char *str) {
    // Push this to the text editor
    auto lines = editor.GetTextLines();
    auto cursor_pos = editor.GetCursorPosition();

    if (lines[cursor_pos.mLine].length() > 0) {
        lines[cursor_pos.mLine].append(",");
    }
    lines[cursor_pos.mLine].append(str);

    if (cursor_pos.mLine == lines.size() - 1) {
        lines.push_back({});
    }

    editor.SetCursorPosition(TextEditor::Coordinates(cursor_pos.mLine + 1, 0));
                    
    editor.SetTextLines(lines);
}

static void s_tick_panel_output(ImGuiID master) {
    ImGui::SetNextWindowDockID(master, ImGuiCond_FirstUseEver);
    ImGui::Begin("Output");

    editor.SetReadOnly(should_block_output_io());

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Copy", "Ctrl+C", (bool *)NULL, editor.HasSelection())) {
                editor.Copy();
            }
            if (ImGui::MenuItem("Cut", "Ctrl+X", (bool *)NULL, editor.HasSelection())) {
                editor.Cut();
            }
            if (ImGui::MenuItem("Delete", "Del", (bool *)NULL, editor.HasSelection())) {
                editor.Delete();
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V", (bool *)NULL, editor.HasSelection())) {
                editor.Paste();
            }
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Dark palette"))
                editor.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette"))
                editor.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette"))
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    editor.Render("TextEditor");
    
    ImGui::End();
}

void tick_gui() {
    ImGui::NewFrame();

    auto master_id = s_tick_panel_master();
    s_tick_panel_controller(master_id);
    s_tick_panel_file_browser();
    s_tick_panel_output(master_id);

    render_panel_video_viewer(master_id);

    call_input_proc();
    // execute_commands();

    ImGui::Render();
}
