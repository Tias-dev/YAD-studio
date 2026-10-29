#ifndef COMPILERERROR_H
#define COMPILERERROR_H
#include "QString"

class CompilerError {
   public:
    CompilerError(const QString& title = "", const QString& description = "",
                  int error_number = -1, int line_numer = -1);

    int getErrorNumber() const;
    int getLineNumber() const;
    const QString& getErrorTitle() const;
    const QString& getErrorDescription() const;
    /**
     * @brief getFullErrorInfo
     * @return ErrorTitle + ErrorDescription
     */
    const QString getFullErrorInfo() const;

    void setErrorNumber(int number);
    void setLineNumber(int line);
    void setErrorTitle(const QString& title);
    void setErrorDescription(const QString& description);

   private:
    // each error type has own number
    int _error_number;

    // number of line, where the error was occurred
    int _line_numer;

    QString _title;
    QString _description;
};

namespace errors {
CompilerError ForbiddenRuleError(int line, QString letter);
CompilerError WrongRuleDefenition(int line, QString text);
CompilerError EmptySymbolInAlphabet(int line);
CompilerError NotInAlphabetError(int line, QString letter);
}  // namespace errors

#endif  // COMPILERERROR_H
