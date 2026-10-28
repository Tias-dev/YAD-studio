#ifndef ALPHABITS_MANAGER_H
#define ALPHABITS_MANAGER_H

#include <qtmetamacros.h>

#include <QObject>
#include <set>

#include "MarkovAlphabet.h"

class AlphabetsManager : public QObject {
    Q_OBJECT;

   public:
    static AlphabetsManager& Instance();

    enum class AlphabetErrorCode {
        kInvalidLetter,
        kMultipleLetters,
        kRuntimeAlphabetDontContainInputAlphabet,
    };

    enum class AlphabetType { kRuntime, kInput };

   public slots:
    void SetRuntimeAlphabet(MarkovAlphabet new_alphabet);
    void SetInputAlphabet(MarkovAlphabet new_alphabet);

   signals:
    void OnNewRuntimeAlphabet(MarkovAlphabet new_alphabet);
    void OnNewInputAlphabet(MarkovAlphabet new_alphabet);

    void OnNewAlphabetError(AlphabetType type, AlphabetErrorCode code,
                            QString message);
    void OnRemovedAlphabetError(AlphabetType type, AlphabetErrorCode code);

   private:
    void UpdateErrors();
    static AlphabetsManager* instance_;
    std::set<AlphabetErrorCode> runtime_alphabet_errors_;
    std::set<AlphabetErrorCode> input_alphabet_errors_;
    AlphabetsManager() = default;

    MarkovAlphabet runtime_alphabet_, input_alphabet_;
};

#endif  // !ALPHABITS_MANAGER_H
