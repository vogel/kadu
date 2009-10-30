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
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "debug.h"
#include "gui/widgets/contacts-list-view.h"
#include "gui/widgets/contacts-list-widget.h"
#include "gui/widgets/contacts-list-view-menu-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/image-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-box.h"
#include "gui/hot-key.h"
#include "icons-manager.h"

#include "modules.h"
#include "misc/path-conversion.h"

#include "sms-gateway-query.h"


SmsGatewayQuery::~SmsGatewayQuery()
{
	kdebugf();
	emit finished(false, "");
	if (Query)
	{
		disconnect(Query, SIGNAL(done(bool)), this, SLOT(queryFinished(bool)));
		delete Query;
	}
	delete QueryBuffer;
	kdebugf2();
}

void SmsGatewayQuery::queryFinished(bool error)
{
	QString data = QueryBuffer->data();
	if(!error)
	{
		QString provider;
		
		if(data.contains("260 01", Qt::CaseInsensitive))
			provider = "plus";
		else if(data.contains("260 02", Qt::CaseInsensitive))
			provider = "era";
		else if(data.contains("260 03", Qt::CaseInsensitive))
			provider = "orange";
		else if(data.contains("260 06", Qt::CaseInsensitive))
			provider = "play";
		
		emit finished(true, provider);
	}
	else
	{
		emit finished(false, "");
	}
}

void SmsGatewayQuery::process(const QString& number)
{
	kdebugf();
	
	Query = new QHttp("is.eranet.pl", 80, this);
	QueryBuffer = new QBuffer(this);
	connect(Query, SIGNAL(done(bool)), this, SLOT(queryFinished(bool)));
	Query->get("/updir/check.cgi?t=48" + number, QueryBuffer);
	
	kdebugf2();
}

