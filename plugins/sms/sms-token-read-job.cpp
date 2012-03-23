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

	emit progress("dialog-information", tr("Downloading token image..."));
}

void SmsTokenReadJob::tokenImageDownloaded()
{
	if (QNetworkReply::NoError != TokenNetworkReply->error())
	{
		emit finished(false, "dialog-error", tr("Unable to fetch token image."));
		tokenValueEntered(QString());
		return;
	}

	QPixmap tokenPixmap;
	if (!tokenPixmap.loadFromData(TokenNetworkReply->readAll()))
	{
		emit finished(false, "dialog-error", tr("Unable to read token image."));
		tokenValueEntered(QString());
		return;
	}

	emit progress("dialog-information", tr("Waiting for entry of token value..."));

	TokenWindow *tokenWindow = new TokenWindow(tokenPixmap, 0);
	connect(tokenWindow, SIGNAL(tokenValueEntered(QString)), this, SLOT(tokenValueEntered(QString)));
	tokenWindow->exec();
}

void SmsTokenReadJob::tokenValueEntered(const QString &tokenValue)
{
	if (tokenValue.isEmpty())
	{
		emit finished(false, "dialog-error", tr("No token value provided."));
		return;
	}

	emit progress("dialog-information", tr("Received token value."));

	QScriptValueList arguments;
	arguments.append(tokenValue);
	CallbackMethod.call(CallbackObject, arguments);

	deleteLater();
}
