#ifndef PREVIEW_H
#define PREVIEW_H

#include <qglobal.h>

#include <qobject.h>

#include "userlistelement.h"
#include "usergroup.h"

#include "kadu_text_browser.h"

class Preview : public KaduTextBrowser
{
	Q_OBJECT

	UserListElement ule;
	QObjectList objectsToParse;
	UserListElements ules;
	QString resetBackgroundColor;

public:
	Preview(QWidget *parent = 0, char *name = 0);
	~Preview();

	void setResetBackgroundColor(const QString &resetBackgroundColor) { this->resetBackgroundColor = resetBackgroundColor; }
	void addObjectToParse(UserListElement ule, QObject *object) { ules.append(ule); objectsToParse.append(object); }

public slots:
	void syntaxChanged(const QString &content);

signals:
	void needSyntaxFixup(QString &syntax);
	void needFixup(Preview *preview);

};

#endif // PREVIEW_H
