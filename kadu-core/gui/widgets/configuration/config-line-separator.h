/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONFIG_LINE_SEPARATOR_H
#define CONFIG_LINE_SEPARATOR_H

#include <QtGui/QFrame>

#include "configuration/configuration-window-data-manager.h"
#include "gui/widgets/configuration/config-widget.h"

class ConfigGroupBox;

/**
	&lt;line-separator id="id" /&gt;
 **/
class KADUAPI ConfigLineSeparator : public QFrame, public ConfigWidget
{
	Q_OBJECT

protected:
	virtual void createWidgets();

public:
	ConfigLineSeparator(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigLineSeparator() {}

	virtual void loadConfiguration() {};
	virtual void saveConfiguration() {};

};

#endif
