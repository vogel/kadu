#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QString;

class Highlighter : public QSyntaxHighlighter
{
	Q_OBJECT

	static QList<Highlighter *> highlighters;
	static QTextCharFormat highlightFormat;

public:
        Highlighter(QTextDocument* document);
        ~Highlighter();

        virtual void highlightBlock(const QString& text);

	static void rehighlightAll();
	static void setHighlightFormat(QTextCharFormat format);
};

#endif

