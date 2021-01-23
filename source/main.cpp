#include <imgui.h>
#include "ctrl_render.hpp"
#include "ctrl_cmd.hpp"

#include <stdlib.h>

#include "app_context.hpp"
#include "out_editor.hpp"
#include "va_analyser.hpp"
#include "localisation.hpp"

#if _WIN32
int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd) {
#else
int main() {
#endif
    init_languages();

    app::init_context();
    va::init();
    out::init();
    ctrl::init_controller();
    ctrl::init_controller_display();

    { // Main loop
        while (app::is_running()) {
            auto master_id = app::begin_frame();

            ctrl::render_and_tick(master_id);
            out::render_and_tick(master_id);
            va::render_and_tick(master_id);

            app::call_input_proc();

            app::end_frame();
        }
    }

    return 0;
}
