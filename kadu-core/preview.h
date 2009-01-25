/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PREVIEW_H
#define PREVIEW_H

#include "contacts/contact.h"
#include "contacts/contact-list.h"
#include "kadu_text_browser.h"

class Preview : public KaduTextBrowser
{
	Q_OBJECT

	Account *account;
	Contact contact;
	QObjectList objectsToParse;
	ContactList contacts;
	QString resetBackgroundColor;

public:
	Preview(QWidget *parent = 0);
	~Preview();

	void setResetBackgroundColor(const QString &resetBackgroundColor) { this->resetBackgroundColor = resetBackgroundColor; }
	void addObjectToParse(Contact contact, QObject *object) { contacts.append(contact); objectsToParse.append(object); }

public slots:
	void syntaxChanged(const QString &content);

signals:
	void needSyntaxFixup(QString &syntax);
	void needFixup(QString &syntax);

};

#endif // PREVIEW_H
