/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qstyle.h>
#include <qstylefactory.h>

#include "chat_message.h"
#include "config_file.h"
#include "configuration_window_widgets.h"
#include "debug.h"
#include "emoticons.h"
#include "icons_manager.h"
#include "kadu.h"
#include "modules.h"

#include "main_configuration_window.h"

MainConfigurationWindow *MainConfigurationWindow::Instance = 0;
QValueList<QPair<QString, ConfigurationUiHandler *> > MainConfigurationWindow::UiFiles;

void MainConfigurationWindow::registerUiFile(const QString &uiFile, ConfigurationUiHandler *uiHandler)
{
	UiFiles.append(qMakePair(uiFile, uiHandler));
	if (Instance)
	{
		QValueList<ConfigWidget *> widgets = Instance->appendUiFile(uiFile);

		if (uiHandler)
			uiHandler->mainConfigurationWindowCreated(Instance);

		// allow uiHandler handle this...
		// TODO: make it pretty
		FOREACH(widget, widgets)
			if (*widget)
				(*widget)->loadConfiguration();
	}
}

void MainConfigurationWindow::unregisterUiFile(const QString &uiFile, ConfigurationUiHandler *uiHandler)
{
	UiFiles.remove(qMakePair(uiFile, uiHandler));
	if (Instance)
		Instance->removeUiFile(uiFile);
}

void MainConfigurationWindow::instanceCreated()
{
	FOREACH(configurationUiHandlerPair, UiFiles)
	{
		ConfigurationUiHandler *uiHandler = (*configurationUiHandlerPair).second;
		Instance->appendUiFile((*configurationUiHandlerPair).first);
		if (uiHandler)
			uiHandler->mainConfigurationWindowCreated(Instance);
	}
}

MainConfigurationWindow::MainConfigurationWindow()
	: ConfigurationWindow(), lookChatAdvanced(0)
{
	appendUiFile(dataPath("kadu/configuration/dialog.ui"));

	onStartupSetLastDescription = dynamic_cast<QCheckBox *>(widgetById("onStartupSetLastDescription"));
	QLineEdit *disconnectDescription = dynamic_cast<QLineEdit *>(widgetById("disconnectDescription"));
	QLineEdit *onStartupSetDescription = dynamic_cast<QLineEdit *>(widgetById("onStartupSetDescription"));
	disconnectDescription->setMaxLength(GG_STATUS_DESCR_MAXSIZE);
	onStartupSetDescription->setMaxLength(GG_STATUS_DESCR_MAXSIZE);

	connect(widgetById("disconnectWithCurrentDescription"), SIGNAL(toggled(bool)), disconnectDescription, SLOT(setDisabled(bool)));
	connect(onStartupSetLastDescription, SIGNAL(toggled(bool)), onStartupSetDescription, SLOT(setDisabled(bool)));
	connect(widgetById("receiveImages"), SIGNAL(toggled(bool)), widgetById("receiveImagesDuringInvisibility"), SLOT(setEnabled(bool)));
	connect(widgetById("receiveImages"), SIGNAL(toggled(bool)), widgetById("maxImageRequests"), SLOT(setEnabled(bool)));
	connect(widgetById("startupStatus"), SIGNAL(activated(int)), this, SLOT(onChangeStartupStatus(int)));
	connect(widgetById("showDescription"), SIGNAL(toggled(bool)), widgetById("multilineDescription"), SLOT(setEnabled(bool)));
	connect(widgetById("useDefaultServers"), SIGNAL(toggled(bool)), widgetById("serverList"), SLOT(setDisabled(bool)));

	emoticonsStyleComboBox = dynamic_cast<ConfigComboBox *>(widgetById("emoticonsStyle"));
	emoticonsThemeComboBox = dynamic_cast<ConfigComboBox *>(widgetById("emoticonsTheme"));
	connect(emoticonsStyleComboBox, SIGNAL(activated(int)), this, SLOT(onChangeEmoticonsStyle(int)));

	QWidget *showInformationPanel = widgetById("showInformationPanel");
	connect(showInformationPanel, SIGNAL(toggled(bool)), widgetById("showVerticalScrollbar"), SLOT(setEnabled(bool)));
	connect(showInformationPanel, SIGNAL(toggled(bool)), widgetById("showEmoticonsInPanel"), SLOT(setEnabled(bool)));

	browserComboBox = dynamic_cast<ConfigComboBox *>(widgetById("browser"));
	browserCommandLineEdit = dynamic_cast<ConfigLineEdit *>(widgetById("browserPath"));
	connect(browserComboBox, SIGNAL(activated(int)), this, SLOT(onChangeBrowser(int)));

	mailComboBox = dynamic_cast<ConfigComboBox *>(widgetById("mail"));
	mailCommandLineEdit = dynamic_cast<ConfigLineEdit *>(widgetById("mailPath"));
	connect(mailComboBox, SIGNAL(activated(int)), this, SLOT(onChangeMail(int)));

	QWidget *useProxy = widgetById("useProxy");
	QLineEdit *proxyPassword = (dynamic_cast<QLineEdit *>(widgetById("proxyPassword")));
	proxyPassword->setEchoMode(QLineEdit::Password);

	connect(useProxy, SIGNAL(toggled(bool)), widgetById("proxyHost"), SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), widgetById("proxyPort"), SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), widgetById("proxyUser"), SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), proxyPassword, SLOT(setEnabled(bool)));

	connect(widgetById("lookChatAdvanced"), SIGNAL(clicked()), this, SLOT(showLookChatAdvanced()));

	Preview *infoPanelSyntaxPreview = dynamic_cast<Preview *>(widgetById("infoPanelSyntaxPreview"));
	infoPanelSyntaxPreview->setResetBackgroundColor(config_file.readEntry("Look", "InfoPanelBgColor"));
	connect(widgetById("infoPanelSyntax"), SIGNAL(syntaxChanged(const QString &)), infoPanelSyntaxPreview, SLOT(syntaxChanged(const QString &)));

	chatPreview = dynamic_cast<Preview *>(widgetById("chatSyntaxPreview"));
	prepareChatPreview(chatPreview, true);

	connect(widgetById("chatSyntax"), SIGNAL(syntaxChanged(const QString &)), widgetById("chatSyntaxPreview"), SLOT(syntaxChanged(const QString &)));
	connect(widgetById("chatSyntax"), SIGNAL(onSyntaxEditorWindowCreated(SyntaxEditorWindow *)),
		this, SLOT(onChatSyntaxEditorWindowCreated(SyntaxEditorWindow *)));

// 	connect(widgetById("iconPaths"), SIGNAL(changed()), this, SLOT(setIconThemes()));

	loadGeometry(this, "General", "ConfigGeometry", 0, 30, 790, 480);
}

MainConfigurationWindow::~MainConfigurationWindow()
{
	saveGeometry(this, "General", "ConfigGeometry");
	Instance = 0;
}

void MainConfigurationWindow::show()
{
	if (!isShown())
	{
		setLanguages();
		setQtThemes();
		setIconThemes();
		setEmoticonThemes();
		setToolTipClasses();

		ConfigurationWindow::show();
	}
}

void MainConfigurationWindow::prepareChatPreview(Preview *preview, bool append)
{
	kdebugf();

	UserStatus status;
	status.setBusy(qApp->translate("@default", "Description"));

	UserListElement example;
	example.addProtocol("Gadu", "999999");
	example.setStatus("Gadu", status);
	example.setFirstName(qApp->translate("@default", "Mark"));
	example.setLastName(qApp->translate("@default", "Smith"));
	example.setNickName(qApp->translate("@default", "Jimbo"));
	example.setAltNick(qApp->translate("@default", "Jimbo"));
	example.setMobile("+48123456789");
	example.setEmail("jimbo@mail.server.net");
	example.setHomePhone("+481234567890");
	example.setAddressAndPort("Gadu", QHostAddress(2130706433), 80);
	example.setDNSName("Gadu", "host.server.net");

	preview->setResetBackgroundColor(config_file.readEntry("Look", "ChatBgColor"));
	preview->setStyleSheet(new StaticStyleSheet(chatPreview, emoticons->themePath()));

	ChatMessage *chatMessage = new ChatMessage(kadu->myself(), "Your message", true, QDateTime::currentDateTime(), QDateTime::currentDateTime());
	chatMessage->setSeparatorSize(0);
	preview->addObjectToParse(kadu->myself(), chatMessage);
	if (append)
		chatMessages.append(chatMessage);

	chatMessage = new ChatMessage(example, "Message from Your friend", false, QDateTime::currentDateTime(), QDateTime::currentDateTime());
	chatMessage->setSeparatorSize(4);
	preview->addObjectToParse(example, chatMessage);
	if (append)
		chatMessages.append(chatMessage);

	connect(preview, SIGNAL(needSyntaxFixup(QString &)), this, SLOT(chatSyntaxFixup(QString &)));
	connect(preview, SIGNAL(needFixup(Preview *)), this, SLOT(chatFixup(Preview *)));
}

void MainConfigurationWindow::chatSyntaxFixup(QString &syntax)
{
	syntax.replace("<kadu:header>", "");
	syntax.replace("</kadu:header>", "");
}

void MainConfigurationWindow::chatFixup(Preview *preview)
{
	int i = 0;
	CONST_FOREACH(chatMessage, chatMessages)
		preview->setParagraphBackgroundColor(i++, (*chatMessage)->backgroundColor);
}

void MainConfigurationWindow::onChangeStartupStatus(int index)
{
	onStartupSetLastDescription->setEnabled(index != 4);
	widgetById("onStartupSetDescription")->setEnabled(!onStartupSetLastDescription->isChecked() && index != 4);
}

void MainConfigurationWindow::setLanguages()
{
	ConfigComboBox *languages = dynamic_cast<ConfigComboBox *>(widgetById("languages"));

	QDir locale(dataPath("kadu/translations/"), "kadu_*.qm");
	QStringList files = locale.entryList();
	QStringList itemValues;
	QStringList itemCaptions;

	FOREACH(file, files)
	{
		QString itemValue = (*file).mid(5, (*file).length() - 8);
		QString itemCaption = translateLanguage(qApp, itemValue, true);

		itemValues.append(itemValue);
		itemCaptions.append(itemCaption);
	}

	languages->setItems(itemValues, itemCaptions);
}

void MainConfigurationWindow::setQtThemes()
{
	ConfigComboBox *qtThemes = dynamic_cast<ConfigComboBox *>(widgetById("qtThemes"));

	QStringList themes = QStyleFactory::keys();
	QString currentStyle = QApplication::style().name();
	if (!themes.contains(currentStyle))
		themes.append(currentStyle);

	qtThemes->setItems(themes, themes);
	qtThemes->setCurrentText(currentStyle);
}

void MainConfigurationWindow::setIconThemes()
{
	ConfigComboBox *iconThemes = dynamic_cast<ConfigComboBox *>(widgetById("iconThemes"));

	QStringList themes = icons_manager->themes();

	iconThemes->setItems(themes, themes);
	iconThemes->setCurrentText(icons_manager->theme());
}

void MainConfigurationWindow::setEmoticonThemes()
{
	ConfigComboBox *emoticonsTheme = dynamic_cast<ConfigComboBox *>(widgetById("emoticonsTheme"));

	QStringList themes = emoticons->themes();

	emoticonsTheme->setItems(themes, themes);
}

void MainConfigurationWindow::setToolTipClasses()
{
	QStringList captions;
	QStringList values;
	captions << tr("None");
	values << "";

	QStringList toolTipClasses = tool_tip_class_manager->getToolTipClasses();
	CONST_FOREACH(toolTipClass, toolTipClasses)
	{
		captions << tr(*toolTipClass);
		values << *toolTipClass;
	}

	dynamic_cast<ConfigComboBox *>(widgetById("toolTipClasses"))->setItems(values, captions);
}

QString MainConfigurationWindow::findExecutable(const QStringList &paths, const QStringList &executableNames)
{
	QFileInfo fi;

	CONST_FOREACH(path, paths)
		CONST_FOREACH(executableName, executableNames)
		{
			fi.setFile(*path + "/" + *executableName);
			if (fi.isExecutable())
				return *path + "/" + *executableName;
		}

	return QString::null;
}

void MainConfigurationWindow::onChangeEmoticonsStyle(int index)
{
	emoticonsThemeComboBox->setEnabled(emoticonsStyleComboBox->currentItemValue() != "0");
}

void MainConfigurationWindow::onChangeBrowser(int index)
{
	QStringList searchPath = QStringList::split(":", QString(getenv("PATH")));
	QStringList executableName;
// 	QStringList options;

	QString parameters;

	browserCommandLineEdit->setEnabled(index == 0);
// 	browserOptionComboBox->setEnabled(index >= 2 && index <= 4);

	switch (index)
	{
		case 1: // konqueror
		{
			searchPath.append("/opt/kde/bin");
			searchPath.append("/opt/kde3/bin");
			executableName.append("kfmclient");

			parameters = "openURL";

// 			options << tr("Open in new window") << tr("Open in new tab");
// 			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 2: // opera
		{
			searchPath.append("/opt/opera");
			executableName.append("opera");

			parameters = "";

// 			options << tr("Open in new window") << tr("Open in new tab") << tr("Open in background tab");
			break;
		}
		case 3: // mozilla
		{
			QString homePath = getenv("HOME");
			QStringList dirList = QDir("/usr/lib").entryList("mozilla*", QDir::All, QDir::Name|QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/" + (*dir));

			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append(homePath + "/Mozilla");
			searchPath.append(homePath + "/mozilla");
			executableName.append("mozilla");
// it is for old mozillas, unsupported
// 			executableName.append("mozilla-xremote-client");

			parameters = "";

// 			options << tr("Open in new window") << tr("Open in new tab");
			break;
		}
		case 4: // firefox
		{
			QString homePath = getenv("HOME");

			QStringList dirList = QDir("/usr/lib").entryList("mozilla-firefox*", QDir::All, QDir::Name | QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/" + (*dir));
			dirList = QDir("/usr/lib").entryList("firefox*", QDir::All, QDir::Name | QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/" + (*dir));

			searchPath.append("/usr/lib/MozillaFirefox");
			searchPath.append("/usr/local/Firefox");
			searchPath.append("/usr/local/firefox");
			searchPath.append("/opt/firefox");
			searchPath.append(homePath + "/Firefox");
			searchPath.append(homePath + "/firefox");
			executableName.append("firefox");

			parameters = "";
//	do we need it anyway ??
// 			executableName.append("mozilla-xremote-client");
// 			executableName.append("mozilla-firefox-xremote-client");
// 			executableName.append("firefox-xremote-client");

			dirList = QDir("/usr/lib").entryList("mozilla*", QDir::All, QDir::Name | QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/" + (*dir));

// 			options << tr("Open in new window") << tr("Open in new tab");
			break;
		}
		case 5: // dillo
			executableName.append("dillo");
		case 6: // galeon
			executableName.append("galeon");
		case 7: // Safaro
		{
			searchPath.append("/Applications");
			executableName.append("Safari.app");
			parameters = "open";
		}
	}

// 	browserOptionComboBox->clear();
// 	browserOptionComboBox->setItems(options, options);

	if (index != 0)
	{
		QString executable = findExecutable(searchPath, executableName);
		if (!executable.isNull())
			browserCommandLineEdit->setText(executable + " " + parameters);
		else
			browserCommandLineEdit->setText(tr("Not found"));
	}
	else
		browserCommandLineEdit->setText("");
}

// void MainConfigurationWindow::onChangeBrowserOption(int index)
// {
// }

void MainConfigurationWindow::onChangeMail(int index)
{
	QStringList searchPath = QStringList::split(":", QString(getenv("PATH")));
	QStringList executableName;
	QString parameters;

	mailCommandLineEdit->setEnabled(index == 0);

	switch (index)
	{
		case 1: // kmail
		{
			searchPath.append("/opt/kde/bin");
			searchPath.append("/opt/kde3/bin");
			executableName.append("kmail");
			break;
		}
		case 2: // thunderbird
		{
			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append("/usr/local/Thunderbird");
			searchPath.append("/usr/local/thunderbird");
			searchPath.append("/opt/thunderbird");
			executableName.append("thunderbird");
			executableName.append("mozilla-thunderbird");
			parameters = " -compose mailto:";
			break;
		}
		case 3: // seamonkey
		{
			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append("/usr/local/Seamonkey");
			searchPath.append("/usr/local/seamonkey");
			searchPath.append("/opt/seamonkey");
			executableName.append("seamonkey");
			parameters = " -compose mailto:";
			break;
		}
		case 4: // evolution
		{
			searchPath.append("/opt/evolution");
			searchPath.append("/usr/local/evolution");
			searchPath.append("/usr/local/Evolution");
			executableName.append("evolution");
			parameters = " mailto:";
			break;
		}
	}

	if (index != 0)
	{
		QString executable = findExecutable(searchPath, executableName);
		if (!executable.isNull())
			mailCommandLineEdit->setText(executable + parameters);
		else
			mailCommandLineEdit->setText(tr("Not found"));
	}
	else
		mailCommandLineEdit->setText("");
}

void MainConfigurationWindow::onChatSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow)
{
	prepareChatPreview(syntaxEditorWindow->preview());
}

void MainConfigurationWindow::showLookChatAdvanced()
{
	if (!lookChatAdvanced)
	{
		lookChatAdvanced = new ConfigurationWindow();
		lookChatAdvanced->appendUiFile(dataPath("kadu/configuration/dialog-look-chat-advanced.ui"));

		connect(lookChatAdvanced->widgetById("removeServerTime"), SIGNAL(toggled(bool)), lookChatAdvanced->widgetById("maxTimeDifference"), SLOT(setEnabled(bool)));
		connect(lookChatAdvanced->widgetById("noHeaderRepeat"), SIGNAL(toggled(bool)), lookChatAdvanced->widgetById("noHeaderInterval"), SLOT(setEnabled(bool)));
		connect(lookChatAdvanced->widgetById("chatPrune"), SIGNAL(toggled(bool)), lookChatAdvanced->widgetById("chatPruneLen"), SLOT(setEnabled(bool)));
		connect(lookChatAdvanced->widgetById("foldLink"), SIGNAL(toggled(bool)), lookChatAdvanced->widgetById("linkFoldTreshold"), SLOT(setEnabled(bool)));
		connect(lookChatAdvanced->widgetById("chatCloseTimer"), SIGNAL(toggled(bool)), lookChatAdvanced->widgetById("chatCloseTimerPeriod"), SLOT(setEnabled(bool)));

		connect(lookChatAdvanced, SIGNAL(destroyed()), this, SLOT(lookChatAdvancedDestroyed()));
	}

	lookChatAdvanced->show();
}

void MainConfigurationWindow::lookChatAdvancedDestroyed()
{
	lookChatAdvanced = 0;
}
