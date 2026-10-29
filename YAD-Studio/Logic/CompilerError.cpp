#include "CompilerError.h"

#include <QObject>

CompilerError::CompilerError(const QString& title, const QString& description,
                             int error_number, int line_numer)
    : _title(title),
      _description(description),
      _error_number(error_number),
      _line_numer(line_numer) {}
int CompilerError::getErrorNumber() const { return _error_number; }

int CompilerError::getLineNumber() const { return _line_numer; }

const QString& CompilerError::getErrorTitle() const { return _title; }

const QString& CompilerError::getErrorDescription() const {
    return _description;
}

const QString CompilerError::getFullErrorInfo() const {
    return _title + ": " + _description;
}

void CompilerError::setErrorNumber(int number) { _error_number = number; }

void CompilerError::setLineNumber(int line) { _line_numer = line; }

void CompilerError::setErrorTitle(const QString& title) { _title = title; }

void CompilerError::setErrorDescription(const QString& description) {
    _description = description;
}

namespace errors {

CompilerError WrongRuleDefenition(int line, QString text) {
    QString title = QObject::tr("Wrong rule definition");
    QString description = QObject::tr(
                              "Definition of rule is wrong. ('%1'). Example: "
                              "'aa -> bb' or 'aa ->. bb'")
                              .arg(text);

    return CompilerError(title, description, 203, line);
}

CompilerError NotInAlphabetError(int line, QString letter) {
    QString title = QObject::tr("Symbol '%1' is not in alphabet").arg(letter);
    QString description =
        QObject::tr("You can add it to alphabet in the field above");

    return CompilerError(title, description, 200, line);
}
}  // namespace errors
