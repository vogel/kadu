/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>

#include "gui/widgets/configuration/config-gg-password-edit.h"
#include "gui/widgets/configuration/config-group-box.h"

ConfigGGPasswordEdit::ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ConfigLineEdit(section, item, widgetCaption, toolTip, parentConfigGroupBox, dataManager, name)
{
	setEchoMode(QLineEdit::Password);
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager, char *name)
	: ConfigLineEdit(parentConfigGroupBox, dataManager, name)
{
	setEchoMode(QLineEdit::Password);
}

void ConfigGGPasswordEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setText(pwHash(dataManager->readEntry(section, item).toString()));
}

void ConfigGGPasswordEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	dataManager->writeEntry(section, item, QVariant(pwHash(text())));
}
