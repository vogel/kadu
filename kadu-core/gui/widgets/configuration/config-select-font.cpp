/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-select-font.h"

#include "debug.h"

ConfigSelectFont::ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager), label(0)
{
	createWidgets();
}

ConfigSelectFont::ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager), label(0)
{
}

ConfigSelectFont::~ConfigSelectFont()
{
	if (label)
		delete label;
}

void ConfigSelectFont::createWidgets()
{
	kdebugf();

	label = new QLabel(QCoreApplication::translate("@default", widgetCaption.toUtf8().constData()) + ':', parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
		label->setToolTip(QCoreApplication::translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
	}
}

void ConfigSelectFont::loadConfiguration()
{
	if (!dataManager)
		return;
	QFont font;
	if (font.fromString(dataManager->readEntry(section, item).toString()))
		setFont(font);
}

void ConfigSelectFont::saveConfiguration()
{
	if (!dataManager)
		return;
	dataManager->writeEntry(section, item, QVariant(font().toString()));
}

void ConfigSelectFont::setVisible(bool visible)
{
	label->setVisible(visible);
	SelectFont::setVisible(visible);
}

#include "moc_config-select-font.cpp"
