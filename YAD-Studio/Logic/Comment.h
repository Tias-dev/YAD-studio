#ifndef COMMENT_H
#define COMMENT_H

#include <QString>
#include <cstddef>

/**
 * @class Comment
 * @brief Contains comment with position
 */
struct Comment {
    size_t line;
    QString content;
};

#endif  //! COMMENT_H
