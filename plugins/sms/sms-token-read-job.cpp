/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "gui/windows/message-dialog.h"
#include "gui/windows/token-window.h"

#include "sms-token-read-job.h"

SmsTokenReadJob::SmsTokenReadJob(QScriptValue callbackObject, QScriptValue callbackMethod, QObject *parent) :
		QObject(parent), CallbackObject(callbackObject), CallbackMethod(callbackMethod)
{
}

SmsTokenReadJob::~SmsTokenReadJob()
{
}

void SmsTokenReadJob::exec(const QString &tokenImageUrl)
{
	QNetworkAccessManager *network = new QNetworkAccessManager(this);
	TokenNetworkReply = network->get(QNetworkRequest(tokenImageUrl));

	connect(TokenNetworkReply, SIGNAL(finished()), this, SLOT(tokenImageDownloaded()));
}

void SmsTokenReadJob::tokenImageDownloaded()
{
	if (QNetworkReply::NoError != TokenNetworkReply->error())
	{
		MessageDialog::exec(KaduIcon("dialog-error"), tr("SMS"), tr("Unable to fetch required token"));
		tokenValueEntered(QString());
		return;
	}

	QPixmap tokenPixmap;
	if (!tokenPixmap.loadFromData(TokenNetworkReply->readAll()))
	{
		tokenValueEntered(QString());
		return;
	}

	TokenWindow *tokenWindow = new TokenWindow(tokenPixmap, 0);
	connect(tokenWindow, SIGNAL(tokenValueEntered(QString)), this, SLOT(tokenValueEntered(QString)));
	tokenWindow->exec();
}

void SmsTokenReadJob::tokenValueEntered(const QString &tokenValue)
{
	QScriptValueList arguments;
	arguments.append(tokenValue);
	CallbackMethod.call(CallbackObject, arguments);

	deleteLater();
}
