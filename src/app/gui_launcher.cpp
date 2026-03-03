#include <app/ui/main_window.h>
#include "gui_host.h"
#include <quill/quill_static.h>
#include <quill/logging.h>


int main() {

#ifdef _WIN32
#  ifndef NDEBUG
    SetConsoleOutputCP(CP_UTF8);
#  endif
#endif

    setupQuill("logs/matcher.log");
    auto eng = std::make_shared<sigma::Engine>();
    ui::MainWindow gui(eng);

    LOG_INFO("Quill initialized");
    GuiHost host(900, 600, "Sigma Engine GUI");
    host.Run([&]() {
        gui.Show();
        });

    eng->Stop();

    return 0;
}
