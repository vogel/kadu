/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_SLIDER_H
#define CONFIG_SLIDER_H

#include <QtGui/QSlider>

#include "configuration/configuration-window-data-manager.h"
#include "gui/widgets/configuration/config-widget-value.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;

/**
	&lt;slider caption="caption" id="id" /&gt;

	@arg minValue - minimalna wartość (obowiązkowo)
	@arg maxValue - maksymalna wartość (obowiązkowo)
	@arg pageStep - krok wartości (obowiązkowo)
 **/
class KADUAPI ConfigSlider : public QSlider, public ConfigWidgetValue
{
	Q_OBJECT

	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSlider(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigSlider(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigSlider();

	virtual void setVisible(bool visible);

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual bool fromDomElement(QDomElement domElement);

};

#endif
