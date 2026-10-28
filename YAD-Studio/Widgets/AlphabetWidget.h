#ifndef ALPHABET_WIDGET_H
#define ALPHABET_WIDGET_H

#include <QLineEdit>

class AlphabetWidget : public QLineEdit {
    Q_OBJECT
   public:
    explicit AlphabetWidget(QWidget* parent = 0);
    ~AlphabetWidget();

    enum class AlphabetType { kRuntime, kInput };

    void SetAlphabetType(AlphabetType new_alphabet_type);
    void SetAlphabit(QString new_alphabet);
   signals:
    void onAlphabetChanged(QString new_alphabet);

   private:
    QString alphabet_;
};

#endif  // !ALPHABET_WIDGET_H
