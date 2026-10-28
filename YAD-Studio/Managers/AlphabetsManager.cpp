#include "AlphabetsManager.h"

#include <qset.h>
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

    if (multiplied_letters.isEmpty() &&
        runtime_alphabet_errors_.contains(
            AlphabetErrorCode::kMultipleLetters)) {
        runtime_alphabet_errors_.erase(AlphabetErrorCode::kMultipleLetters);
        emit OnRemovedAlphabetError(AlphabetType::kRuntime,
                                    AlphabetErrorCode::kMultipleLetters);
    } else if (!multiplied_letters.isEmpty() &&
               !runtime_alphabet_errors_.contains(
                   AlphabetErrorCode::kMultipleLetters)) {
        QString message{"Multiplied letters: "};
        message += ConcatSet(multiplied_letters);
        emit OnNewAlphabetError(AlphabetType::kRuntime,
                                AlphabetErrorCode::kMultipleLetters, message);
        runtime_alphabet_errors_.insert(AlphabetErrorCode::kMultipleLetters);
    }
    runtime_alphabet_ = new_alphabet;
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

    if (multiplied_letters.isEmpty() &&
        input_alphabet_errors_.contains(AlphabetErrorCode::kMultipleLetters)) {
        input_alphabet_errors_.erase(AlphabetErrorCode::kMultipleLetters);
        emit OnRemovedAlphabetError(AlphabetType::kInput,
                                    AlphabetErrorCode::kMultipleLetters);
    } else if (!multiplied_letters.isEmpty() &&
               !input_alphabet_errors_.contains(
                   AlphabetErrorCode::kMultipleLetters)) {
        QString message{"Multiplied letters: "};
        message += ConcatSet(multiplied_letters);
        emit OnNewAlphabetError(AlphabetType::kInput,
                                AlphabetErrorCode::kMultipleLetters, message);
        input_alphabet_errors_.insert(AlphabetErrorCode::kMultipleLetters);
    }
    if (invalid_letters.isEmpty() &&
        input_alphabet_errors_.contains(
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet)) {
        input_alphabet_errors_.erase(
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet);
        emit OnRemovedAlphabetError(
            AlphabetType::kInput,
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet);
    } else if (!invalid_letters.isEmpty() &&
               !input_alphabet_errors_.contains(
                   AlphabetErrorCode::
                       kRuntimeAlphabetDontContainInputAlphabet)) {
        QString message{"Letters ouside runtime alphabet: "};
        message += ConcatSet(invalid_letters);
        emit OnNewAlphabetError(
            AlphabetType::kInput,
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet,
            message);
        input_alphabet_errors_.insert(
            AlphabetErrorCode::kRuntimeAlphabetDontContainInputAlphabet);
    }

    input_alphabet_ = new_alphabet;
}
