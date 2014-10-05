#ifndef TRIANGLESADDRESSVALIDATOR_H
#define TRIANGLESADDRESSVALIDATOR_H

#include <QRegExpValidator>

/** Base48 entry widget validator.
   Corrects near-miss characters and refuses characters that are no part of base48.
 */
class TrianglesAddressValidator : public QValidator
{
    Q_OBJECT
public:
    explicit TrianglesAddressValidator(QObject *parent = 0);

    State validate(QString &input, int &pos) const;

    static const int MaxAddressLength = 128;
signals:

public slots:

};

#endif // TRIANGLESADDRESSVALIDATOR_H
