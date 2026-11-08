#include "TaskApp.hpp"

#include <firmir/gui/editabletext.hpp>
#include <firmir/gui/listmodel.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

std::filesystem::path storePath() {
    const char* home = std::getenv("HOME");
    return std::filesystem::path(home ? home : ".") / ".local" / "share" /
           "firmir-tasks" / "store.txt";
}

} // namespace

TaskApp::TaskApp(Object* parent) : Object(parent) {
    setProperty("page", 0.0); // Firml binding: tasks page visible
    loadStore();
}

TaskApp::~TaskApp() { saveStore(); }

void TaskApp::attachViews(Firmir::Gui::ListModel* taskModel,
                          Firmir::Gui::EditableText* notesEdit) {
    m_model = taskModel;
    m_notesEdit = notesEdit;
    if (m_notesEdit)
        m_notesEdit->setText(Firmir::String(m_notes.c_str()));
    refillModel();
}

void TaskApp::showPage(double page) { setProperty("page", page); }

void TaskApp::addTask(Firmir::String text) {
    std::string trimmed = text.toStdString();
    const auto first = trimmed.find_first_not_of(' ');
    if (first == std::string::npos)
        return;
    m_tasks.push_back({false, trimmed.substr(first)});
    saveStore();
    // Incremental: one delegate is built for the new row, existing rows are
    // untouched.
    if (m_model)
        m_model->append(
            {{"done", false}, {"label", Firmir::String(m_tasks.back().text.c_str())}});
}

void TaskApp::removeTask(double index) {
    const int i = static_cast<int>(index);
    if (i < 0 || i >= static_cast<int>(m_tasks.size()))
        return;
    m_tasks.erase(m_tasks.begin() + i);
    saveStore();
    // Incremental: only this row's delegate is removed, the rest shift.
    if (m_model)
        m_model->removeAt(i);
}

void TaskApp::setTaskDone(double index, bool done) {
    const int i = static_cast<int>(index);
    if (i < 0 || i >= static_cast<int>(m_tasks.size()))
        return;
    m_tasks[i].done = done;
    saveStore();
    // Incremental: the label color and the checkbox of this row update in
    // place (the label color depends on the done role).
    if (m_model)
        m_model->setData(i, "done", done);
}

void TaskApp::setNotes(Firmir::String text) {
    m_notes = text.toStdString();
    saveStore();
}

void TaskApp::loadStore() {
    std::ifstream in(storePath());
    std::string line;
    while (std::getline(in, line)) {
        if (line.size() > 4 && line[0] == 'T' && line[1] == '\t') {
            const auto tab = line.find('\t', 2);
            if (tab != std::string::npos)
                m_tasks.push_back({line[2] == '1', line.substr(tab + 1)});
        } else if (line.size() > 2 && line[0] == 'N' && line[1] == '\t') {
            m_notes += line.substr(2) + '\n';
        }
    }
}

void TaskApp::saveStore() {
    const auto path = storePath();
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path);
    for (const Task& task : m_tasks)
        out << "T\t" << (task.done ? '1' : '0') << '\t' << task.text << '\n';
    std::string rest = m_notes;
    size_t pos = 0;
    while (pos < rest.size()) {
        const auto nl = rest.find('\n', pos);
        out << "N\t" << rest.substr(pos, nl - pos) << '\n';
        if (nl == std::string::npos)
            break;
        pos = nl + 1;
    }
}

void TaskApp::refillModel() {
    if (!m_model)
        return;
    m_model->clear();
    for (const Task& task : m_tasks)
        m_model->append({{"done", task.done}, {"label", Firmir::String(task.text.c_str())}});
}
