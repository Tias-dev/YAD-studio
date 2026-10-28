#include "YadFile.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringList>

YadFile::YadFile(QString source_code, std::vector<QString> history)
    : _source_code(source_code), _history(history) {}

QString YadFile::getSourceCode() const { return _source_code; }

std::vector<QString> YadFile::getHistory() const { return _history; }
