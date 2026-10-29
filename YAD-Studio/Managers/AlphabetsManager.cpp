#include "AlphabetsManager.h"

#include <qset.h>
#include <qtmetamacros.h>

#include "MarkovAlphabet.h"
AlphabetsManager* AlphabetsManager::instance_ = nullptr;
AlphabetsManager& AlphabetsManager::Instance() {
    if (!instance_) {
        instance_ = new AlphabetsManager;
    }
    return *instance_;
}

namespace {
QString ConcatSet(QSet<QChar> letters) {
    auto it = std::begin(letters), end = std::end(letters);
    QString message = *it;
    ++it;
    while (it != end) {
        message += QString{", "} + *it;
        ++it;
    }
    return message;
}
}  // namespace

void AlphabetsManager::SetRuntimeAlphabet(MarkovAlphabet new_alphabet) {
    runtime_alphabet_ = new_alphabet;
    emit OnNewRuntimeAlphabet(runtime_alphabet_);
    UpdateInputErrors();
}

MarkovAlphabet AlphabetsManager::GetRuntimeAlphabet() const {
    return runtime_alphabet_;
}

MarkovAlphabet AlphabetsManager::GetInputAlphabet() const {
    return input_alphabet_;
}

void AlphabetsManager::UpdateInputErrors() {
    QSet<QChar> invalid_letters;
    for (QChar c : input_alphabet_.getAllChars()) {
        if (!runtime_alphabet_.getAlphabet().contains(c)) {
            invalid_letters.insert(c);
        }
    }
    if (invalid_letters.isEmpty()) {
        emit OnRemovedAlphabetError(
            AlphabetType::kInput,
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet);
    } else {
        QString message{"Letters ouside runtime alphabet: "};
        message += ConcatSet(invalid_letters);
        emit OnNewAlphabetError(
            AlphabetType::kInput,
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet,
            message);
    }
}

void AlphabetsManager::SetInputAlphabet(MarkovAlphabet new_alphabet) {
    QSet<QChar> invalid_letters;
    for (QChar c : new_alphabet.getAllChars()) {
        if (!runtime_alphabet_.getAlphabet().contains(c)) {
            invalid_letters.insert(c);
        }
    }

    if (invalid_letters.isEmpty()) {
        emit OnRemovedAlphabetError(
            AlphabetType::kInput,
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet);
    } else {
        QString message{"Letters ouside runtime alphabet: "};
        message += ConcatSet(invalid_letters);
        emit OnNewAlphabetError(
            AlphabetType::kInput,
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet,
            message);
    }

    input_alphabet_ = new_alphabet;
    if (new_alphabet.getAlphabet().isEmpty()) {
        input_alphabet_ = MarkovAlphabet(/*is_auto=*/true);
    }
    emit OnNewInputAlphabet(input_alphabet_);
}
