#include "MainWindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QTextCursor>
#include <QUrl>

#include "AboutWindow.h"
#include "AlphabetsDisplayWidget.h"
#include "EditorWindowWidget.h"
#include "Managers/FileManager.h"
#include "Managers/HistoryManager.h"
#include "Managers/MarkovRunManager.h"
#include "Managers/SourceCodeManager.h"
#include "MarkovEditorWidget.h"
#include "Widgets/DebugRunWidget.h"
#include "Widgets/RunWidget.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _has_unsaved_data(true),
      _current_file(""),
      _can_run(false),
      _is_running(false),
      _is_debug_input(false) {
    ui->setupUi(this);

    initStyles();
    updateWindowTitle();
    redoAvailable(false);
    undoAvailable(false);
    copyAvailable(false);

    _window = this;

    ui->debugRun->setVisible(false);
    ui->runWidget->setVisible(false);

    updateDebugMenu();

    // Connect MainWindow menu
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionUndo, SIGNAL(triggered()), this, SIGNAL(undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), this, SIGNAL(redo()));
    connect(ui->actionSelect_All, SIGNAL(triggered()), this,
            SIGNAL(selectAll()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SIGNAL(copy()));
    connect(ui->actionPaste, SIGNAL(triggered()), this, SIGNAL(paste()));
    connect(ui->actionCut, SIGNAL(triggered()), this, SIGNAL(cut()));
    connect(ui->actionDelete, SIGNAL(triggered()), this,
            SIGNAL(deleteSelection()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SIGNAL(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SIGNAL(open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SIGNAL(save()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SIGNAL(saveAs()));
    connect(ui->actionTutorial, SIGNAL(triggered()), this, SLOT(tutorial()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    // Connect InputWidget and HistoryManager
    HistoryManager* history_manager = HistoryManager::getInstance();

    connect(ui->input, SIGNAL(addToHistory(QString)), history_manager,
            SLOT(addToHistory(QString)));
    connect(history_manager, SIGNAL(wordSelected(QString)), ui->input,
            SLOT(setInput(QString)));

    // Connect HistoryWidget and HistoryManager
    connect(ui->history, SIGNAL(inputWordSelected(QString)), history_manager,
            SIGNAL(wordSelected(QString)));
    connect(ui->history, SIGNAL(removeFromHistory(QString)), history_manager,
            SLOT(removeFromHistory(QString)));
    connect(history_manager, SIGNAL(historyChanged(std::vector<QString>)),
            ui->history, SLOT(historyChanged(std::vector<QString>)));

    // Connect MainWindows and FileManager
    FileManager* file_manager = FileManager::getInstance();
    connect(this, SIGNAL(newFile()), file_manager, SLOT(newFile()));
    connect(this, SIGNAL(open()), file_manager, SLOT(open()));
    connect(this, SIGNAL(save()), file_manager, SLOT(save()));
    connect(this, SIGNAL(saveAs()), file_manager, SLOT(saveAs()));
    connect(file_manager, SIGNAL(hasUnsavedData(bool)), this,
            SLOT(hasUnsavedData(bool)));
    connect(file_manager, SIGNAL(fileNameChanged(QString)), this,
            SLOT(fileNameChanged(QString)));

    // Connect MainWindows and EditorWindowWidget
    connect(this, SIGNAL(undo()), ui->editorWindow, SLOT(undo()));
    connect(this, SIGNAL(redo()), ui->editorWindow, SLOT(redo()));
    connect(this, SIGNAL(selectAll()), ui->editorWindow, SLOT(selectAll()));
    connect(this, SIGNAL(copy()), ui->editorWindow, SLOT(copy()));
    connect(this, SIGNAL(paste()), ui->editorWindow, SLOT(paste()));
    connect(this, SIGNAL(cut()), ui->editorWindow, SLOT(cut()));
    connect(this, SIGNAL(deleteSelection()), ui->editorWindow,
            SLOT(deleteSelection()));

    connect(ui->editorWindow, SIGNAL(redoAvailable(bool)), this,
            SLOT(redoAvailable(bool)));
    connect(ui->editorWindow, SIGNAL(undoAvailable(bool)), this,
            SLOT(undoAvailable(bool)));
    connect(ui->editorWindow, SIGNAL(copyAvailable(bool)), this,
            SLOT(copyAvailable(bool)));

    // Connect InputWidget and MarkovRunManager
    MarkovRunManager* run_manager = MarkovRunManager::getInstance();

    connect(ui->input, SIGNAL(run(QString)), run_manager,
            SLOT(runWithoutDebug(QString)));
    connect(ui->input, SIGNAL(runWithDebug(QString)), run_manager,
            SLOT(runWithDebug(QString)));
    connect(run_manager, SIGNAL(runWithoutDebugStarted(QString)), ui->input,
            SLOT(runStarted()));
    connect(ui->input, SIGNAL(runWithDebugStepByStep(QString)), run_manager,
            SLOT(runWithDebugStepByStep(QString)));
    connect(run_manager, SIGNAL(debugStarted(QString)), ui->input,
            SLOT(runStarted()));
    connect(run_manager,
            SIGNAL(runWithoutDebugFinishFail(QString, RunError, int)),
            ui->input, SLOT(runFinished()));
    connect(run_manager,
            SIGNAL(runWithoutDebugFinishSuccess(QString, QString, int)),
            ui->input, SLOT(runFinished()));
    connect(run_manager, SIGNAL(debugFinishFail(QString, RunError, int)),
            ui->input, SLOT(runFinished()));
    connect(run_manager, SIGNAL(debugFinishSuccess(QString, QString, int)),
            ui->input, SLOT(runFinished()));
    connect(run_manager, SIGNAL(canRunSourceCode(bool)), ui->input,
            SLOT(canRunAlgorithm(bool)));

    // Connect SourceCodeManager and EditorWindowWidget
    SourceCodeManager* source_manager = SourceCodeManager::getInstance();
    connect(source_manager, SIGNAL(newSourceCodeWasLoaded(QString)),
            ui->editorWindow, SLOT(newSourceCode(QString)));
    connect(ui->editorWindow, SIGNAL(sourceCodeChanged(QString)),
            source_manager, SLOT(setSourceCode(QString)));

    // Connect InputWidget and FileManager
    connect(ui->input, SIGNAL(save()), file_manager, SLOT(save()));

    // Connect MarkovRunManager and EditorWindowWidget
    connect(ui->editorWindow, SIGNAL(canRun(bool)), run_manager,
            SLOT(setCanRunSourceCode(bool)));
    connect(ui->editorWindow, SIGNAL(markovAlgorithmChanged(MarkovAlgorithm)),
            run_manager, SLOT(setAlgorithm(MarkovAlgorithm)));

    // Connect SourceCodeManager and FileManager
    connect(file_manager, SIGNAL(newSourceCodeLoaded(QString)), source_manager,
            SLOT(setNewSourceCodeFromFile(QString)));
    connect(source_manager, SIGNAL(sourceCodeChanged(QString)), file_manager,
            SLOT(sourceCodeChanged()));

    // Connect FileManager and HistoryManager
    connect(file_manager, SIGNAL(newHistoryLoaded(std::vector<QString>)), this,
            SLOT(newHistoryLoaded(std::vector<QString>)));
    connect(history_manager, SIGNAL(historyChanged(std::vector<QString>)),
            file_manager, SLOT(historyChanged()));

    // Connect RunWidget and MarkovRunManager
    connect(run_manager, SIGNAL(runWithoutDebugStarted(QString)), ui->runWidget,
            SLOT(runStarted(QString)));
    connect(run_manager, SIGNAL(runStepsMade(int)), ui->runWidget,
            SLOT(runStepsMade(int)));
    connect(run_manager,
            SIGNAL(runWithoutDebugFinishFail(QString, RunError, int)),
            ui->runWidget, SLOT(runFailed(QString, RunError, int)));
    connect(run_manager,
            SIGNAL(runWithoutDebugFinishSuccess(QString, QString, int)),
            ui->runWidget, SLOT(runSuccess(QString, QString, int)));
    connect(run_manager, SIGNAL(debugStarted(QString)), ui->runWidget,
            SLOT(hide()));

    // Connect DebugRunWidget and MarkovRunManager
    connect(ui->debugRun, SIGNAL(nextStepClicked()), run_manager,
            SLOT(debugNextStep()));
    connect(ui->debugRun, SIGNAL(continueClicked()), run_manager,
            SLOT(debugContinue()));
    connect(ui->debugRun, SIGNAL(stopClicked()), run_manager,
            SLOT(debugStop()));

    connect(run_manager, SIGNAL(debugStarted(QString)), ui->debugRun,
            SLOT(debugStarted(QString)));
    connect(run_manager, SIGNAL(debugFinishSuccess(QString, QString, int)),
            ui->debugRun, SLOT(debugSuccess(QString, QString, int)));
    connect(run_manager, SIGNAL(debugFinishFail(QString, RunError, int)),
            ui->debugRun, SLOT(debugFailed(QString, RunError, int)));
    connect(run_manager,
            SIGNAL(debugStepFinished(int, QString, QString, MarkovRule)),
            ui->debugRun,
            SLOT(debugStepFinished(int, QString, QString, MarkovRule)));
    connect(run_manager, SIGNAL(debugBreakPointReached(int)), ui->debugRun,
            SLOT(breakPointReached(int)));
    connect(run_manager, SIGNAL(runWithoutDebugStarted(QString)), ui->debugRun,
            SLOT(hide()));
    connect(run_manager, SIGNAL(debugFinishFail(QString, RunError, int)),
            ui->runWidget, SLOT(hide()));
    connect(run_manager,
            SIGNAL(runWithoutDebugFinishFail(QString, RunError, int)),
            ui->debugRun, SLOT(hide()));

    // Connect DebugRunWidget and EditorWindowWidget
    connect(ui->debugRun, SIGNAL(removeBreakPoint()), ui->editorWindow,
            SLOT(removeLineHighlight()));
    connect(ui->debugRun, SIGNAL(showBreakPoint(int)), ui->editorWindow,
            SLOT(showLineHighlight(int)));

    // Connect MarkovRunManager and EditorWindowWidget
    connect(ui->editorWindow, SIGNAL(breakPointAdded(int)), run_manager,
            SLOT(addBreakPoint(int)));
    connect(ui->editorWindow, SIGNAL(breakPointRemoved(int)), run_manager,
            SLOT(removeBreakPoint(int)));

    // Connect top menu
    connect(run_manager, SIGNAL(runWithoutDebugStarted(QString)), this,
            SLOT(runStarted()));
    connect(run_manager, SIGNAL(debugStarted(QString)), this,
            SLOT(runStarted()));
    connect(run_manager,
            SIGNAL(runWithoutDebugFinishFail(QString, RunError, int)), this,
            SLOT(runFinished()));
    connect(run_manager,
            SIGNAL(runWithoutDebugFinishSuccess(QString, QString, int)), this,
            SLOT(runFinished()));
    connect(run_manager, SIGNAL(debugFinishFail(QString, RunError, int)), this,
            SLOT(runFinished()));
    connect(run_manager, SIGNAL(debugFinishSuccess(QString, QString, int)),
            this, SLOT(runFinished()));
    connect(run_manager, SIGNAL(canRunSourceCode(bool)), this,
            SLOT(canRunAlgorithm(bool)));
    connect(run_manager, SIGNAL(debugBreakPointReached(int)), this,
            SLOT(debugInputStarted()));
    connect(run_manager,
            SIGNAL(debugStepFinished(int, QString, QString, MarkovRule)), this,
            SLOT(debugInputFinished()));

    connect(ui->actionRun, SIGNAL(triggered()), ui->input, SLOT(runCliked()));
    connect(ui->actionDebug, SIGNAL(triggered()), ui->input,
            SLOT(runWithDebugClicked()));
    connect(ui->actionNext_Step, SIGNAL(triggered()), run_manager,
            SLOT(debugNextStep()));
    connect(ui->actionContinue, SIGNAL(triggered()), run_manager,
            SLOT(debugContinue()));
    connect(ui->actionStop_Debug, SIGNAL(triggered()), run_manager,
            SLOT(debugStop()));
    connect(ui->actionDebug_Step_By_Step, SIGNAL(triggered()), ui->input,
            SLOT(runWithDebugStepByStepClicked()));

    // Read file to open from command line
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() >= 2) {
        QString file_name = arguments.at(1);
        FileManager::getInstance()->openFile(file_name);
    } else {
        source_manager->setNewSourceCodeFromFile(
            tr("//Alphabet\nT = {a, b}\n\n//Rules\nab -> a\na ->. b"));
    }
}
void MainWindow::initStyles() {}
void MainWindow::about() {
    AboutWindow* about_window = new AboutWindow(this);
    about_window->show();
}

void MainWindow::tutorial() {
    QTemporaryFile file{QDir::tempPath() + "/XXXXXX-tutorial.html"};
    if (!file.open()) {
        QMessageBox box;
        box.setWindowTitle("Error");
        box.setText(QString{"Can't open file: "} + file.fileName() +
                    QString{" to write tutorial in it"});
        box.exec();
        return;
    }

    file.write(
#include "docs/tutorial.html"
    );
    file.setAutoRemove(false);
    file.close();
    QUrl url = QUrl::fromLocalFile(file.fileName());
    qDebug() << url;
    QDesktopServices::openUrl(url);
}

void MainWindow::canRunAlgorithm(bool v) {
    if (_can_run != v) {
        _can_run = v;
        updateDebugMenu();
    }
}

void MainWindow::runStarted() {
    if (!_is_running) {
        _is_running = true;
        updateDebugMenu();
    }
}

void MainWindow::runFinished() {
    if (_is_running) {
        _is_running = false;
        updateDebugMenu();
    }
    debugInputFinished();
}

void MainWindow::debugInputStarted() {
    if (!_is_debug_input) {
        _is_debug_input = true;
        updateDebugMenu();
    }
}

void MainWindow::debugInputFinished() {
    if (_is_debug_input) {
        _is_debug_input = false;
        updateDebugMenu();
    }
}

void MainWindow::updateDebugMenu() {
    if (_is_running || !_can_run) {
        ui->actionRun->setEnabled(false);
        ui->actionDebug->setEnabled(false);
        ui->actionDebug_Step_By_Step->setEnabled(false);
    } else {
        ui->actionRun->setEnabled(true);
        ui->actionDebug->setEnabled(true);
        ui->actionDebug_Step_By_Step->setEnabled(true);
    }

    if (_is_debug_input) {
        ui->actionNext_Step->setEnabled(true);
        ui->actionContinue->setEnabled(true);
        ui->actionStop_Debug->setEnabled(true);
    } else {
        ui->actionNext_Step->setEnabled(false);
        ui->actionContinue->setEnabled(false);
        ui->actionStop_Debug->setEnabled(false);
    }
}

void MainWindow::newHistoryLoaded(std::vector<QString> history) {
    HistoryManager::getInstance()->clearHistory();
    HistoryManager::getInstance()->addToHistory(history);
}

void MainWindow::updateWindowTitle() {
    QString file = _current_file;
    if (file == "") file = tr("Untitiled");

    QFileInfo fileInfo(file);
    QString filename(fileInfo.fileName());

    QString saved = "";
    if (_has_unsaved_data) saved = "*";

    QString title = tr("%1%2 - Yad Studio").arg(filename, saved);
    this->setWindowTitle(title);
}

void MainWindow::fileNameChanged(QString new_name) {
    _current_file = new_name;
    updateWindowTitle();
}

void MainWindow::hasUnsavedData(bool has_unsaved_data) {
    _has_unsaved_data = has_unsaved_data;
    updateWindowTitle();
}

void MainWindow::redoAvailable(bool v) { ui->actionRedo->setEnabled(v); }

void MainWindow::undoAvailable(bool v) { ui->actionUndo->setEnabled(v); }

void MainWindow::copyAvailable(bool v) {
    ui->actionCopy->setEnabled(v);
    ui->actionCut->setEnabled(v);
    ui->actionDelete->setEnabled(v);
}

MainWindow::~MainWindow() {
    _window = nullptr;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (FileManager::getInstance()->hasUnsavedData()) {
        QMessageBox msgBox;
        msgBox.setText(tr("<b>The document has been modified.</b>"));
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                                  QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Save) {
            if (FileManager::getInstance()->save())
                QMainWindow::closeEvent(event);
            else
                event->ignore();
        } else if (ret == QMessageBox::Discard) {
            QMainWindow::closeEvent(event);
        } else {
            event->ignore();
        }

    } else {
        QMainWindow::closeEvent(event);
    }
}

MainWindow* MainWindow::_window = nullptr;
MainWindow* MainWindow::getInstance() { return _window; }
