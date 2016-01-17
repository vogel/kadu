/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AutoresponderMessageFilter;
class Configuration;

/**
 * @addtogroup Autoresponder
 * @{
 */

/**
 * @class AutoresponderConfigurator
 * @short Standard configuration setter for AutoresponderMessageFilter.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class listens to changed in global configuration. After change is detected it updated configuration of given
 * AutoresponderMessageFilter with newly created AutoresponderConfiguration instance.
 */
class AutoresponderConfigurator : private QObject, ConfigurationAwareObject
{
	Q_OBJECT
	INJEQT_INSTANCE_IMMEDIATE

public:
	Q_INVOKABLE explicit AutoresponderConfigurator(QObject *parent = nullptr);
	virtual ~AutoresponderConfigurator();

protected:
	virtual void configurationUpdated();

private:
	QPointer<AutoresponderMessageFilter> m_autoresponderMessageFilter;
	QPointer<Configuration> m_configuration;

	void createDefaultConfiguration();

private slots:
	INJEQT_SET void setAutoresponderMessageFilter(AutoresponderMessageFilter *autoresponderMessageFilter);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();

};

/**
 * @}
 */
