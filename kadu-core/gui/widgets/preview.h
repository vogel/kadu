/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"

#include "kadu-text-browser.h"

class Preview : public KaduTextBrowser
{
	Q_OBJECT

	Buddy buddy;
	QObjectList objectsToParse;
	BuddyList buddies;
	QString resetBackgroundColor;

public:
	Preview(QWidget *parent = 0);
	~Preview();

	void setResetBackgroundColor(const QString &resetBackgroundColor) { this->resetBackgroundColor = resetBackgroundColor; }
	void addObjectToParse(Buddy buddy, QObject *object) { buddies.append(buddy); objectsToParse.append(object); }

	const QObjectList getObjectsToParse() const { return objectsToParse; }
	const BuddyList getContactList() const { return buddies; }


public slots:
	void syntaxChanged(const QString &content);

signals:
	void needSyntaxFixup(QString &syntax);
	void needFixup(QString &syntax);

};

#endif // PREVIEW_H
