#include "out_editor.hpp"
#include "va_analyser.hpp"
#include <TextEditor.h>

namespace out {

static TextEditor editor;

void init() {
    editor.SetText("");
    editor.SetShowWhitespaces(0);
    editor.SetColorizerEnable(0);
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

void render_and_tick(ImGuiID master) {
    ImGui::SetNextWindowDockID(master, ImGuiCond_FirstUseEver);
    ImGui::Begin("Output");

    editor.SetReadOnly(va::should_block_output_io());

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

}
