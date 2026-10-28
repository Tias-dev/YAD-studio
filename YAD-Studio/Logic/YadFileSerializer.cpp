#include "YadFileSerializer.h"

#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qset.h>

#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <format>
#include <stdexcept>

#include "MarkovAlgorithm.h"
#include "MarkovAlphabet.h"
#include "MarkovRule.h"

YadFileSerializer::YadFileSerializer(QString file_name)
    : file_name_(file_name) {}

void YadFileSerializer::Serialize(const MarkovAlgorithm& program,
                                  const std::vector<Comment>& comments,
                                  const std::vector<QString>& history) const {
    NAMClosure closure;
    closure.comments = comments;

    closure.input_alphabet = program.getInputAlphabet();
    closure.runtime_alphabet = program.getAlphabet();
    closure.rules = program.getRules();
    closure.history = history;

    Serialize_(closure);
}

YadFileSerializer::DeserializeResult YadFileSerializer::Deserialize() const {
    NAMClosure closure = Deserialize_();

    return DeserializeResult{
        .program = MarkovAlgorithm{closure.runtime_alphabet, closure.rules,
                                   closure.input_alphabet},
        .comments = closure.comments,
        .history = closure.history};
}

void YadFileSerializer::Serialize_(const NAMClosure& closure) const {
    QJsonArray rules;
    for (const MarkovRule& rule : closure.rules) {
        QJsonObject rule_object;
        rule_object.insert("lhs", rule.getLeftPart());
        rule_object.insert("rhs", rule.getRightPart());
        rule_object.insert("term", rule.isFinalRule());
        rule_object.insert("n_line", rule.getLineNumber());
        rules.push_back(rule_object);
    }

    QJsonArray comments;
    for (const Comment& comment : closure.comments) {
        QJsonObject comment_object;
        comment_object.insert("n_line",
                              QJsonValue{static_cast<qint64>(comment.line)});
        comment_object.insert("offset", static_cast<qint64>(comment.offset));
        comment_object.insert("value", comment.content);
    }

    QJsonArray history;
    for (const QString& history_entry : closure.history) {
        history.push_back(history_entry);
    }

    QJsonObject program_object;
    program_object.insert("rules", rules);
    program_object.insert("comments", comments);
    program_object.insert("history", history);
    program_object.insert("input_alphabet",
                          closure.input_alphabet.getAllChars());
    program_object.insert("runtime_alphabet",
                          closure.runtime_alphabet.getAllChars());

    QJsonDocument document{program_object};
    QFile file{file_name_};
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error{
            std::format("Can't open file: {}", file_name_.toStdString())};
    }
    file.write(document.toJson(QJsonDocument::Compact));
}

namespace {

template <typename T>
T GetValueFromRef(const QJsonValueConstRef& ref) {}

template <>
QJsonArray GetValueFromRef<QJsonArray>(const QJsonValueConstRef& ref) {
    if (!ref.isArray()) {
        throw std::runtime_error("Expected value to be array but it is not!");
    }
    return ref.toArray();
}

template <>
QJsonObject GetValueFromRef<QJsonObject>(const QJsonValueConstRef& ref) {
    if (!ref.isObject()) {
        throw std::runtime_error("Expected value to be object but it is not!");
    }
    return ref.toObject();
}

template <>
double GetValueFromRef<double>(const QJsonValueConstRef& ref) {
    if (!ref.isDouble()) {
        throw std::runtime_error("Expected value to be double but it is not!");
    }
    return ref.toDouble();
}

template <>
QString GetValueFromRef<QString>(const QJsonValueConstRef& ref) {
    if (!ref.isString()) {
        throw std::runtime_error("Expected value to be string but it is not!");
    }
    return ref.toString();
}

template <>
bool GetValueFromRef<bool>(const QJsonValueConstRef& ref) {
    if (!ref.isString()) {
        throw std::runtime_error("Expected value to be boolean but it is not!");
    }
    return ref.toBool();
}

template <typename T>
T GetKey(const QJsonObject& object, const char* obj_name,
         const char* key_name) {
    if (!object.contains(key_name)) {
        throw std::runtime_error(std::format("Expected key: {} not found in {}",
                                             key_name, obj_name));
    }
    QJsonValueConstRef ref = object.find(key_name).value();
    try {
        return GetValueFromRef<T>(ref);
    } catch (std::runtime_error& e) {
        throw std::runtime_error(
            std::format("Error while parsing: {} for key: {}. Error: {}",
                        obj_name, key_name, e.what()));
    }
}

}  // namespace

YadFileSerializer::NAMClosure YadFileSerializer::Deserialize_() const {
    QFile file{file_name_};
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error{
            std::format("Can't open file: {}", file_name_.toStdString())};
    }
    QByteArray bytes = file.readAll();
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
    if (document.isNull()) {
        throw std::runtime_error(std::format(
            "Can't parse json data from file: {}", file_name_.toStdString()));
    }
    if (!document.isObject()) {
        throw std::runtime_error(
            "File must be valid JSON object but it is not!");
    }

    QJsonObject data = document.object();

    NAMClosure closure;
    QString input_alphabet =
        GetKey<QString>(data, "document", "input_alphabet");
    closure.input_alphabet = MarkovAlphabet{
        QSet<QChar>{std::begin(input_alphabet), std::end(input_alphabet)}};
    QString runtime_alphabet =
        GetKey<QString>(data, "document", "runtime_alphabet");
    closure.runtime_alphabet = MarkovAlphabet{
        QSet<QChar>{std::begin(runtime_alphabet), std::end(runtime_alphabet)}};

    QJsonArray rules = GetKey<QJsonArray>(data, "document", "rules");
    for (const QJsonValue& rule_raw : rules) {
        if (!rule_raw.isObject()) {
            throw std::runtime_error(
                "Expected rule to be JSON object but it is not");
        }
        QJsonObject rule = rule_raw.toObject();
        closure.rules.push_back(
            MarkovRule{GetKey<QString>(rule, "rule", "lhs"),
                       GetKey<QString>(rule, "rule", "rhs"),
                       GetKey<bool>(rule, "rule", "term"),
                       int(GetKey<double>(rule, "rule", "n_line"))});
    }

    QJsonArray comments = GetKey<QJsonArray>(data, "document", "comments");
    for (const QJsonValue& comment_raw : comments) {
        if (!comment_raw.isObject()) {
            throw std::runtime_error(
                "Expected comment to be JSON object but it is not");
        }
        QJsonObject comment = comment_raw.toObject();
        closure.comments.push_back(Comment{
            .line = size_t(GetKey<double>(comment, "comment", "n_line")),
            .offset = size_t(GetKey<double>(comment, "comment", "offset")),
            .content = GetKey<QString>(comment, "comment", "value"),
        });
    }

    QJsonArray history = GetKey<QJsonArray>(data, "document", "history");
    for (const QJsonValue& history_raw : history) {
        if (!history_raw.isString()) {
            throw std::runtime_error(
                "Expected history item to be JSON string but it is not");
        }
        closure.history.push_back(history_raw.toString());
    }

    return closure;
}
