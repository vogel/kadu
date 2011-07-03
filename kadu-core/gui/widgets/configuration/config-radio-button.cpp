/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
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

#include "gui/widgets/configuration/config-radio-button.h"
#include "gui/widgets/configuration/config-group-box.h"

#include "debug.h"

ConfigRadioButton::ConfigRadioButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager)
	: QRadioButton(widgetCaption, parentConfigGroupBox->widget()), ConfigWidgetValue(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager)
{
	createWidgets();
}

ConfigRadioButton::ConfigRadioButton(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager) :
		QRadioButton(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox, dataManager)
{
}

void ConfigRadioButton::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption.toUtf8().constData()));
	parentConfigGroupBox->addWidget(this, true);

	if (!ConfigWidget::toolTip.isEmpty())
		setToolTip(qApp->translate("@default", ConfigWidget::toolTip.toUtf8().constData()));
}

void ConfigRadioButton::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setChecked(dataManager->readEntry(section, item).toBool());
	emit toggled(isChecked());
}

void ConfigRadioButton::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(isChecked() ? "true" : "false"));
}

void ConfigRadioButton::show()
{
	QRadioButton::show();
}

void ConfigRadioButton::hide()
{
	QRadioButton::hide();
}
