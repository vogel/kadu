/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-holder.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

enum SetStatusMode
{
	SetStatusPerAccount,
	SetStatusPerIdentity,
	SetStatusForAll
};

class Configuration;

class StatusConfigurationHolder : public ConfigurationHolder
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit StatusConfigurationHolder(QObject *parent = nullptr);
	virtual ~StatusConfigurationHolder();

	void configurationUpdated();

	SetStatusMode setStatusMode() const { return SetStatus; }
	bool isSetStatusPerAccount() const { return SetStatus == SetStatusPerAccount; }
	bool isSetStatusPerIdentity() const { return SetStatus == SetStatusPerIdentity; }

signals:
	void setStatusModeChanged();

private:
	QPointer<Configuration> m_configuration;

	SetStatusMode SetStatus;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();

};
