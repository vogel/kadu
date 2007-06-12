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

#include <qvaluelist.h>

#include "configuration_window.h"

class ConfigComboBox;
class ConfigLineEdit;

class ChatMessage;
class Preview;

class QCheckBox;

class MainConfigurationWindow;

class ConfigurationAwareObject : public QObject
{
	Q_OBJECT

public slots:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) = 0;
};

class MainConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	static MainConfigurationWindow *Instance;
	static QValueList<ConfigurationAwareObject *> ConfigurationAwareObjects;
	static QStringList UiFiles;

	static void instanceCreated();

	ConfigurationWindow *lookChatAdvanced;

	QCheckBox *onStartupSetLastDescription;

	ConfigComboBox *emoticonsStyleComboBox;
	ConfigComboBox *emoticonsThemeComboBox;

	ConfigComboBox *browserComboBox;
	ConfigLineEdit *browserCommandLineEdit;

	ConfigComboBox *mailComboBox;
	ConfigLineEdit *mailCommandLineEdit;

	Preview *chatPreview;
	QValueList<ChatMessage *> chatMessages;

	void setLanguages();
	void setIconThemes();
	void setEmoticonThemes();
	void setQtThemes();
	void setToolTipClasses();

	QString findExecutable(const QStringList &paths, const QStringList &executableNames);
	// TODO: remove append parameter
	void prepareChatPreview(Preview *preview, bool append = false);

	void import_0_5_0_configuration();

private slots:
	void onChangeEmoticonsStyle(int index);

	void onChangeStartupStatus(int index);
	void onChangeBrowser(int index);
	void onChangeMail(int index);

	void showLookChatAdvanced();
	void lookChatAdvancedDestroyed();

	void onChatSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow);
	void chatSyntaxFixup(QString &syntax);
	void chatFixup(Preview *preview);

public:
	static MainConfigurationWindow * instance()
	{
		if (!Instance)
		{
			Instance = new MainConfigurationWindow();
			instanceCreated();
		}

		return Instance;
	}
	static void registerConfigurationAwareObject(ConfigurationAwareObject *configurationAwareObject);
	static void unregisterConfigurationAwareObject(ConfigurationAwareObject *configurationAwareObject);
	static void registerUiFile(const QString &uiFile);
	static void unregisterUiFile(const QString &uiFile);

	MainConfigurationWindow();
	virtual ~MainConfigurationWindow();

	virtual void show();
};

#endif // MAIN_CONFIGURATION_WINDOW_H
