#include "AlphabetWidget.h"

#include <qlogging.h>
#include <qtextedit.h>

AlphabetWidget::AlphabetWidget(QWidget* parent) {
    connect(this, &QLineEdit::textChanged, this, [this]() {
        qDebug() << "Text changed to: " << this->text() << '\n';
        QString new_text = this->text();
        if (new_text == alphabet_) {
            return;
        }
        alphabet_ = new_text;
        emit onAlphabetChanged(alphabet_);
    });
}
AlphabetWidget::~AlphabetWidget() {};

void AlphabetWidget::SetAlphabetType(AlphabetType new_alphabet_type) {
    switch (new_alphabet_type) {
        case AlphabetType::kInput:
            return setPlaceholderText(
                "Limit input messages(default same as runtime alphabet)");
        case AlphabetType::kRuntime:
            return setPlaceholderText(
                "Letters allowed to be used(must be set)");
    }
}

void AlphabetWidget::SetAlphabit(QString new_alphabet) {
    if (new_alphabet == alphabet_) {
        return;
    }
    emit textChanged(new_alphabet);
}
