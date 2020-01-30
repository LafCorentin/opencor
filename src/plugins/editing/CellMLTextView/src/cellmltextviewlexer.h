/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://gnu.org/licenses>.

*******************************************************************************/

//==============================================================================
// Lexer for the CellML Text format
//==============================================================================

#pragma once

//==============================================================================

#include <QRegularExpression>

//==============================================================================

#include "qscintillabegin.h"
    #include "Qsci/qscilexercustom.h"
#include "qscintillaend.h"

//==============================================================================

namespace OpenCOR {
namespace CellMLTextView {

//==============================================================================

class CellmlTextViewLexer : public QsciLexerCustom
{
    Q_OBJECT

public:
    enum class Style {
        Default,
        SingleLineComment,
        MultilineComment,
        Keyword,
        CellmlKeyword,
        Number,
        String,
        ParameterBlock,
        ParameterKeyword,
        ParameterCellmlKeyword,
        ParameterNumber,
        ParameterString
    };

    explicit CellmlTextViewLexer(QObject *pParent);

    const char * language() const override;

    QString description(int pStyle) const override;

    QColor color(int pStyle) const override;
    QFont font(int pStyle) const override;

    void styleText(int pBytesStart, int pBytesEnd) override;

private:
    QString mFullText;
    QByteArray mFullTextUtf8;

    QString mEolString;

    void applyStyle(int pBytesStart, int pBytesEnd, Style pStyle);
    void styleText(int pBytesStart, int pBytesEnd, const QString &pText,
                   bool pParameterBlock);
    void styleTextCurrent(int pBytesStart, int pBytesEnd, const QString &pText,
                          bool pParameterBlock);
    void styleTextPreviousMultilineComment(int pPosition, int pBytesStart,
                                           int pBytesEnd, const QString &pText,
                                           bool pParameterBlock);
    void styleTextPreviousParameterBlock(int pPosition, int pBytesStart,
                                         int pBytesEnd, const QString &pText,
                                         bool pParameterBlock);
    void styleTextString(int pPosition, int pBytesStart, int pBytesEnd,
                         const QString &pText, bool pParameterBlock);
    void styleTextRegEx(int pBytesStart, const QString &pText,
                        const QRegularExpression &pRegEx, Style pStyle);
    void styleTextNumber(int pBytesStart, const QString &pText, Style pStyle);

    bool validString(int pFrom, int pTo, Style pStyle) const;
    int findString(const QString &pString, int pFrom, Style pStyle,
                   bool pForward = true);

    int fullTextPosition(int pBytesPosition) const;
    int fullTextLength(int pBytesStart, int pBytesEnd) const;

    int fullTextBytesPosition(int pPosition) const;
    int textBytesPosition(const QString &pText, int pPosition) const;

signals:
    void done();
};

//==============================================================================

} // namespace CellMLTextView
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
