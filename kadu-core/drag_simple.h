#ifndef DRAGSIMPLE_H
#define DRAGSIMPLE_H

#include <qglobal.h>

#include <q3dragobject.h>

class DragSimple : public Q3TextDrag
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
