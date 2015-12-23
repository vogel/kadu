/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "autoaway-status-changer.h"

#include "configuration/configuration-aware-object.h"
#include "misc/memory.h"
#include "plugin/plugin-root-component.h"
#include "status/status-changer.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>

class QLineEdit;
class QSpinBox;
class QCheckBox;

class Idle;

/**
 * @defgroup autoaway Autoaway
 * @{
 */
class Autoaway : public QObject, ConfigurationAwareObject 
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit Autoaway(QObject *pointer = nullptr);
	virtual ~Autoaway();

	AutoawayStatusChanger::ChangeStatusTo changeStatusTo();
	AutoawayStatusChanger::ChangeDescriptionTo changeDescriptionTo();
	QString descriptionAddon() const;

	QString changeDescription(const QString &oldDescription);


private:
	QPointer<AutoawayStatusChanger> m_autoawayStatusChanger;
	QPointer<Idle> m_idle;
	QPointer<PluginRepository> m_pluginRepository;

	owned_qptr<QTimer> m_timer;

	unsigned int m_checkInterval;

	unsigned int m_autoAwayTime;
	unsigned int m_autoExtendedAwayTime;
	unsigned int m_autoDisconnectTime;
	unsigned int m_autoInvisibleTime;

	bool m_autoAwayEnabled;
	bool m_autoExtendedAwayEnabled;
	bool m_autoInvisibleEnabled;
	bool m_autoDisconnectEnabled;
	bool m_parseAutoStatus;

	bool m_statusChanged;

	unsigned int m_idleTime;
	unsigned int m_refreshStatusTime;
	unsigned int m_refreshStatusInterval;

	QString m_autoStatusText;
	QString m_descriptionAddon;

	AutoawayStatusChanger::ChangeDescriptionTo m_changeTo;

	QString parseDescription(const QString &parseDescription);

	void createDefaultConfiguration();

private slots:
	INJEQT_SETTER void setAutoawayStatusChanger(AutoawayStatusChanger *autoawayStatusChanger);
	INJEQT_SETTER void setPluginRepository(PluginRepository *pluginRepository);

	void checkIdleTime();

protected:
	virtual void configurationUpdated();

};

/** @} */
