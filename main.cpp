// firmir-tasks: a tasks + notes app. The UI is declared in tasks.fml
// (compiled by firmlc); the task/notes logic and the persistence live in
// TaskApp (TaskApp.cpp); this file only creates the window and runs the
// event loop.
//
// Backend selection via FIRMIR_GUI_DISPLAY / FIRMIR_GUI_INPUT /
// FIRMIR_GUI_PLUGIN_PATH, like every firmir-gui app.

#include "tasks.hpp"
#include "TaskApp.hpp"

#include <firmir/gui/editabletext.hpp>
#include <firmir/gui/listmodel.hpp>
#include <firmir/gui/platform.hpp>
#include <firmir/gui/screen.hpp>
#include <firmir/gui/window.hpp>
#include <firmir/log.hpp>
#include <firmir/thread.hpp>

#include <cstdio>

int main() {
    auto* platform = Firmir::Gui::Platform::instance();
    if (platform->screens().empty()) {
        std::fprintf(stderr, "no screens — check FIRMIR_GUI_DISPLAY / "
                             "FIRMIR_GUI_PLUGIN_PATH\n");
        return 1;
    }

    // Creates the window, the whole UI tree and the TaskApp logic object,
    // then shows the window (see tasks.fml).
    TasksObjects objects = createTasks(platform->screens().front());
    objects.taskapp->attachViews(objects.taskModel, objects.notesEdit);
    objects.root->connect<bool>(
        objects.root, Firmir::Gui::Window::signalVisibilityChanged,
        [](bool visible) {
            if (!visible)
                Firmir::Thread::mainThread()->quit();
        });

    const int rc = Firmir::Thread::mainThread()->exec();
    Firmir::logInfo() << "firmir-tasks: bye";
    delete objects.root; // owns the whole tree (TaskApp saves on destruction)
    return rc;
}
