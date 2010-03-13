/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptEngine>

#include "gui/windows/message-dialog.h"
#include "misc/token-reader.h"
#include "debug.h"

#include "scripts/sms-script-manager.h"
#include "sms-gateway-query.h"

#include "sms-sender.h"

SmsSender::SmsSender(const QString &number, const QString &gatewayId, QObject *parent) :
		QObject(parent), GatewayId(gatewayId), Number(number), MyTokenReader(0)
{
	fixNumber();
}

SmsSender::~SmsSender()
{
}

void SmsSender::fixNumber()
{
	if (Number.length() == 12 && Number.left(3) == "+48")
		Number = Number.right(9);
}

bool SmsSender::validateNumber()
{
	return 9 == Number.length();
}

bool SmsSender::validateSignature()
{
	return !Signature.isEmpty();
}

void SmsSender::setContact(const QString &contact)
{
	Contact = contact;
}

void SmsSender::setSignature(const QString &signature)
{
	Signature = signature;
}

void SmsSender::sendMessage(const QString &message)
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

void SmsSender::setTokenReader(TokenReader *tokenReader)
{
	MyTokenReader = tokenReader;
}

void SmsSender::queryForGateway()
{
	SmsGatewayQuery *query = new SmsGatewayQuery(this);
	connect(query, SIGNAL(finished(const QString &)), this, SLOT(gatewayQueryDone(const QString &)));
	query->process(Number);
}

void SmsSender::gatewayQueryDone(const QString &gatewayId)
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

void SmsSender::readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod)
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

void SmsSender::tokenImageDownloaded()
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

void SmsSender::tokenRead(const QString &tokenValue)
{
	QScriptValueList arguments;
	arguments.append(tokenValue);
	TokenCallbackMethod.call(TokenCallbackObject, arguments);
}

void SmsSender::sendSms()
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

void SmsSender::result()
{
	emit finished(QString::null);
}

void SmsSender::failure(const QString &errorMessage)
{
	emit finished(errorMessage);
}
