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

#include <QtCore/QBuffer>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class SmsScriptsManager;

class SmsGatewayQuery : public QObject
{
	Q_OBJECT

public:
	explicit SmsGatewayQuery(QObject *parent = 0);
	virtual ~SmsGatewayQuery();

public slots:
	void process(const QString& number);
	void queryFinished(const QString &provider);

signals:
	void finished(const QString &provider);

private:
	QPointer<SmsScriptsManager> m_smsScriptsManager;

private slots:
	INJEQT_SETTER void setSmsScriptsManager(SmsScriptsManager *smsScriptsManager);

};
