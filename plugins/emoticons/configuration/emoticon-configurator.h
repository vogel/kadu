/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef EMOTICON_CONFIGURATOR
#define EMOTICON_CONFIGURATOR

#include <QtCore/QScopedPointer>
#include <QtCore/QPointer>

#include "configuration/emoticon-configuration.h"
#include "theme/emoticon-theme-manager.h"

#include "configuration/configuration-holder.h"

class EmoticonExpanderDomVisitorProvider;
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

	QString LastLoadedThemeName;
	EmoticonTheme LastTheme;
	EmoticonConfiguration Configuration;

	QScopedPointer<EmoticonThemeManager> ThemeManager;
	QPointer<InsertEmoticonAction> InsertAction;
	QPointer<EmoticonExpanderDomVisitorProvider> EmoticonExpanderProvider;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	EmoticonConfigurator();

	/**
	 * @short Sets InsertEmoticonAction to configure.
	 * @author Rafał 'Vogel' Malinowski
	 * @param insertAction InsertEmoticonAction to configured
	 */
	void setInsertAction(InsertEmoticonAction *insertAction);

	/**
	 * @short Sets EmoticonExpanderDomVisitorProvider to configure.
	 * @author Rafał 'Vogel' Malinowski
	 * @param emoticonExpanderProvider EmoticonExpanderDomVisitorProvider to configured
	 */
	void setEmoticonExpanderProvider(EmoticonExpanderDomVisitorProvider *emoticonExpanderProvider);

	/**
	 * @short Update configurations of set objects.
	 * @author Rafał 'Vogel' Malinowski
	 *
	 * Call this method after setInsertAction() and setEmoticonExpanderProvider() to update configuration of
	 * set objects.
	 */
	void configure();

};

/**
 * @}
 */

#endif // EMOTICON_CONFIGURATOR

