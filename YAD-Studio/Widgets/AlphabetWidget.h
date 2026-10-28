#ifndef ALPHABET_WIDGET_H
#define ALPHABET_WIDGET_H

#include <QLineEdit>

#include "MarkovAlphabet.h"
namespace Ui {
class AlphabetWidget;
}

class AlphabetWidget : public QWidget {
    Q_OBJECT
   public:
    explicit AlphabetWidget(QWidget* parent = 0);
    ~AlphabetWidget();

    enum class AlphabetType { kNone, kRuntime, kInput };

    void SetAlphabetType(AlphabetType new_alphabet_type);
    void SetAlphabet(MarkovAlphabet new_alphabet);

    void SetError(QString message);
    void ClearError();

   private:
    AlphabetType type_ = AlphabetType::kNone;
    MarkovAlphabet alphabet_;
    Ui::AlphabetWidget* ui;
};

#endif  // !ALPHABET_WIDGET_H
