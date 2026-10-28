#include "HistoryManager.h"

#include <algorithm>

HistoryManager* HistoryManager::_instance = nullptr;

HistoryManager::HistoryManager() {}
HistoryManager* HistoryManager::getInstance() {
    if (_instance == nullptr) _instance = new HistoryManager();
    return _instance;
}
std::vector<QString> HistoryManager::getHistory() { return _history; }

void HistoryManager::addToHistory(QString input_word) {
    if (input_word.size() && std::find(std::begin(_history), std::end(_history),
                                       input_word) == std::end(_history)) {
        _history.push_back(input_word);
    }

    emit historyChanged(_history);
}

void HistoryManager::removeFromHistory(QString input_word) {
    _history.erase(
        std::find(std::begin(_history), std::end(_history), input_word));
    emit historyChanged(_history);
}

void HistoryManager::clearHistory() {
    _history.clear();

    emit historyChanged(_history);
}

void HistoryManager::addToHistory(std::vector<QString> words) {
    bool new_history_items = false;
    for (const QString& word : words) {
        if (std::find(std::begin(_history), std::end(_history), word) ==
            std::end(_history)) {
            _history.push_back(word);
            new_history_items = true;
        }
    }

    if (new_history_items) {
        emit historyChanged(_history);
    }
}
