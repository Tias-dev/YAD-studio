#ifndef MARKOVPARSER_H
#define MARKOVPARSER_H
#include <QString>
#include <vector>

#include "Comment.h"
#include "CompilerError.h"
#include "MarkovAlgorithm.h"
#include "MarkovAlphabet.h"
#include "MarkovRule.h"

class MarkovParser {
   public:
    /**
     * @brief parses the source code
     * @param source_code code to parse
     * @param input_alphabet
     * @param runtime_alphabet
     * @param errors
     * @return parsed markov rules
     */
    static std::vector<MarkovRule> parseSourceCode(
        QString source_code, const MarkovAlphabet& algorithm,
        std::vector<CompilerError>& errors, std::vector<Comment>& comments);

   private:
    MarkovParser();
};

#endif  // MARKOVPARSER_H
