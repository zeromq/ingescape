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


#ifndef QMLSYNTAXHIGHLIGHTER_H
#define QMLSYNTAXHIGHLIGHTER_H

#include <QObject>
#include <QtQml>
#include <QSyntaxHighlighter>
#include <QQuickTextDocument>

#include "utils/propertieshelpers.h"



/**
 * @brief Categories of syntax elements
 */
PLAYGROUND_QML_ENUM(QMLSyntaxColor,
                    DEFAULT,
                    COMMENT,
                    NUMBER,
                    STRING,
                    OPERATOR,
                    KEYWORD,
                    JAVASCRIPT,
                    ITEM,
                    PROPERTY
                    )




/**
 * @brief The QMLSyntaxHighlighter class is used to highlight keywords in a code editor
 */
class QMLSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    // Text document associated to a QQuickTextEdit i.e. a QML TextEdit item
    PLAYGROUND_QML_PROPERTY_CUSTOM_SETTER(QQuickTextDocument*, quickTextDocument)


public:
    /**
     * @brief Default constructor
     */
    explicit QMLSyntaxHighlighter();


    /**
     * @brief Destructor
     */
    ~QMLSyntaxHighlighter();


    /**
     * @brief Load a given dictionary
     * @param filepath
     * @param result
     */
    void loadDictionary(QString filepath, QSet<QString> &result);



private Q_SLOTS:
    /**
     * @brief Called when our quickTextDocument is destroyed
     */
    void _onQuickTextDocumentDestroyed(QObject*);


protected:
    /**
     * @brief Called when necessary by the rich text engine to highlight a given rich-text
     *
     * @param text
     */
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;


protected:
    // Our palette of colors
    QHash<QMLSyntaxColor::Value, QColor> _colors;

    // List of QML keywords
    QSet<QString> _keywords;

    // List of javascript keywords
    QSet<QString> _javascript;

    // List of properties
    QSet<QString> _properties;

    // List of QML items
    QSet<QString> _items;
};

QML_DECLARE_TYPE(QSyntaxHighlighter)

#endif // QMLSYNTAXHIGHLIGHTER_H
