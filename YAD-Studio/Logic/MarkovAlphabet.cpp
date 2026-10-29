#include "MarkovAlphabet.h"

#include <queue>

MarkovAlphabet::MarkovAlphabet(const QSet<QChar>& alphabeth)
    : _chars(alphabeth), _is_auto(false) {}

MarkovAlphabet::MarkovAlphabet(const bool is_auto) : _is_auto(is_auto) {}

QSet<QChar> MarkovAlphabet::getAlphabet() const { return _chars; }

QString MarkovAlphabet::getAllChars() const {
    std::priority_queue<QChar> letters;
    for (const QChar c : _chars) {
        letters.push(c);
    }

    QString result{};
    while (!letters.empty()) {
        result += letters.top();
        letters.pop();
    }
    return result;
}

bool MarkovAlphabet::isAuto() const { return _is_auto; }
QString MarkovAlphabet::getSource() const { return _source; }
void MarkovAlphabet::setSource(QString source) { _source = source; }

bool MarkovAlphabet::isInAlphabet(QChar c) const {
    // empty symbol
    if (_is_auto) return true;

    return _chars.find(c) != _chars.end();
}
