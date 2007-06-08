/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MAIN_CONFIGURATION_WINDOW_H
#define MAIN_CONFIGURATION_WINDOW_H

#include "configuration_window.h"

class ConfigComboBox;
class ConfigLineEdit;

class QCheckBox;

class MainConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	ConfigurationWindow *lookChatAdvanced;

	QCheckBox *onStartupSetLastDescription;

	ConfigComboBox *emoticonsStyleComboBox;
	ConfigComboBox *emoticonsThemeComboBox;

	ConfigComboBox *browserComboBox;
	ConfigLineEdit *browserCommandLineEdit;

	ConfigComboBox *mailComboBox;
	ConfigLineEdit *mailCommandLineEdit;

	void setLanguages();
	void setIconThemes();
	void setEmoticonThemes();
	void setQtThemes();
	void setToolTipClasses();

	QString findExecutable(const QStringList &paths, const QStringList &executableNames);

	void import_0_5_0_configuration();

private slots:
	void onChangeEmoticonsStyle(int index);

	void onChangeStartupStatus(int index);
	void onChangeBrowser(int index);
	void onChangeMail(int index);

	void showLookChatAdvanced();
	void lookChatAdvancedDestroyed();

public:
	MainConfigurationWindow();
	virtual ~MainConfigurationWindow();

	virtual void show();
};

#endif // MAIN_CONFIGURATION_WINDOW_H
