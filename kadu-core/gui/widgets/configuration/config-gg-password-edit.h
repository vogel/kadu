/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_GG_PASSWORD_EDIT_H
#define CONFIG_GG_PASSWORD_EDIT_H

#include "gui/widgets/configuration/config-line-edit.h"
#include "configuration/configuration-window-data-manager.h"
#include "misc/misc.h"

class ConfigGroupBox;

/**
	&lt;gg-password-edit caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigGGPasswordEdit : public ConfigLineEdit
{
public:
	ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigGGPasswordEdit() {}

	virtual void loadConfiguration();
	virtual void saveConfiguration();
};

#endif
