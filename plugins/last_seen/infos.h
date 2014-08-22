/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Dariusz Markowicz (darom@alari.pl)
 * Copyright 2008, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef INFOS_H
#define INFOS_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "contacts/contact.h"
#include "status/status.h"

//! A "dictionary oriented" list type, holding uins and "Last seen" times.
typedef QMap<QPair<QString, QString>, QString> LastSeen;

class ActionDescription;

/*!
 * This class handles the "Last seen" time for InfosDialog class.
 * \brief "Last seen, IP, DNS" main class.
 */
class Infos : public QObject, AccountsAwareObject
{
	Q_OBJECT

private slots:
	void contactStatusChanged(Contact contact, Status status);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	//! updates Last Seen times in lastSeen.
	void updateTimes();

	/*!
	 * It holds the data file name. It's set to <i>KaduPaths::instance()->profilePath() + QLatin1String("last_seen.data")</i>
	 * in the constructor.
	 */
	QString fileName;

	/*!
	 * List with "last seen on" time.
	 * The keys are uins,
	 * the values are "last seen" times.
	 */
	LastSeen lastSeen;

	//! Holds the "&Show infos about contacts" menu entry ID.
	int menuID;

	ActionDescription* lastSeenActionDescription;

public:
	/*!
	 * \brief Default contructor.
	 * \param parent - the parent object,
	 * \param name - the object's name
	 */
	explicit Infos(QObject *parent = 0);

	//! Default destructor
	~Infos();

public slots:
	//! This slot open InfosDialog dialog.
	void onShowInfos();
};

#endif // INFOS_H
