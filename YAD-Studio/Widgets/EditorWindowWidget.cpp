#include "EditorWindowWidget.h"

#include "LineInfoWidget.h"
#include "MarkovEditorWidget.h"
#include "ui_EditorWindowWidget.h"

EditorWindowWidget::EditorWindowWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::EditorWindowWidget) {
    ui->setupUi(this);

    connect(ui->markov_program_editor, SIGNAL(canRun(bool)), this,
            SIGNAL(canRun(bool)));
    connect(ui->markov_program_editor, SIGNAL(sourceCodeChanged(QString)), this,
            SIGNAL(sourceCodeChanged(QString)));
    connect(ui->markov_program_editor,
            SIGNAL(markovAlgorithmChanged(MarkovAlgorithm)), this,
            SIGNAL(markovAlgorithmChanged(MarkovAlgorithm)));
    connect(ui->markov_program_editor, SIGNAL(redoAvailable(bool)), this,
            SIGNAL(redoAvailable(bool)));
    connect(ui->markov_program_editor, SIGNAL(undoAvailable(bool)), this,
            SIGNAL(undoAvailable(bool)));
    connect(ui->markov_program_editor, SIGNAL(copyAvailable(bool)), this,
            SIGNAL(copyAvailable(bool)));

    connect(ui->markov_program_editor, SIGNAL(breakPointAdded(int)), this,
            SIGNAL(breakPointAdded(int)));
    connect(ui->markov_program_editor, SIGNAL(breakPointRemoved(int)), this,
            SIGNAL(breakPointRemoved(int)));

    connect(ui->markov_program_editor,
            SIGNAL(lineFocusWithError(int, CompilerError)), ui->widget,
            SLOT(lineWithErrorFocus(int, CompilerError)));
    connect(ui->markov_program_editor, SIGNAL(lineFocusWithoutError(int)),
            ui->widget, SLOT(lineWithoutErrorFocus(int)));
}

EditorWindowWidget::~EditorWindowWidget() { delete ui; }

void EditorWindowWidget::newInputAlphabet(QString new_alphabet) {
    ui->markov_program_editor->newInputAlphabet(new_alphabet);
}

void EditorWindowWidget::newRuntimeAlphabet(QString new_alphabet) {
    ui->markov_program_editor->newRuntimeAlphabet(new_alphabet);
}

void EditorWindowWidget::showLineHighlight(int line_number) {
    ui->markov_program_editor->showLineHighlight(line_number);
}

void EditorWindowWidget::removeLineHighlight() {
    ui->markov_program_editor->removeLineHighlight();
}

void EditorWindowWidget::newSourceCode(QString new_source) {
    ui->markov_program_editor->newSourceCode(new_source);
}

void EditorWindowWidget::undo() { ui->markov_program_editor->undo(); }

void EditorWindowWidget::redo() { ui->markov_program_editor->redo(); }
void EditorWindowWidget::selectAll() { ui->markov_program_editor->selectAll(); }

void EditorWindowWidget::copy() { ui->markov_program_editor->copy(); }
void EditorWindowWidget::paste() { ui->markov_program_editor->paste(); }

void EditorWindowWidget::cut() { ui->markov_program_editor->cut(); }
void EditorWindowWidget::deleteSelection() {
    ui->markov_program_editor->deleteSelection();
}
void EditorWindowWidget::setDefaultSourceCode(QString source_code) {
    ui->markov_program_editor->newSourceCode(source_code, false);
}
