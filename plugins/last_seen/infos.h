/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "contacts/contact.h"
#include "status/status.h"

//! A "dictionary oriented" list type, holding uins and "Last seen" times.
typedef QMap<QPair<QString, QString>, QString> LastSeen;

class AccountManager;
class ActionDescription;
class Actions;
class ContactManager;
class InjectedFactory;
class MenuInventory;
class PathsProvider;

/*!
 * This class handles the "Last seen" time for InfosDialog class.
 * \brief "Last seen, IP, DNS" main class.
 */
class Infos : public QObject, AccountsAwareObject
{
	Q_OBJECT

	QPointer<AccountManager> m_accountManager;
	QPointer<Actions> m_actions;
	QPointer<ContactManager> m_contactManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void contactStatusChanged(Contact contact, Status status);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

	//! updates Last Seen times in lastSeen.
	void updateTimes();

	/*!
	 * It holds the data file name. It's set to <i>profilePath + QLatin1String("last_seen.data")</i>
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
	Q_INVOKABLE explicit Infos(QObject *parent = nullptr);

	//! Default destructor
	virtual ~Infos();

public slots:
	//! This slot open InfosDialog dialog.
	void onShowInfos();

};
