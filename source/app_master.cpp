#include <imgui.h>
#include "app_context.hpp"
#include <imfilebrowser.h>

#include "ctrl_render.hpp"
#include "va_analyser.hpp"

namespace app {

static ImGui::FileBrowser file_dialog;

static void s_tick_file_browser() {
    file_dialog.Display();

    if (file_dialog.HasSelected()) {
        //printf("%s\n", file_dialog.GetSelected().string().c_str());
        static char cmdbuf[150] = {};
        sprintf(cmdbuf, "load_file(\"%s\")", file_dialog.GetSelected().string().c_str());
        for (uint32_t i = 0; i < strlen(cmdbuf); ++i) {
            if (cmdbuf[i] == '\\')
                cmdbuf[i] = '/';
        }

        ctrl::begin_cmd(cmdbuf);
        ctrl::end_cmd();

        file_dialog.ClearSelected();
    }
}

union popups_t {
    struct {
        uint8_t open_axes_popup: 1;
        uint8_t open_info_selection_popup: 1;
    };
    uint8_t bytes;
};

static void s_tick_popups(const popups_t *popups) {
    if (popups->open_axes_popup)
        ImGui::OpenPopup("Axes");

    if (popups->open_info_selection_popup)
        ImGui::OpenPopup("Choose record information");

    if (ImGui::BeginPopupModal("Axes")) {
        static char dist_str[10] = {};

        if (ImGui::InputText("Distance", dist_str, 10, ImGuiInputTextFlags_EnterReturnsTrue)) {
            char cmd_str[40] = {};
            sprintf(cmd_str, "make_axes(%s)", dist_str);
            ctrl::begin_cmd(cmd_str);
            ctrl::end_cmd();
            
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("OK")) {
            char cmd_str[40] = {};
            sprintf(cmd_str, "make_axes(%s)", dist_str);
            ctrl::begin_cmd(cmd_str);
            ctrl::end_cmd();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Choose record information")) {
        va::handle_settings_checkboxes();

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    
}

ImGuiID render_and_tick_master() {
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

    popups_t popups;
    popups.bytes = 0;

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
                ctrl::begin_cmd("begin_record()");
                ctrl::end_cmd();
            }
            if (ImGui::MenuItem("End record", "Ctrl+e")) {
                ctrl::begin_cmd("end_record()");
                ctrl::end_cmd();
            }
            if (ImGui::MenuItem("Make axes", "Ctrl+m")) {
                popups.open_axes_popup = 1;
            }
            if (ImGui::MenuItem("Choose record information", "Ctrl+i")) {
                popups.open_info_selection_popup = 1;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    dock_space_id = ImGui::GetID("HUD_DockSpace");
    ImGui::DockSpace(dock_space_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    s_tick_popups(&popups);
    s_tick_file_browser();

    return dock_space_id;
}

}
