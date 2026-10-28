#ifndef YADFILE_SERIALIZER_H
#define YADFILE_SERIALIZER_H

#include <MarkovAlgorithm.h>
#include <MarkovAlphabet.h>
#include <MarkovRule.h>

#include <QString>
#include <vector>

#include "Comment.h"
#include "YadFile.h"

/**
 * @class YadFileSerializer
 * @brief Serializes and deserializes NAM program
 * in JSON format
 */
class YadSerializer {
   public:
    YadSerializer(QString file_name);

    /**
     * @brief Serializes program
     *
     * @param program
     */
    void Serialize(const YadFile& yad_file) const;
    /**
     * @brief Deserialize program
     *
     * @return structured program data
     */
    YadFile Deserialize() const;

   private:
    /**
     * @class NAMClosure
     * @brief all NAM program data
     */
    struct NAMClosure {
        MarkovAlphabet input_alphabet, runtime_alphabet;
        std::vector<MarkovRule> rules;
        std::vector<Comment> comments;
        std::vector<QString> history;
    };

    /**
     * @brief Inner serializer function
     * std::runtime_error on error
     *
     * @param closure
     */
    void Serialize_(const NAMClosure& closure) const;

    /**
     * @brief Inner deserializer function
     * std::runtime_error on error
     *
     * @return NAM program closure
     */
    NAMClosure Deserialize_() const;

   private:
    QString file_name_;
};

#endif  //! YADFILE_SERIALIZER_H
