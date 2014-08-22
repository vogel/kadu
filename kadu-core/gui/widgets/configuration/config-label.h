/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONFIG_LABEL_H
#define CONFIG_LABEL_H

#include <QtWidgets/QLabel>

#include "configuration/configuration-window-data-manager.h"
#include "gui/widgets/configuration/config-widget.h"

class ConfigGroupBox;

/**
	&lt;label caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigLabel : public QLabel, public ConfigWidget
{
	Q_OBJECT

protected:
	virtual void createWidgets();

public:
	ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigLabel(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigLabel() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

	void setText(const QString &text, bool defaultFormatting = true);

};

#endif
