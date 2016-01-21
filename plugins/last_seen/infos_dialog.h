/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

typedef QMap<QPair<QString, QString>, QString> LastSeen;

class ChatManager;
class ContactManager;
class MenuInventory;
class StatusTypeManager;

class QTreeWidget;

/*!
 * Creates a QListView within itself. It also updates the "last seen" times.
 * \brief Dialog window for "last seen", IP, DNS.
 */
class InfosDialog : public QDialog
{
	Q_OBJECT

	QPointer<ChatManager> m_chatManager;
	QPointer<ContactManager> m_contactManager;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<StatusTypeManager> m_statusTypeManager;

	const LastSeen &m_lastSeen;

	QTreeWidget *ListView;

private slots:
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_INIT void init();

	void customContextMenuRequested(const QPoint &point);

public:
	/*! Default constructor. */
	explicit InfosDialog(const LastSeen &lastSeen, QWidget *parent = nullptr);

	/*! Default destructor. */
	virtual ~InfosDialog();

};
