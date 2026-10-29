#include "MarkovParser.h"

#include <qcontainerfwd.h>
#include <qregularexpression.h>
#include <qstringview.h>

#include <QDebug>
#include <QRegularExpression>
#include <utility>

#include "CompilerError.h"
#include "MarkovAlphabet.h"

std::pair<QString, QString> splitByComment(QString line) {
    QString command = line, comment;
    int comments_index = line.indexOf("//");
    if (comments_index >= 0) {
        command = line.sliced(0, comments_index);
        comment = line.sliced(comments_index + 2);
    }
    return {command, comment};
}

QString removeComments(QString line) {
    int comments_index = line.indexOf("//");
    if (comments_index >= 0) return line.mid(0, comments_index);
    return line;
}

QString trim(QString line) { return line.trimmed(); }

bool isCharsInAlphabet(int line_number, QString text,
                       std::vector<CompilerError>& errors,
                       const MarkovAlphabet& alphabet) {
    bool res = true;
    for (int i = 0; i < text.size(); ++i) {
        QChar ch = text[i];
        if (!alphabet.isInAlphabet(ch)) {
            errors.push_back(errors::NotInAlphabetError(line_number, ch));
            res = false;
            break;
        }
    }
    return res;
}

void parseRule(int line_number, QString line,
               std::vector<CompilerError>& errors,
               std::vector<MarkovRule>& rules, const MarkovAlphabet& alphabet) {
    long transition_pos = line.indexOf("->");
    long transition_size = 2;
    if (transition_pos < 0) {
        errors.push_back(errors::WrongRuleDefenition(line_number, line));
        return;
    }

    bool is_final = line.size() > transition_pos + transition_size &&
                    line.sliced(transition_pos, transition_size + 1) == "->.";
    if (is_final) {
        ++transition_size;
    }

    // Read rule
    QString left_part = trim(line.sliced(0, transition_pos));
    QString right_part = trim(line.sliced(transition_pos + transition_size));

    if (isCharsInAlphabet(line_number, left_part, errors, alphabet) &&
        isCharsInAlphabet(line_number, right_part, errors, alphabet)) {
        MarkovRule rule(left_part, right_part, is_final, line_number);
        rules.push_back(rule);
#ifndef QT_NO_DEBUG
        qDebug() << "Rule: " << left_part << is_final << right_part;
#endif
    }
}

std::vector<MarkovRule> MarkovParser::parseSourceCode(
    QString source_code, const MarkovAlphabet& runtime_alphabet,
    std::vector<CompilerError>& errors, std::vector<Comment>& comments) {
    QStringList lines = source_code.split(QRegularExpression("[\r\n]"));

    errors.clear();
    comments.clear();

    MarkovAlphabet alphabet(true);
    MarkovAlphabet input_alphabet(true);
    std::vector<MarkovRule> rules;

    for (int i = 0; i < lines.size(); ++i) {
        int line_number = i + 1;
        auto [line, comment] = splitByComment(lines[i]);
        if (!comment.isEmpty()) {
            comments.push_back(
                {.line = static_cast<size_t>(line_number), .content = comment});
        }
        line = trim(line);

        if (!line.isEmpty()) {
            parseRule(line_number, line, errors, rules, runtime_alphabet);
        }
    }

    return rules;
}
