/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptEngine>

#include "gui/windows/message-dialog.h"
#include "misc/token-reader.h"
#include "debug.h"

#include "scripts/sms-script-manager.h"
#include "sms-gateway-query.h"

#include "sms-internal-sender.h"

SmsInternalSender::SmsInternalSender(const QString &number, const QString &gatewayId, QObject *parent) :
		QObject(parent), GatewayId(gatewayId), Number(number), MyTokenReader(0)
{
	fixNumber();
}

SmsInternalSender::~SmsInternalSender()
{
}

void SmsInternalSender::fixNumber()
{
	if (Number.length() == 12 && Number.left(3) == "+48")
		Number = Number.right(9);
}

bool SmsInternalSender::validateNumber()
{
	return 9 == Number.length();
}

bool SmsInternalSender::validateSignature()
{
	return !Signature.isEmpty();
}

void SmsInternalSender::setContact(const QString &contact)
{
	Contact = contact;
}

void SmsInternalSender::setSignature(const QString &signature)
{
	Signature = signature;
}

void SmsInternalSender::sendMessage(const QString &message)
{
	Message = message;
	
	if (!validateNumber())
	{
		MessageDialog::msg(tr("Mobile number is incorrect"), false, "32x32/dialog-warning.png", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}

	if (!validateSignature())
	{
		MessageDialog::msg(tr("Signature can't be empty"), false, "32x32/dialog-warning.png", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}

	if (GatewayId.isEmpty())
		queryForGateway();
	else
		sendSms();
}

void SmsInternalSender::setTokenReader(TokenReader *tokenReader)
{
	MyTokenReader = tokenReader;
}

void SmsInternalSender::queryForGateway()
{
	SmsGatewayQuery *query = new SmsGatewayQuery(this);
	connect(query, SIGNAL(finished(const QString &)), this, SLOT(gatewayQueryDone(const QString &)));
	query->process(Number);
}

void SmsInternalSender::gatewayQueryDone(const QString &gatewayId)
{
	if (gatewayId.isEmpty())
	{
		MessageDialog::msg(tr("Automatic gateway selection is not available. Please select SMS gateway manually."), false, "32x32/dialog-warning.png", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}

	GatewayId = gatewayId;

	sendSms();
}

void SmsInternalSender::readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod)
{
	if (!MyTokenReader)
	{
		failure("Cannot read token value");
		return;
	}

	TokenCallbackObject = callbackObject;
	TokenCallbackMethod = callbackMethod;

	QNetworkAccessManager *network = new QNetworkAccessManager(this);
	TokenReply = network->get(QNetworkRequest(tokenImageUrl));
	connect(TokenReply, SIGNAL(finished()), this, SLOT(tokenImageDownloaded()));
}

void SmsInternalSender::tokenImageDownloaded()
{
	if (QNetworkReply::NoError != TokenReply->error())
	{
		failure("Cannot download token image");
		return;
	}

	QPixmap image;
	if (!image.loadFromData(TokenReply->readAll()))
	{
		failure("Cannot display token image");
		return;
	}

	MyTokenReader->readTokenAsync(image, this);
}

void SmsInternalSender::tokenRead(const QString &tokenValue)
{
	QScriptValueList arguments;
	arguments.append(tokenValue);
	TokenCallbackMethod.call(TokenCallbackObject, arguments);
}

void SmsInternalSender::sendSms()
{
	QScriptEngine* engine = SmsScriptsManager::instance()->engine();

	QScriptValue jsGatewayManagerObject = engine->evaluate("gatewayManager");
	QScriptValue jsSendSms = jsGatewayManagerObject.property("sendSms");

	QScriptValueList arguments;
	arguments.append(GatewayId);
	arguments.append(Number);
	arguments.append(Contact);
	arguments.append(Signature);
	arguments.append(Message);
	arguments.append(engine->newQObject(this));

	jsSendSms.call(jsGatewayManagerObject, arguments);
}

void SmsInternalSender::result()
{
	emit finished(QString::null);
}

void SmsInternalSender::failure(const QString &errorMessage)
{
	emit finished(errorMessage);
}
