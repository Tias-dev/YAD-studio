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

    void SetError(QString message);
    void ClearError();

   public slots:
    void SetAlphabetType(AlphabetType new_alphabet_type);
    void SetAlphabet(MarkovAlphabet new_alphabet);

   private:
    AlphabetType type_ = AlphabetType::kNone;
    MarkovAlphabet alphabet_;
    Ui::AlphabetWidget* ui;
};

#endif  // !ALPHABET_WIDGET_H
