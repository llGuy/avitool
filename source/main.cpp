#include <imgui.h>
#include "controller.hpp"
#include <opencv2/highgui.hpp>
#include <imfilebrowser.h>
#include <GL/glew.h>
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>

#include <stdlib.h>

#include "ui.hpp"
#include "context.hpp"
#include "input_mode.hpp"
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
    init_input_modes();
    prepare_imgui();
    init_context();
    va::init();
    init_controller();

    { // Main loop
        while (is_running()) {
            begin_frame();
            tick_gui();
            end_frame();
        }
    }

    return 0;
}
