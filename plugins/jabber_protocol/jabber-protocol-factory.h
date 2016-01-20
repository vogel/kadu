/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "protocols/protocol-factory.h"
#include "status/status-adapter.h"

#include <QtCore/QPointer>
#include <memory>
#include <injeqt/injeqt.h>

class FacebookDepreceatedMessage;
class JabberProtocolMenuManager;

class JabberProtocolFactory : public ProtocolFactory
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit JabberProtocolFactory(QObject *parent = nullptr);
	virtual ~JabberProtocolFactory();

	virtual Protocol * createProtocolHandler(Account account);
	virtual AccountDetails * createAccountDetails(AccountShared *accountShared);
	virtual AccountAddWidget * newAddAccountWidget(bool showButtons, QWidget *parent);
	virtual AccountCreateWidget * newCreateAccountWidget(bool showButtons, QWidget *parent);
	virtual AccountEditWidget* newEditAccountWidget(Account, QWidget*);
	virtual QWidget * newContactPersonalInfoWidget(Contact contact, QWidget *parent = nullptr);
    virtual ProtocolMenuManager * protocolMenuManager();
	virtual QList<StatusType> supportedStatusTypes();
	virtual StatusAdapter * statusAdapter() { return m_statusAdapter.get(); }
	virtual QString idLabel();
	virtual QValidator::State validateId(QString id);
	virtual bool canRegister();
	virtual bool allowChangeServer();
	virtual QString defaultServer();
	virtual QString whatIsMyUsername() { return QString(); }

	virtual QString name() { return "jabber"; }
	virtual QString displayName() { return "Jabber/XMPP"; }

	virtual KaduIcon icon();

private:
	QPointer<FacebookDepreceatedMessage> m_facebookDepreceatedMessage;
	QPointer<JabberProtocolMenuManager> m_jabberProtocolMenuManager;

	QList<StatusType> m_supportedStatusTypes;

	std::unique_ptr<StatusAdapter> m_statusAdapter;

private slots:
	INJEQT_SET void setFacebookDepreceatedMessage(FacebookDepreceatedMessage *facebookDepreceatedMessage);
	INJEQT_SET void setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager);

};
