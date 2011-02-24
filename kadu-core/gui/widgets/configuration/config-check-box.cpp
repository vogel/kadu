/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>

#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/config-group-box.h"

#include "debug.h"

ConfigCheckBox::ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QCheckBox(widgetCaption, parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigCheckBox::ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager) :
		QCheckBox(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager)
{
}

void ConfigCheckBox::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption.toAscii().data()));
	parentConfigGroupBox->addWidget(this, true);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toAscii().data()));
}

void ConfigCheckBox::loadConfiguration()
{
	if (!dataManager)
		return;
	setChecked(dataManager->readEntry(section, item).toBool());
	emit toggled(isChecked());
}

void ConfigCheckBox::saveConfiguration()
{
	if (!dataManager)
		return;
	dataManager->writeEntry(section, item, QVariant(isChecked() ? "true" : "false"));
}

void ConfigCheckBox::show()
{
	QCheckBox::show();
}

void ConfigCheckBox::hide()
{
	QCheckBox::hide();
}
