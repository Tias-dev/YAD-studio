#ifndef ALPHABETSDISPLAYWIDGET_H
#define ALPHABETSDISPLAYWIDGET_H

#include <QWidget>
#include <map>

#include "AlphabetsManager.h"

namespace Ui {
class AlphabetsDisplayWidget;
}

class AlphabetsDisplayWidget : public QWidget {
    Q_OBJECT

   public:
    explicit AlphabetsDisplayWidget(QWidget* parent = nullptr);
    ~AlphabetsDisplayWidget();

   private:
    std::map<AlphabetsManager::AlphabetErrorCode, QString>
        runtime_alphabet_errors_;
    std::map<AlphabetsManager::AlphabetErrorCode, QString>
        input_alphabet_errors_;

    Ui::AlphabetsDisplayWidget* ui;
};

#endif  // ALPHABETSDISPLAYWIDGET_H
