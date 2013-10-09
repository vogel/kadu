/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#ifndef CONFIG_SPIN_BOX_H
#define CONFIG_SPIN_BOX_H

#include <QtGui/QSpinBox>

#include "configuration/configuration-window-data-manager.h"
#include "gui/widgets/configuration/config-widget-value.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;

/**
	&lt;spin-box caption="caption" id="id" min-value="minValue" max-value="maxValue" step="step" suffix="suffix" special-value="specialValue" /&gt;

	@arg minValue - minimalna wartość (obowiązkowo)
	@arg maxValue - maksymalna wartość (obowiązkowo)
	@arg step - krok wartości (nieobowiazkowo, domyślnie 1)
	@arg suffix - przyrostek za wartością, np. jednostka
	@arg specialValue - wyświetlana nazwa specjalnej wartości spinboxa, równej minValue (nieobowiazkowo)
 **/
class KADUAPI ConfigSpinBox : public QSpinBox, public ConfigWidgetValue
{
	Q_OBJECT

	QLabel *label;

	QString Suffix;

private slots:
	void onValueChanged(int i);

protected:
	virtual void createWidgets();

public:
	ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, const QString &specialValue, int minValue, int maxValue, int step, QString suffix, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigSpinBox();

	virtual void setVisible(bool visible);

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual bool fromDomElement(QDomElement domElement);

};

#endif
