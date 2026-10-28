#ifndef YADFILE_H
#define YADFILE_H
#include <QString>
#include <vector>

/**
 * @brief Saves and loads data from file
 */
class YadFile {
   public:
    /**
     * @brief YadFile
     * @param source_code complete source code (with comments)
     * @param history history of launches
     */
    YadFile(QString source_code, std::vector<QString> history);

    /**
     * @brief Returns source code
     * @return
     */
    QString getSourceCode() const;

    /**
     * @brief Returns history of launches
     * @return
     */
    std::vector<QString> getHistory() const;

   private:
    /**
     * @brief complete source code (with comments)
     */
    QString _source_code;
    /**
     * @brief history of launches
     */
    std::vector<QString> _history;
};

#endif  // YADFILE_H
