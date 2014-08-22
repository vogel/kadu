/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONFIG_LIST_WIDGET_H
#define CONFIG_LIST_WIDGET_H

#include <QtGui/QListWidget>

#include "gui/widgets/configuration/config-widget-value.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;
class ConfigurationWindowDataManager;

/**
	&lt;list-box caption="caption" id="id"&gt;
		&lt;item value="value" caption="caption"&gt;
		...
	&lt;/list-box&gt;

	@arg value - wartość zapisana do pliku konfiguracyjnego
	@arg caption - wartość wyświetlana
 **/
class KADUAPI ConfigListWidget : public QListWidget, public ConfigWidgetValue
{
	Q_OBJECT

	QLabel *label;

	QStringList itemValues;
	QStringList itemCaptions;

protected:
	virtual void createWidgets();

public:
	ConfigListWidget(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigListWidget(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigListWidget();

	virtual void setVisible(bool visible);

	void setItems(const QStringList &itemValues, const QStringList &itemCaptions);
	void setIcons(const QList<QIcon> &icons);
	void setCurrentItem(const QString &currentItem);
	QString currentItemValue() const { return currentRow() != -1 ? itemValues.at(currentRow()) : QString(); }

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual bool fromDomElement(QDomElement domElement);

};

#endif
