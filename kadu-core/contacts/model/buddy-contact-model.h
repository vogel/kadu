/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_CONTACT_MODEL_H
#define BUDDY_CONTACT_MODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

#include "buddies/buddy.h"

class Account;
class Contact;

class BuddyContactModel : public QAbstractListModel
{
	Q_OBJECT

	Buddy SourceBuddy;
	bool IncludeIdentityInDisplay;

private slots:
	void contactAboutToBeAdded(Contact data);
	void contactAdded(Contact data);
	void contactAboutToBeRemoved(Contact data);
	void contactRemoved(Contact data);

public:
	explicit BuddyContactModel(Buddy contact, QObject *parent = 0);
	virtual ~BuddyContactModel();

	virtual int columnCount(const QModelIndex &parent) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex &index, int role) const;

	Contact contact(const QModelIndex &index) const;
	int contactIndex(Contact data);
	QModelIndex contactModelIndex(Contact data);

	void setIncludeIdentityInDisplay(bool includeIdentityInDisplay);

};

#endif // BUDDY_CONTACT_MODEL_H
