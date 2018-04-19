/*
 *  Mastic - QML playground
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#include "qmlsyntaxhighlighter.h"

#include <QTextCharFormat>
#include <QDebug>



/**
 * @brief QMLSyntaxColor - pretty print of enum value
 * @param value
 * @return
 */
QString QMLSyntaxColor::enumToString(int value)
{
    Q_UNUSED(value)

    return QString("");
}





/**
 * @brief Constructor
 * @param parent
 */
QMLSyntaxHighlighter::QMLSyntaxHighlighter()
    :  QSyntaxHighlighter(static_cast<QTextDocument *>(NULL)),
      _quickTextDocument(NULL)
{
    //
    // Colors
    //
    _colors[QMLSyntaxColor::DEFAULT] = QColor("#000000");
    _colors[QMLSyntaxColor::COMMENT] = QColor("#008000");
    _colors[QMLSyntaxColor::NUMBER] = QColor("#000000");
    _colors[QMLSyntaxColor::STRING] = QColor("#008000");
    _colors[QMLSyntaxColor::OPERATOR] = QColor("#000000");
    _colors[QMLSyntaxColor::KEYWORD] = QColor("#808000");
    _colors[QMLSyntaxColor::JAVASCRIPT] = QColor("#0055af");
    _colors[QMLSyntaxColor::ITEM] = QColor("#800080");
    _colors[QMLSyntaxColor::PROPERTY] = QColor("#800000");


    //
    //  Load our list of keywords
    //
    loadDictionary(":/resources/dictionaries/keywords.txt", _keywords);
    loadDictionary(":/resources/dictionaries/javascript.txt", _javascript);
    loadDictionary(":/resources/dictionaries/items.txt", _items);
    loadDictionary(":/resources/dictionaries/properties.txt", _properties);
}


/**
 * @brief Destructor
 */
QMLSyntaxHighlighter::~QMLSyntaxHighlighter()
{
    setquickTextDocument(NULL);
    setDocument(NULL);
}



/**
 * @brief Set the QQuickTextDocument associated to our syntax highlighter
 * @param value
 */
void QMLSyntaxHighlighter::setquickTextDocument(QQuickTextDocument* value)
{
    if (_quickTextDocument != value)
    {
        // Clean-up if needed
        if (_quickTextDocument != NULL)
        {
            disconnect(_quickTextDocument, 0, this, 0);
        }

        // Save our new value
        _quickTextDocument = value;

        // Update our syntax highlighter
        if (_quickTextDocument != NULL)
        {
            // Set our document
            setDocument(_quickTextDocument->textDocument());

            // Subscribe to destruction
            connect(_quickTextDocument, &QQuickTextDocument::destroyed, this, &QMLSyntaxHighlighter::_onQuickTextDocumentDestroyed);
        }
        else
        {
            // Clean-up our document
            setDocument(NULL);
        }
    }
}


/**
 * @brief Load a given dictionary
 * @param filepath
 * @param result
 */
void QMLSyntaxHighlighter::loadDictionary(QString filepath, QSet<QString> &result)
{
    QFile file(filepath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream textStream(&file);
        while (!textStream.atEnd())
        {
            QString lineContent = textStream.readLine().trimmed();
            if (!lineContent.isEmpty() && !lineContent.startsWith("//"))
            {
                result << lineContent;
            }
        }
    }
}


/**
 * @brief Called when necessary by the rich text engine to highlight a given rich-text
 *
 * @param text
 */
void QMLSyntaxHighlighter::highlightBlock(const QString &text)
{
    enum {
        StartState = 0,
        NumberState = 1,
        IdentifierState = 2,
        StringState = 3,
        CommentState = 4
    };

    QList<int> bracketPositions;

    int blockState = previousBlockState();
    int bracketLevel = blockState >> 4;
    int state = blockState & 15;
    if (blockState < 0)
    {
        bracketLevel = 0;
        state = StartState;
    }

    int start = 0;
    int index = 0;
    while (index <= text.length())
    {
        QChar currentChar = (index < text.length()) ? text.at(index) : QChar();
        QChar nextChar = (index < text.length() - 1) ? text.at(index + 1) : QChar();

        switch (state) {
            case StartState:
                {
                    start = index;
                    if (currentChar.isSpace())
                    {
                        ++index;
                    }
                    else if (currentChar.isDigit())
                    {
                        ++index;
                        state = NumberState;
                    }
                    else if (currentChar.isLetter() || currentChar == '_')
                    {
                        ++index;
                        state = IdentifierState;
                    }
                    else if (currentChar == '\'' || currentChar == '\"')
                    {
                        ++index;
                        state = StringState;
                    }
                    else if (currentChar == '/' && nextChar == '*') {
                        ++index;
                        ++index;
                        state = CommentState;
                    }
                    else if (currentChar == '/' && nextChar == '/')
                    {
                        index = text.length();
                        setFormat(start, text.length(), _colors[QMLSyntaxColor::COMMENT]);
                    }
                    else
                    {
                        if (!QString("(){}[]").contains(currentChar))
                        {
                            setFormat(start, 1, _colors[QMLSyntaxColor::OPERATOR]);
                        }

                        if (currentChar =='{' || currentChar == '}')
                        {
                            bracketPositions += index;
                            if (currentChar == '{')
                            {
                                bracketLevel++;
                            }
                            else
                            {
                                bracketLevel--;
                            }
                        }

                        ++index;
                        state = StartState;
                    }
                }
                break;

            case NumberState:
                {
                    if (currentChar.isSpace() || !currentChar.isDigit()) {
                        setFormat(start, index - start, _colors[QMLSyntaxColor::NUMBER]);
                        state = StartState;
                    }
                    else
                    {
                        ++index;
                    }
                }
                break;

            case IdentifierState:
                {
                    if (currentChar.isSpace() || !(currentChar.isDigit() || currentChar.isLetter() || currentChar == '_'))
                    {
                        QString token = text.mid(start, index - start).trimmed();
                        if (_keywords.contains(token))
                        {
                            setFormat(start, index - start, _colors[QMLSyntaxColor::KEYWORD]);
                        }
                        else if (_items.contains(token))
                        {
                            setFormat(start, index - start, _colors[QMLSyntaxColor::ITEM]);
                        }
                        else if (_properties.contains(token))
                        {
                            setFormat(start, index - start, _colors[QMLSyntaxColor::PROPERTY]);
                        }
                        else if (_javascript.contains(token))
                        {
                            setFormat(start, index - start, _colors[QMLSyntaxColor::JAVASCRIPT]);
                        }

                        state = StartState;
                    }
                    else
                    {
                        ++index;
                    }
                }
                break;

            case StringState:
                {
                    if (currentChar == text.at(start)) {
                        QChar previousChar = (index > 0) ? text.at(index - 1) : QChar();
                        if (previousChar != '\\')
                        {
                            ++index;
                            setFormat(start, index - start, _colors[QMLSyntaxColor::STRING]);
                            state = StartState;
                        }
                        else
                        {
                            ++index;
                        }
                    }
                    else
                    {
                        ++index;
                    }
                }
                break;

            case CommentState:
                {
                    if ((currentChar == '*') && (nextChar == '/'))
                    {
                        ++index;
                        ++index;
                        setFormat(start, index - start, _colors[QMLSyntaxColor::COMMENT]);
                        state = StartState;
                    }
                    else
                    {
                        ++index;
                    }
                }
                break;

            default:
                {
                    state = StartState;
                }
                break;
            }
    }

    if (state == CommentState)
    {
        setFormat(start, text.length(), _colors[QMLSyntaxColor::COMMENT]);
    }
    else
    {
        state = StartState;
    }


    blockState = (state & 15) | (bracketLevel << 4);
    setCurrentBlockState(blockState);
}



/**
 * @brief Called when our quickTextDocument is destroyed
 */
void QMLSyntaxHighlighter::_onQuickTextDocumentDestroyed(QObject*)
{
    // Clean-up
    _quickTextDocument = NULL;
    setDocument(NULL);

    // Notify change
    Q_EMIT quickTextDocumentChanged(NULL);
}
