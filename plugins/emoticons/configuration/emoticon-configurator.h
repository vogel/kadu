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

#include "configuration/emoticon-configuration.h"

#include "configuration/configuration-holder.h"
#include "theme/emoticon-theme-manager.h"

#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>
#include <injeqt/injeqt.h>

class Configuration;
class EmoticonExpanderDomVisitorProvider;
class PluginInjectedFactory;
class InsertEmoticonAction;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonConfigurator
 * @short Standard configuration setter for emoticon plugin.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class listens to changed in global configuration. After change is detected it updated configuration of given
 * InsertEmoticonAction and EmoticonExpanderDomVisitorProvider with updated EmoticonConfiguration instance.
 */
class EmoticonConfigurator : public ConfigurationHolder
{
	Q_OBJECT

	QPointer<Configuration> m_configuration;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

	QString LastLoadedThemeName;
	EmoticonTheme LastTheme;
	EmoticonConfiguration m_emoticonConfiguration;

	QScopedPointer<EmoticonThemeManager> ThemeManager;
	QPointer<InsertEmoticonAction> InsertAction;
	QPointer<EmoticonExpanderDomVisitorProvider> EmoticonExpanderProvider;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	Q_INVOKABLE explicit EmoticonConfigurator(QObject *parent = nullptr);
	virtual ~EmoticonConfigurator();

	/**
	 * @short Update configurations of set objects.
	 * @author Rafał 'Vogel' Malinowski
	 *
	 * Call this method after setInsertAction() and setEmoticonExpanderProvider() to update configuration of
	 * set objects.
	 */
	void configure();

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setInsertAction(InsertEmoticonAction *insertAction);
	INJEQT_SET void setEmoticonExpanderProvider(EmoticonExpanderDomVisitorProvider *emoticonExpanderProvider);
	INJEQT_INIT void init();

};

/**
 * @}
 */
