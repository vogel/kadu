/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jid.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <qxmpp/QXmppClient.h>
#include <memory>

class JabberErrorService;
class JabberRegisterExtension;

class QXmppRegisterIq;

class JabberRegisterAccount : public QObject
{
	Q_OBJECT

public:
	explicit JabberRegisterAccount(Jid jid, QString password, QString email, QObject *parent = nullptr);
	virtual ~JabberRegisterAccount();

	void setErrorService(JabberErrorService *errorService);

	void start();

	Jid jid() const;

signals:
	void statusMessage(const QString &statusMessage);
	void success();
	void error(const QString &error);

private:
	enum class State
	{
		None,
		Connecting,
		WaitForRegistrationForm,
		WaitForRegistrationConfirmation
	};

	QPointer<JabberErrorService> m_errorService;

	QPointer<QXmppClient> m_client;
	std::unique_ptr<JabberRegisterExtension> m_registerExtension;

	Jid m_jid;
	QString m_password;
	QString m_email;

	State m_state;
	QString m_id;

	void handleSuccess();
	void handleError(const QString &errorMessage);

private slots:
	void clientError(QXmppClient::Error error);
	void askForRegistration();
	void registerIqReceived(const QXmppRegisterIq &registerIq);
	void handleRegistrationForm(const QXmppRegisterIq &registerIq);
	void sendFilledRegistrationForm();
	void handleRegistrationConfirmation(const QXmppRegisterIq &registerIq);

};
