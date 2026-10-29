#ifndef MARKOVALPHABET_H
#define MARKOVALPHABET_H

#include <QChar>
#include <QSet>
#include <QString>

class MarkovAlphabet {
   public:
    MarkovAlphabet(const QSet<QChar>& alphabeth = QSet<QChar>());

    bool isInAlphabet(QChar c) const;
    QSet<QChar> getAlphabet() const;

    QString getAllChars() const;
    QString getSource() const;
    void setSource(QString source);

    bool isAuto() const;

   private:
    QSet<QChar> _chars;
    QString _all_chars;
    QString _source;
};

#endif  // MARKOVALPHABET_H
