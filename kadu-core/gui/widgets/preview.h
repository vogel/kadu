/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PREVIEW_H
#define PREVIEW_H

#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "contacts/contact-set.h"

#include "kadu-web-view.h"

class Preview : public KaduWebView
{
	Q_OBJECT

	Contact contact;
	QObjectList objectsToParse;
	ContactSet contacts;
	QString resetBackgroundColor;

public:
	Preview(QWidget *parent = 0);
	~Preview();

	void setResetBackgroundColor(const QString &resetBackgroundColor) { this->resetBackgroundColor = resetBackgroundColor; }
	void addObjectToParse(Contact contact, QObject *object) { contacts.insert(contact); objectsToParse.append(object); }

	const QObjectList & getObjectsToParse() const { return objectsToParse; }
	const ContactSet & getContactList() const { return contacts; }


public slots:
	void syntaxChanged(const QString &content);

signals:
	void needSyntaxFixup(QString &syntax);
	void needFixup(QString &syntax);

};

#endif // PREVIEW_H
