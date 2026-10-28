#include "AlphabetsDisplayWidget.h"

#include "ui_AlphabetsDisplayWidget.h"

AlphabetsDisplayWidget::AlphabetsDisplayWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::AlphabetsDisplayWidget) {
    ui->setupUi(this);
}

AlphabetsDisplayWidget::~AlphabetsDisplayWidget() { delete ui; }
