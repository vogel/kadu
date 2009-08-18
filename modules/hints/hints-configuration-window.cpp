/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLabel>
#include <QtGui/QPalette>
#include <QtGui/QSpinBox>

#include "configuration/notifier-configuration-data-manager.h"
#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"

#include "hints-configuration-window.h"

QMap<QString, HintsConfigurationWindow *> HintsConfigurationWindow::ConfigurationWindows;

HintsConfigurationWindow::HintsConfigurationWindow(const QString &eventName, NotifierConfigurationDataManager *dataManager) :
	ConfigurationWindow("hints-notifier-" + eventName, tr("Hints configuration"), dataManager), EventName(eventName)
{
	widget()->appendUiFile(dataPath("kadu/modules/configuration/hints-notifier.ui"));

	widget()->widgetById("syntax")->setToolTip(tr(MainConfigurationWindow::SyntaxTextNotify));
	dynamic_cast<QSpinBox *>(widget()->widgetById("timeout"))->setSpecialValueText(tr("Dont hide"));

	preview = dynamic_cast<QLabel *>(widget()->widgetById("preview"));

	connect(dynamic_cast<ConfigSelectFont *>(widget()->widgetById("font")), SIGNAL(fontChanged(QFont)),
			this, SLOT(fontChanged(QFont)));
	connect(dynamic_cast<ConfigColorButton *>(widget()->widgetById("fgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(foregroundColorChanged(const QColor &)));
	connect(dynamic_cast<ConfigColorButton *>(widget()->widgetById("bgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(backgroundColorChanged(const QColor &)));

	dataManager->configurationWindowCreated(this);

	QFont font;
	font.fromString(dataManager->readEntry("Hints", "_font").toString());
	preview->setFont(font);

	QColor bcolor = dataManager->readEntry("Hints", "_bgcolor").value<QColor>();
	QColor fcolor = dataManager->readEntry("Hints", "_fgcolor").value<QColor>();
	QString style = narg("QWidget {color:%1; background-color:%2}", fcolor.name(), bcolor.name());
	preview->setStyleSheet(style);
}

HintsConfigurationWindow::~HintsConfigurationWindow()
{
	HintsConfigurationWindow::windowDestroyed(EventName);
}

void HintsConfigurationWindow::windowDestroyed(const QString &eventName)
{
	ConfigurationWindows.remove(eventName);
}

HintsConfigurationWindow * HintsConfigurationWindow::configWindowForEvent(const QString &eventName)
{
    	if (ConfigurationWindows[eventName])
		return ConfigurationWindows[eventName];
	else
	{
		NotifierConfigurationDataManager *dataManager = NotifierConfigurationDataManager::dataManagerForEvent(eventName);
		return ConfigurationWindows[eventName] = new HintsConfigurationWindow(eventName, dataManager);
	}
}

void HintsConfigurationWindow::fontChanged(QFont font)
{
	preview->setFont(font);
}

void HintsConfigurationWindow::foregroundColorChanged(const QColor &color)
{
    	QColor bcolor = preview->palette().color(preview->backgroundRole());
	QString style = narg("QWidget {color:%1; background-color:%2}", color.name(), bcolor.name());
	preview->setStyleSheet(style);
}

void HintsConfigurationWindow::backgroundColorChanged(const QColor &color)
{
	QColor fcolor = preview->palette().color(preview->foregroundRole());
	QString style = narg("QWidget {color:%1; background-color:%2}", fcolor.name(), color.name());
	preview->setStyleSheet(style);
}
