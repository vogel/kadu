/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <QtCore/QTimer>
#include <QtCore/QObject>

#include "gui/windows/main-configuration-window.h"
#include "status/status-changer.h"

#include "autoaway-status-changer.h"

class QLineEdit;
class QSpinBox;
class QCheckBox;

/**
 * @defgroup autoaway Autoaway
 * @{
 */
class AutoAway : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	AutoAwayStatusChanger *autoAwayStatusChanger;
	QTimer *timer;

	unsigned int checkInterval;

	unsigned int autoAwayTime;
	unsigned int autoExtendedAwayTime;
	unsigned int autoDisconnectTime;
	unsigned int autoInvisibleTime;

	bool autoAwayEnabled;
	bool autoExtendedAwayEnabled;
	bool autoInvisibleEnabled;
	bool autoDisconnectEnabled;
	bool parseAutoStatus;

	bool StatusChanged;

	unsigned int idleTime;
	unsigned int refreshStatusTime;
	unsigned int refreshStatusInterval;

	QSpinBox *autoAwaySpinBox;
	QSpinBox *autoExtendedAwaySpinBox;
	QSpinBox *autoInvisibleSpinBox;
	QSpinBox *autoOfflineSpinBox;
	QSpinBox *autoRefreshSpinBox;

	QLineEdit *descriptionTextLineEdit;

	QCheckBox *parseStatusCheckBox;

	QString autoStatusText;
	QString DescriptionAddon;

	AutoAwayStatusChanger::ChangeDescriptionTo changeTo;

	QString parseDescription(const QString &parseDescription);

	void createDefaultConfiguration();

private slots:
	void checkIdleTime();

	void autoAwaySpinBoxValueChanged(int value);
	void autoExtendedAwaySpinBoxValueChanged(int value);
	void autoInvisibleSpinBoxValueChanged(int value);
	void autoOfflineSpinBoxValueChanged(int value);

	void descriptionChangeChanged(int index);

protected:
	virtual void configurationUpdated();

public:
	AutoAway();
	virtual ~AutoAway();

	AutoAwayStatusChanger::ChangeStatusTo changeStatusTo();
	AutoAwayStatusChanger::ChangeDescriptionTo changeDescriptionTo();
	QString descriptionAddon() const;

	QString changeDescription(const QString &oldDescription);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

/** @} */

#endif
