/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2011 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002 Dariusz Jagodzik (mast3r@kadu.net)
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

#ifndef SMS_CONFIGURATION_UI_HANDLER_H
#define SMS_CONFIGURATION_UI_HANDLER_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QHttp>

#include "buddies/buddy-manager.h"
#include "chat/chat.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"

#include "sms_exports.h"

class ConfigComboBox;
class ConfigLineEdit;

class QCheckBox;
class QComboBox;
class QListWidget;
class QProcess;
class QTextEdit;

class SMSAPI SmsConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	static SmsConfigurationUiHandler *Instance;

	QCheckBox *useBuiltIn;
	QLineEdit *customApp;
	QCheckBox *useCustomString;
	QLineEdit *customString;
	ConfigComboBox *EraGatewayComboBox;
	ConfigLineEdit *EraSponsoredUser;
	ConfigLineEdit *EraSponsoredPassword;
	ConfigLineEdit *EraOmnixUser;
	ConfigLineEdit *EraOmnixPassword;
	ConfigLineEdit *OrangeMultiboxUser;
	ConfigLineEdit *OrangeMultiboxPassword;

	void createDefaultConfiguration();

	SmsConfigurationUiHandler();
	virtual ~SmsConfigurationUiHandler();

private slots:
	void onSmsBuildInCheckToggle(bool);
	void onEraGatewayChanged(int);

public:
	static void registerConfigurationUi();
	static void unregisterConfigurationUi();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
};

#endif // SMS_CONFIGURATION_UI_HANDLER_H
