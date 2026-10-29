#include "AlphabetsDisplayWidget.h"

#include "AlphabetsManager.h"
#include "ui_AlphabetsDisplayWidget.h"

AlphabetsDisplayWidget::AlphabetsDisplayWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::AlphabetsDisplayWidget) {
    ui->setupUi(this);

    ui->input_alphabet->SetAlphabetType(AlphabetWidget::AlphabetType::kInput);
    ui->runtime_alphabet->SetAlphabetType(
        AlphabetWidget::AlphabetType::kRuntime);

    connect(&AlphabetsManager::Instance(),
            &AlphabetsManager::OnNewRuntimeAlphabet, ui->runtime_alphabet,
            &AlphabetWidget::SetAlphabet);
    connect(&AlphabetsManager::Instance(),
            &AlphabetsManager::OnNewInputAlphabet, ui->input_alphabet,
            &AlphabetWidget::SetAlphabet);

    connect(&AlphabetsManager::Instance(),
            &AlphabetsManager::OnNewAlphabetError, this,
            [this](AlphabetsManager::AlphabetType type,
                   AlphabetsManager::AlphabetErrorCode code, QString message) {
                if (type == AlphabetsManager::AlphabetType::kRuntime) {
                    runtime_alphabet_errors_[code] = message;
                    ui->runtime_alphabet->SetError(message);
                }
                if (type == AlphabetsManager::AlphabetType::kInput) {
                    input_alphabet_errors_[code] = message;
                    ui->input_alphabet->SetError(message);
                }
            });

    connect(&AlphabetsManager::Instance(),
            &AlphabetsManager::OnRemovedAlphabetError, this,
            [this](AlphabetsManager::AlphabetType type,
                   AlphabetsManager::AlphabetErrorCode code) {
                if (type == AlphabetsManager::AlphabetType::kRuntime) {
                    runtime_alphabet_errors_.erase(code);
                    if (auto it = std::begin(runtime_alphabet_errors_);
                        it != std::end(runtime_alphabet_errors_)) {
                        ui->runtime_alphabet->SetError(it->second);
                    } else {
                        ui->runtime_alphabet->ClearError();
                    }
                }
                if (type == AlphabetsManager::AlphabetType::kInput) {
                    input_alphabet_errors_.erase(code);
                    if (auto it = std::begin(input_alphabet_errors_);
                        it != std::end(input_alphabet_errors_)) {
                        ui->input_alphabet->SetError(it->second);
                    } else {
                        ui->input_alphabet->ClearError();
                    }
                }
            });
}

AlphabetsDisplayWidget::~AlphabetsDisplayWidget() { delete ui; }
