/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPair>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <injeqt/injeqt.h>

class SmsGateway;
class SmsScriptsManager;

class SmsGatewayManager : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SmsGatewayManager(QObject *parent = nullptr);
	virtual ~SmsGatewayManager();

	void load();

	const QList<SmsGateway> & items() const { return m_items; }

	SmsGateway byId(const QString &id) const;

private:
	QPointer<SmsScriptsManager> m_smsScriptsManager;

	QList<SmsGateway> m_items;

private slots:
	INJEQT_SET void setSmsScriptsManager(SmsScriptsManager *smsScriptsManager);

};
