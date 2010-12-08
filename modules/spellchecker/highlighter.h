#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QtGui/QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
	Q_OBJECT

	static QList<Highlighter *> Highlighters;
	static QTextCharFormat HighlightFormat;

public:
	explicit Highlighter(QTextDocument *document);
	virtual ~Highlighter();

	virtual void highlightBlock(const QString &text);

	static void rehighlightAll();
	static void setHighlightFormat(const QTextCharFormat &format);

};

#endif // HIGHLIGHTER_H

