#pragma once

// firmir-tasks application logic, exposed to the Firml UI (tasks.fml):
// pages switch through the "page" property, the task rows are a ListModel
// refilled by the app, and the buttons/line edits call the methods below.

#include <firmir/object.hpp>
#include <firmir/string.hpp>

#include <string>
#include <vector>

namespace Firmir {
namespace Gui {
class EditableText;
class ListModel;
} // namespace Gui
} // namespace Firmir

class TaskApp : public Firmir::Object {
public:
    explicit TaskApp(Object* parent = nullptr);
    ~TaskApp() override; // saves the store

    /// Wires the FML-created views (called once from main()).
    void attachViews(Firmir::Gui::ListModel* taskModel,
                     Firmir::Gui::EditableText* notesEdit);

    // ---- called from the Firml handlers ----
    void showPage(double page); // 0 = tasks, 1 = notes
    void addTask(Firmir::String text);
    void removeTask(double index);
    void setTaskDone(double index, bool done);
    void setNotes(Firmir::String text);

private:
    struct Task {
        bool done = false;
        std::string text;
    };

    void loadStore();
    void saveStore();
    void refillModel();

    std::vector<Task> m_tasks;
    std::string m_notes;
    Firmir::Gui::ListModel* m_model = nullptr;   // not owned
    Firmir::Gui::EditableText* m_notesEdit = nullptr; // not owned
};
