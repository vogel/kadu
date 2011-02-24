/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_INFO_PANEL_H
#define BUDDY_INFO_PANEL_H

#include "buddies/buddy-or-contact.h"
#include "configuration/configuration-aware-object.h"
#include "contacts/contact.h"

#include "kadu-web-view.h"

class BuddyInfoPanel : public KaduWebView, private ConfigurationAwareObject
{
	Q_OBJECT

	BuddyOrContact Item;
	QString Template;
	QString Syntax;
	QString BackgroundColor;

	void connectItem();
	void disconnectItem();

private slots:
	void buddyUpdated(Buddy &buddy);

protected:
	virtual void configurationUpdated();

public:
	explicit BuddyInfoPanel(QWidget *parent = 0);
	virtual ~BuddyInfoPanel();

	virtual void setVisible(bool visible);

public slots:
	void displayItem(BuddyOrContact item);
	void update();
	void styleFixup(QString &syntax);

};

#endif // BUDDY_INFO_PANEL_H
