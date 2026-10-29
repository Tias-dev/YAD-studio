#include "AlphabetWidget.h"

#include <qobject.h>
#include <qwidget.h>

#include <QChar>
#include <QSet>
#include <QTextEdit>
#include <QtLogging>

#include "AlphabetsManager.h"
#include "MarkovAlphabet.h"
#include "ui_AlphabetWidget.h"

AlphabetWidget::AlphabetWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::AlphabetWidget) {
    ui->setupUi(this);
    ui->error->setText("");
    ui->error->setStyleSheet("QLabel { color: red;  }");
    connect(ui->letters, &QLineEdit::textChanged, this, [this]() {
        if (ui->letters->text() == alphabet_.getAllChars()) {
            return;
        }
        QSet<QChar> letters;
        for (const QChar c : this->ui->letters->text()) {
            letters.insert(c);
        }
        alphabet_ = MarkovAlphabet{letters};
        if (type_ == AlphabetType::kRuntime) {
            AlphabetsManager::Instance().SetRuntimeAlphabet(alphabet_);
        }
        if (type_ == AlphabetType::kInput) {
            AlphabetsManager::Instance().SetInputAlphabet(alphabet_);
        }
    });
}
AlphabetWidget::~AlphabetWidget() { delete ui; };

void AlphabetWidget::SetAlphabetType(AlphabetType new_alphabet_type) {
    if (type_ != AlphabetType::kNone) {
        throw std::logic_error("Redefinition of alphabet type is not allowed!");
    }

    type_ = new_alphabet_type;
    switch (type_) {
        case AlphabetType::kInput:
            ui->name->setText("Input Alphabet");
            return ui->letters->setPlaceholderText(
                "Limit input messages(default same as runtime alphabet)");
        case AlphabetType::kRuntime:
            ui->name->setText("Runtime Alphabet");
            return ui->letters->setPlaceholderText(
                "Letters allowed to be used(must be set)");
        default:
            throw std::logic_error(
                "Setting kNone alphabet type in not allowed!");
    }
}

void AlphabetWidget::SetError(QString message) {
    ui->error->setText(message);
    ui->letters->setStyleSheet(
        "QLineEdit { border: 1px solid red; border-radius: 4px; }");
}

void AlphabetWidget::ClearError() {
    ui->error->setText("");
    ui->letters->setStyleSheet("");
}

void AlphabetWidget::SetAlphabet(MarkovAlphabet new_alphabet) {
    alphabet_ = new_alphabet;
    ui->letters->setText(alphabet_.getAllChars());
}
