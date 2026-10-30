#include <getopt.h>
#include <qnamespace.h>

#include <QString>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "MarkovAlgorithm.h"
#include "YadSerializer.h"

QString RunProgram(QString word, const MarkovAlgorithm& algorithm) {
    while (true) {
        const MarkovRule* rule = algorithm.getRuleFor(word);
        if (!rule) {
            break;
        }
        word = algorithm.useRule(word, *rule);
        if (rule->isFinalRule()) {
            break;
        }
    }
    return word;
}

int main(int argc, char* argw[]) {
    std::vector<option> opts;
    opts.push_back(
        option{.name = "file", .has_arg = 1, .flag = nullptr, .val = 'f'});
    opts.push_back(
        option{.name = "verbose", .has_arg = 1, .flag = nullptr, .val = 'v'});
    opts.push_back(option{
        .name = "json_output", .has_arg = 1, .flag = nullptr, .val = 'j'});
    opts.push_back({});
    memset(&opts.back(), 0, sizeof(option));

    std::string file_path;
    bool json_output = false;
    bool verbose = false;
    int arg_value;
    int opt_index;
    while ((arg_value =
                getopt_long(argc, argw, "f:jv", opts.data(), nullptr)) != -1) {
        switch (arg_value) {
            case ':':
                std::cerr << "Required parameter is missed!\n";
                break;
            case '?':
                break;
            case 'f':
                file_path = optarg;
                break;
            case 'j':
                json_output = true;
                break;
            case 'v':
                verbose = true;
                break;
            default:
                std::cerr << "?? getopt return unexpected result: " << arg_value
                          << '\n';
                break;
        }
    };

    if (file_path.empty()) {
        std::cerr << "Path to file is not set!\n";
        return 1;
    }
    std::string input_line;
    std::getline(std::cin, input_line);

    if (verbose) {
        std::cerr << "File path: " << file_path << '\n';
        std::cerr << "Input line: " << input_line << '\n';
    }

    YadSerializer serializer(QString{file_path.c_str()});

    YadSerializer::NAMClosure closure = serializer.DeserializeRaw();

    MarkovAlgorithm algorithm{closure.runtime_alphabet, closure.rules,
                              closure.input_alphabet};

    bool input_valid = true;
    MarkovAlphabet& alphabet = closure.input_alphabet.isAuto()
                                   ? closure.runtime_alphabet
                                   : closure.input_alphabet;
    for (const char c : input_line) {
        if (!alphabet.getAlphabet().contains(QChar(c))) {
            input_valid = false;
            std::cerr << "Input char: " << c << " outside program alphabet!\n";
        }
    }
    if (!input_valid) {
        return 1;
    }

    QString output = RunProgram(QString{input_line.c_str()}, algorithm);
    if (json_output) {
        std::cout << "{\"out_line\": \"" << output.toStdString() << "\"}\n";
    } else {
        std::cout << output.toStdString() << '\n';
    }

    return 0;
}
