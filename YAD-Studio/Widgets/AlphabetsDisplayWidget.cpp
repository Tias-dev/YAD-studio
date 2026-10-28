#include "AlphabetsDisplayWidget.h"

#include "ui_AlphabetsDisplayWidget.h"

AlphabetsDisplayWidget::AlphabetsDisplayWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::AlphabetsDisplayWidget) {
    ui->setupUi(this);

    ui->input_alphabet->SetAlphabetType(AlphabetWidget::AlphabetType::kInput);
    ui->runtime_alphabet->SetAlphabetType(
        AlphabetWidget::AlphabetType::kRuntime);

    connect(ui->runtime_alphabet, &AlphabetWidget::onAlphabetChanged, this,
            [this](QString new_alphabet) {
                emit onRuntimeAlphabetChanged(new_alphabet);
            });
    connect(ui->input_alphabet, &AlphabetWidget::onAlphabetChanged, this,
            [this](QString new_alphabet) {
                emit onInputAlphabetChanged(new_alphabet);
            });
}

AlphabetsDisplayWidget::~AlphabetsDisplayWidget() { delete ui; }
