/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "message/message-filter.h"

#include <injeqt/injeqt.h>

class Account;
class AntistringConfiguration;
class AntistringNotificationService;
class Contact;
class HtmlString;
class MessageManager;

class AntistringMessageFilter : public QObject, public MessageFilter
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AntistringMessageFilter(QObject *parent = nullptr);
	virtual ~AntistringMessageFilter();

	virtual bool acceptMessage(const Message &message);

private:
	QPointer<AntistringConfiguration> m_antistringConfiguration;
	QPointer<AntistringNotificationService> m_antistringNotificationService;
	QPointer<MessageManager> m_messageManager;

	int points(const QString &message);
	void writeLog(Contact sender, const QString &message);

private slots:
	INJEQT_SET void setAntistringConfiguration(AntistringConfiguration *antistringConfiguration);
	INJEQT_SET void setAntistringNotificationService(AntistringNotificationService *antistringNotificationService);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);

};
