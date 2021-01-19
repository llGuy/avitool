#include <imgui.h>
#include "controller.hpp"
#include <opencv2/highgui.hpp>
#include <imfilebrowser.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/videoio.hpp>

#include <stdlib.h>

#include "ui.hpp"
#include "context.hpp"
#include "input_mode.hpp"
#include "video_viewer.hpp"
#include "localisation.hpp"

int main() {
    init_languages();
    init_input_modes();
    prepare_imgui();
    init_context();
    init_video_viewer();
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
