/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "url-handlers/url-handler.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QRegExp>
#include <injeqt/injeqt.h>

class AccountManager;
class ChatWidgetManager;
class ContactManager;

class QAction;

class GaduUrlHandler : public QObject, public UrlHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit GaduUrlHandler(QObject *parent = nullptr);
	virtual ~GaduUrlHandler();

	virtual bool isUrlValid(const QByteArray &url);
	virtual void openUrl(const QByteArray &url, bool disableMenu = false);

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ContactManager> m_contactManager;
	QRegExp m_gaduRegExp;

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);

	void accountSelected(QAction *action);

};
