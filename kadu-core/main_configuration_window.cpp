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
	: ConfigurationWindow("main", tr("Kadu configuration")), lookChatAdvanced(0)
{
	appendUiFile(dataPath("kadu/configuration/dialog.ui"));

#ifndef DEBUG_ENABLED
	((QWidget *)(widgetById("debug")->parent()))->hide();
#endif

	onStartupSetLastDescription = dynamic_cast<QCheckBox *>(widgetById("onStartupSetLastDescription"));
	QLineEdit *disconnectDescription = dynamic_cast<QLineEdit *>(widgetById("disconnectDescription"));
	QLineEdit *onStartupSetDescription = dynamic_cast<QLineEdit *>(widgetById("onStartupSetDescription"));
	disconnectDescription->setMaxLength(GG_STATUS_DESCR_MAXSIZE);
	onStartupSetDescription->setMaxLength(GG_STATUS_DESCR_MAXSIZE);

	connect(widgetById("disconnectWithCurrentDescription"), SIGNAL(toggled(bool)), disconnectDescription, SLOT(setDisabled(bool)));
	connect(onStartupSetLastDescription, SIGNAL(toggled(bool)), onStartupSetDescription, SLOT(setDisabled(bool)));
	connect(widgetById("foldLink"), SIGNAL(toggled(bool)), widgetById("linkFoldTreshold"), SLOT(setEnabled(bool)));
	connect(widgetById("chatPrune"), SIGNAL(toggled(bool)), widgetById("chatPruneLen"), SLOT(setEnabled(bool)));
	connect(widgetById("chatCloseTimer"), SIGNAL(toggled(bool)), widgetById("chatCloseTimerPeriod"), SLOT(setEnabled(bool)));
	connect(widgetById("startupStatus"), SIGNAL(activated(int)), this, SLOT(onChangeStartupStatus(int)));
	connect(widgetById("showDescription"), SIGNAL(toggled(bool)), widgetById("multilineDescription"), SLOT(setEnabled(bool)));
	connect(widgetById("useDefaultServers"), SIGNAL(toggled(bool)), widgetById("serverList"), SLOT(setDisabled(bool)));
	connect(widgetById("openChatOnMessage"), SIGNAL(toggled(bool)), widgetById("openChatOnMessageWhenOnline"), SLOT(setEnabled(bool)));

	emoticonsStyleComboBox = dynamic_cast<ConfigComboBox *>(widgetById("emoticonsStyle"));
	emoticonsThemeComboBox = dynamic_cast<ConfigComboBox *>(widgetById("emoticonsTheme"));
	connect(emoticonsStyleComboBox, SIGNAL(activated(int)), this, SLOT(onChangeEmoticonsStyle(int)));
	connect(widgetById("emoticonsPaths"), SIGNAL(changed()), this, SLOT(setEmoticonThemes()));

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

 	connect(widgetById("iconPaths"), SIGNAL(changed()), this, SLOT(setIconThemes()));

	connect(widgetById("ignoreMessagesFromAnonymous"), SIGNAL(toggled(bool)), widgetById("ignoreMessagesFromAnonymousInConferences"), SLOT(setEnabled(bool)));

	QWidget *useUserboxBackground = widgetById("useUserboxBackground");
	connect(useUserboxBackground, SIGNAL(toggled(bool)), widgetById("userboxBackground"), SLOT(setEnabled(bool)));
	connect(useUserboxBackground, SIGNAL(toggled(bool)), widgetById("userboxBackgroundDisplayStyle"), SLOT(setEnabled(bool)));

	QToolTip::add(widgetById("parseStatus") , qApp->translate("@default", Kadu::SyntaxText));
	(dynamic_cast<ConfigSyntaxEditor *>(widgetById("chatSyntax")))->setSyntaxHint(qApp->translate("@default", Kadu::SyntaxTextExtended));
	(dynamic_cast<ConfigSyntaxEditor *>(widgetById("infoPanelSyntax")))->setSyntaxHint(qApp->translate("@default", Kadu::SyntaxText));

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
	}

	ConfigurationWindow::show();
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

	ChatMessage *chatMessage = new ChatMessage(kadu->myself(), tr("Your message"), TypeSent,
		QDateTime::currentDateTime(), QDateTime::currentDateTime());
	chatMessage->setSeparatorSize(0);
	preview->addObjectToParse(kadu->myself(), chatMessage);
	if (append)
		chatMessages.append(chatMessage);

	chatMessage = new ChatMessage(example, tr("Message from Your friend"), TypeReceived,
		QDateTime::currentDateTime(), QDateTime::currentDateTime());
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
	widgetById("startupStatusInvisibleWhenLastWasOffline")->setEnabled(index == 0);
	widgetById("onStartupSetDescription")->setEnabled(!onStartupSetLastDescription->isChecked() && index != 4);
}

void MainConfigurationWindow::setLanguages()
{
	ConfigComboBox *languages = dynamic_cast<ConfigComboBox *>(widgetById("languages"));

	QStringList files;
	files.append("kadu_pl.qm");
	files.append("kadu_en.qm");
	files.append("kadu_de.qm");
	files.append("kadu_it.qm");

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
	icons_manager->setPaths((dynamic_cast<PathListEdit *>(widgetById("iconPaths")))->pathList());

	QT_TRANSLATE_NOOP("@default", "default");
	QStringList themes = icons_manager->themes();
	QStringList captions;
	themes.sort();

	FOREACH(theme, themes)
		captions.append(qApp->translate("@default", *theme));

	iconThemes->setItems(themes, captions);
	iconThemes->setCurrentText(icons_manager->theme());
}

void MainConfigurationWindow::setEmoticonThemes()
{
	ConfigComboBox *emoticonsTheme = dynamic_cast<ConfigComboBox *>(widgetById("emoticonsTheme"));
	emoticons->setPaths((dynamic_cast<PathListEdit *>(widgetById("emoticonsPaths")))->pathList());

	QStringList themes = emoticons->themes();
	themes.sort();

	emoticonsTheme->setItems(themes, themes);
	emoticonsTheme->setCurrentText(emoticons->theme());
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
		captions << qApp->translate("@default", *toolTipClass);
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

QString MainConfigurationWindow::getBrowserExecutable(int browserIndex)
{
	QStringList searchPath = QStringList::split(":", QString(getenv("PATH")));
	QStringList executableName;
// 	QStringList options;

	QString parameters;

// 	browserOptionComboBox->setEnabled(index >= 2 && index <= 4);

	switch (browserIndex)
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
		case 3: // opera with params
		{
			searchPath.append("/opt/opera");
			executableName.append("opera");

			if (browserIndex == 3)
				parameters = "-newpage";

// 			options << tr("Open in new window") << tr("Open in new tab") << tr("Open in background tab");
			break;
		}
		case 4: // mozilla
		{
			QString homePath = getenv("HOME");
			QStringList dirList = QDir("/usr/lib").entryList("mozilla*", QDir::All, QDir::Name | QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/" + (*dir));

			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append(homePath + "/Mozilla");
			searchPath.append(homePath + "/mozilla");
			executableName.append("mozilla");
// it is for old mozillas, unsupported
// 			executableName.append("mozilla-xremote-client");

//			parameters = "";

// 			options << tr("Open in new window") << tr("Open in new tab");
			break;
		}
		case 5: // seamonkey
		{
			QString homePath = getenv("HOME");
			QStringList dirList = QDir("/usr/lib").entryList("seamonkey*", QDir::All, QDir::Name | QDir::Reversed);
			CONST_FOREACH(dir, dirList)
				searchPath.append("/usr/lib/" + (*dir));

			searchPath.append("/usr/local/Seamonkey");
    			searchPath.append("/usr/local/seamonkey");
			searchPath.append(homePath + "/Seamonkey");
			searchPath.append(homePath + "/seamonkey");
			executableName.append("seamonkey");

			// options << tr("Open in new window") << tr("Open in new tab");
			break;
		}
		case 6: // firefox
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

//			parameters = "";
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
		case 7: // dillo
		{
			executableName.append("dillo");
			break;
		}
		case 8: // galeon
		{
			executableName.append("galeon");
			break;
		}
		case 9: // Safari
		{
			searchPath.append("/Applications");
			executableName.append("Safari.app");
			parameters = "open";
			break;
		}
	}

// 	browserOptionComboBox->clear();
// 	browserOptionComboBox->setItems(options, options);

	if (browserIndex != 0)
	{
		QString executable = findExecutable(searchPath, executableName);
		if (!executable.isNull())
			return executable + " " + parameters;
		else
			return tr("Not found");
	}
	else
		return QString::null;
}

void MainConfigurationWindow::onChangeBrowser(int index)
{
	browserCommandLineEdit->setEnabled(index == 0);
	browserCommandLineEdit->setText(getBrowserExecutable(index));
}

// void MainConfigurationWindow::onChangeBrowserOption(int index)
// {
// }

QString MainConfigurationWindow::getEMailExecutable(int emailIndex)
{
	QStringList searchPath = QStringList::split(":", QString(getenv("PATH")));
	QStringList executableName;
	QString parameters;

	switch (emailIndex)
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
			parameters = "-compose mailto:";
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
			parameters = "-compose mailto:";
			break;
		}
		case 4: // evolution
		{
			searchPath.append("/opt/evolution");
			searchPath.append("/usr/local/evolution");
			searchPath.append("/usr/local/Evolution");
			executableName.append("evolution");
			parameters = "mailto:";
			break;
		}
	}

	if (emailIndex != 0)
	{
		QString executable = findExecutable(searchPath, executableName);
		if (!executable.isNull())
			return (executable + " " + parameters);
		else
			return tr("Not found");
	}
	else
		return QString::null;
}

void MainConfigurationWindow::onChangeMail(int index)
{
	mailCommandLineEdit->setEnabled(index == 0);
	mailCommandLineEdit->setText(getEMailExecutable(index));
}

QString MainConfigurationWindow::browserIndexToString(int browserIndex)
{
	switch (browserIndex)
	{
		case 0: return "Specify path";
		case 1: return "Konqueror";
		case 2: return "Opera";
		case 3: return "Opera (new tab)";
		case 4: return "Mozilla";
		case 5: return "SeaMonkey";
		case 6: return "Mozilla Firefox";
		case 7: return "Dillo";
		case 8: return "Galeon";
		case 9: return "Safari";
		default: return QString::null;
	}
}

QString MainConfigurationWindow::emailIndexToString(int emailIndex)
{
	switch (emailIndex)
	{
		case 0: return "Specify path";
		case 1: return "KMail";
		case 2: return "Thunderbird";
		case 3: return "SeaMonkey";
		case 4: return "Evolution";
		default: return QString::null;
	}
}

void MainConfigurationWindow::onChatSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow)
{
	prepareChatPreview(syntaxEditorWindow->preview());
}

void MainConfigurationWindow::showLookChatAdvanced()
{
	if (!lookChatAdvanced)
	{
		lookChatAdvanced = new ConfigurationWindow("dialog-look-chat-advanced", tr("Advenced chat's look configuration"));
		lookChatAdvanced->appendUiFile(dataPath("kadu/configuration/dialog-look-chat-advanced.ui"));

		connect(lookChatAdvanced->widgetById("removeServerTime"), SIGNAL(toggled(bool)), lookChatAdvanced->widgetById("maxTimeDifference"), SLOT(setEnabled(bool)));
		connect(lookChatAdvanced->widgetById("noHeaderRepeat"), SIGNAL(toggled(bool)), lookChatAdvanced->widgetById("noHeaderInterval"), SLOT(setEnabled(bool)));

		QToolTip::add(lookChatAdvanced->widgetById("chatSyntax") , qApp->translate("@default", Kadu::SyntaxText));
		QToolTip::add(lookChatAdvanced->widgetById("conferencePrefix") , qApp->translate("@default", Kadu::SyntaxText));
		QToolTip::add(lookChatAdvanced->widgetById("conferenceSyntax") , qApp->translate("@default", Kadu::SyntaxText));

		connect(lookChatAdvanced, SIGNAL(destroyed()), this, SLOT(lookChatAdvancedDestroyed()));
	}

	lookChatAdvanced->show();
}

void MainConfigurationWindow::lookChatAdvancedDestroyed()
{
	lookChatAdvanced = 0;
}

void MainConfigurationWindow::keyPressEvent(QKeyEvent *e)
{
	ConfigurationWindow::keyPressEvent(e);
}
