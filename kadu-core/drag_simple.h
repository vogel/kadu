#ifndef DRAGSIMPLE_H
#define DRAGSIMPLE_H

#include <qdragobject.h>

class DragSimple : public QTextDrag
{
	QString MimeType;
	QString Content;

public:
	DragSimple(const QString &mimeType, const QString &content, QWidget* dragSource = 0, const char* name = 0);

	// QMimeFactory
	const char * format(int i) const;
	bool provides(const char *mimeType) const;
	QByteArray encodedData(const char *mimeType) const;

};

#endif
