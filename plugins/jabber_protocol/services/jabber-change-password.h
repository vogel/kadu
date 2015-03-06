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

#include <QtCore/QObject>
#include <QtCore/QPointer>

class JabberErrorService;
class JabberRegisterExtension;

class QXmppRegisterIq;

class JabberChangePassword : public QObject
{
	Q_OBJECT

public:
	explicit JabberChangePassword(const QString &jid, const QString &newPassword, JabberRegisterExtension *registerExtension, QObject *parent = nullptr);
	virtual ~JabberChangePassword();

	void setErrorService(JabberErrorService *errorService);

signals:
	void error(const QString &error);
	void passwordChanged();

private:
	QPointer<JabberErrorService> m_errorService;

	QString m_id;

private slots:
	void registerIqReceived(const QXmppRegisterIq &registerIq);

};
