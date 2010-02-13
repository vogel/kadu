/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "core/core.h"
#include "debug.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/image-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/hot-key.h"
#include "icons-manager.h"

#include "modules.h"
#include "misc/path-conversion.h"

#include "../history/history.h"

#include "sms-sender.h"
#include "mobile-number-manager.h"
#include "sms-gateway-manager.h"
#include "sms-gateway-query.h"

SmsSender::SmsSender(QObject* parent)
	: QObject(parent), CurrentGateway(0)
{
}

SmsSender::~SmsSender()
{
	kdebugf();
	emit finished(false);
	if (CurrentGateway)
	{
		disconnect(CurrentGateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
		delete CurrentGateway;
	}
	kdebugf2();
}

void SmsSender::onFinished(bool success)
{
	emit finished(success);
}

void SmsSender::send(const QString& number,const QString& message, const QString& contact, const QString& signature, bool autoSelectProvider, QString provider)
{
	kdebugf();
	Number = number;
	Message = message;
	Contact = contact;
	Signature = signature;
	
	if (Number.length() == 12 && Number.left(3) == "+48")
		Number=Number.right(9);
	if (Number.length() != 9)
	{
		MessageDialog::msg(tr("Mobile number is incorrect"), false, "32x32/dialog-warning.png", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}
	if (signature.isEmpty())
	{
		MessageDialog::msg(tr("Signature can't be empty"), false, "32x32/dialog-warning.png", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}

	SmsGateway * gateway = MobileNumberManager::instance()->gateway(number);
	if (gateway)
	{
		CurrentGateway = gateway;
		gatewaySelected();
	}
	else if (autoSelectProvider)
	{
		findGatewayForNumber(Number);
	}
	else
	{
	  	CurrentGateway = SmsGatewayManager::instance()->gateways().take(provider);
		gatewaySelected();
	}
	

	kdebugf2();
}

void SmsSender::findGatewayForNumber(const QString& number)
{
	SmsGatewayQuery *query = new SmsGatewayQuery();
	connect(query, SIGNAL(finished(bool, const QString &)), this, SLOT(gatewayQueryDone(bool, const QString &)));
	query->process(number);
}

void SmsSender::gatewayQueryDone(bool success, const QString &provider)
{
	if (success)
	{
		CurrentGateway = SmsGatewayManager::instance()->gateways().take(provider);
		gatewaySelected();
	}
	else
	{
		MessageDialog::msg(tr("Automatic gateway selection is not available. Please select SMS gateway manually."), false, "32x32/dialog-warning.png", (QWidget*)parent());
		emit finished(false);
		kdebugf2();
		return;
	}
}

void SmsSender::gatewaySelected()
{
	connect(CurrentGateway, SIGNAL(finished(bool)), this, SLOT(onFinished(bool)));
	CurrentGateway->send(Number, Message, Contact, Signature); 
}
