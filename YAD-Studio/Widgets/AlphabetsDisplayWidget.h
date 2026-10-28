#ifndef ALPHABETSDISPLAYWIDGET_H
#define ALPHABETSDISPLAYWIDGET_H

#include <QWidget>

namespace Ui {
class AlphabetsDisplayWidget;
}

class AlphabetsDisplayWidget : public QWidget {
    Q_OBJECT

   public:
    explicit AlphabetsDisplayWidget(QWidget* parent = nullptr);
    ~AlphabetsDisplayWidget();

   signals:
    void onRuntimeAlphabetChanged(QString new_alphabet);
    void onInputAlphabetChanged(QString new_alphabet);

   private:
    Ui::AlphabetsDisplayWidget* ui;
};

#endif  // ALPHABETSDISPLAYWIDGET_H
