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
#include <QtScript/QScriptEngine>

#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "core/core.h"
#include "debug.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/hot-key.h"
#include "icons/icons-manager.h"

#include "misc/path-conversion.h"

#include "scripts/sms-script-manager.h"

#include "sms-gateway-query.h"

SmsGatewayQuery::SmsGatewayQuery(QObject *parent) :
		QObject(parent)
{
}

SmsGatewayQuery::~SmsGatewayQuery()
{
}

void SmsGatewayQuery::queryFinished(const QString &provider)
{
	emit finished(provider);

	deleteLater();
}

void SmsGatewayQuery::process(const QString &number)
{
	QScriptEngine* engine = SmsScriptsManager::instance()->engine();
	QScriptValue jsGatewayQueryObject = engine->evaluate("new GatewayQuery()");
	QScriptValue jsGetGateway = jsGatewayQueryObject.property("getGateway");

	QScriptValueList arguments;
	arguments.append(number);
	arguments.append(engine->newQObject(this));

	jsGetGateway.call(jsGatewayQueryObject, arguments);
}

