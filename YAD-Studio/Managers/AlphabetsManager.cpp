#include "AlphabetsManager.h"

#include <qset.h>
#include <qtmetamacros.h>
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
    QSet<QChar> letters, multiplied_letters;
    for (QChar c : new_alphabet.getAllChars()) {
        if (letters.contains(c)) {
            multiplied_letters.insert(c);
        }
        letters.insert(c);
    }

    if (multiplied_letters.isEmpty()) {
        emit OnRemovedAlphabetError(AlphabetType::kRuntime,
                                    AlphabetErrorCode::kMultipleLetters);
    } else {
        QString message{"Multiplied letters: "};
        message += ConcatSet(multiplied_letters);
        emit OnNewAlphabetError(AlphabetType::kRuntime,
                                AlphabetErrorCode::kMultipleLetters, message);
    }
    runtime_alphabet_ = new_alphabet;
    emit OnNewRuntimeAlphabet(runtime_alphabet_);
    UpdateInputErrors();
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
    QSet<QChar> letters, multiplied_letters, invalid_letters;
    for (QChar c : new_alphabet.getAllChars()) {
        if (letters.contains(c)) {
            multiplied_letters.insert(c);
        }
        if (!runtime_alphabet_.getAlphabet().contains(c)) {
            invalid_letters.insert(c);
        }
        letters.insert(c);
    }

    if (multiplied_letters.isEmpty()) {
        emit OnRemovedAlphabetError(AlphabetType::kInput,
                                    AlphabetErrorCode::kMultipleLetters);
    } else {
        QString message{"Multiplied letters: "};
        message += ConcatSet(multiplied_letters);
        emit OnNewAlphabetError(AlphabetType::kInput,
                                AlphabetErrorCode::kMultipleLetters, message);
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
    emit OnNewInputAlphabet(input_alphabet_);
}
