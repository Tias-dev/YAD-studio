#include "MarkovAlgorithm.h"

MarkovAlgorithm::MarkovAlgorithm(MarkovAlphabet alphabet,
                                 std::vector<MarkovRule> rules,
                                 MarkovAlphabet input_alphabet)
    : _alphabet(alphabet), _rules(rules), _input_alphabet(input_alphabet) {}

void MarkovAlgorithm::setAlphabet(const MarkovAlphabet& new_alphabet) {
    _alphabet = new_alphabet;
}

void MarkovAlgorithm::setInputAlphabet(const MarkovAlphabet& new_alphabet) {
    _input_alphabet = new_alphabet;
}

bool MarkovAlgorithm::RuleFitResult::isValid() const {
    return start_index >= 0 && length >= 0;
}

MarkovAlgorithm::RuleFitResult::RuleFitResult(int start_index_, int length_)
    : start_index(start_index_), length(length_) {}

MarkovAlgorithm::RuleFitResult MarkovAlgorithm::isRuleFits(
    QString input_word, const MarkovRule& rule) {
    QString pattern = rule.getLeftPart();
    QString pattern_empty = pattern.replace("$", "");

    // Empty string matches the beginning
    if (pattern_empty.isEmpty()) return RuleFitResult(0, 0);

    int index = input_word.indexOf(pattern_empty);
    if (index < 0) return RuleFitResult(-1, -1);
    return RuleFitResult(index, pattern_empty.length());
}
QString MarkovAlgorithm::useRule(QString input_word, const MarkovRule& rule) {
    QString res = "";

    RuleFitResult rule_fits = isRuleFits(input_word, rule);
    res = input_word.mid(0, rule_fits.start_index);
    res += rule.getRightPart();
    res += input_word.mid(rule_fits.start_index + rule_fits.length,
                          input_word.size());
    res = res.replace("$", "");
    return res;
}

const MarkovRule* MarkovAlgorithm::getRuleFor(QString input) const {
    const MarkovRule* res = nullptr;

    for (int i = 0; i < _rules.size(); ++i) {
        RuleFitResult fit = isRuleFits(input, _rules[i]);
        if (fit.isValid()) {
            res = &_rules[i];
            break;
        }
    }

    return res;
}
const MarkovAlphabet& MarkovAlgorithm::getInputAlphabet() const {
    return _input_alphabet;
}
const MarkovAlphabet& MarkovAlgorithm::getAlphabet() const { return _alphabet; }

std::vector<MarkovRule> MarkovAlgorithm::getRules() const { return _rules; }
