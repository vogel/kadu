/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtGui/QApplication>
#include <QtGui/QStyleFactory>

#include "configuration/config-file-data-manager.h"

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat_message.h"
#include "chat/chat_styles_manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-data.h"
#include "contacts/contact-account-data.h"
#include "core/core.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "gui/widgets/configuration/config-preview.h"
#include "gui/widgets/configuration/config-syntax-editor.h"
#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/chat_messages_view.h"
#include "gui/widgets/contact-info-panel.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "gui/windows/kadu-window.h"
#include "protocols/status.h"

//#include "../modules/gadu_protocol/gadu-protocol.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "main_configuration_window.h"

MainConfigurationWindow *MainConfigurationWindow::Instance = 0;
ConfigFileDataManager *MainConfigurationWindow::InstanceDataManager = 0;
QList<QString> MainConfigurationWindow::UiFiles;
QList<ConfigurationUiHandler *> MainConfigurationWindow::ConfigurationUiHandlers;

const char *MainConfigurationWindow::SyntaxText = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size\n"
);

const char *MainConfigurationWindow::SyntaxTextNotify = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
	"#{protocol} - protocol that triggered event,\n"
	"#{event} - name of event,\n"
);

MainConfigurationWindow * MainConfigurationWindow::instance()
{
	if (!Instance)
	{
		InstanceDataManager = new ConfigFileDataManager();
		Instance = new MainConfigurationWindow();
		instanceCreated();
	}

	return Instance;
}

void MainConfigurationWindow::registerUiFile(const QString &uiFile)
{
	UiFiles.append(uiFile);
	if (Instance)
	{
		QList<ConfigWidget *> widgets = Instance->widget()->appendUiFile(uiFile);

		// allow uiHandler handle this...
		// TODO: make it pretty
		foreach(ConfigWidget *widget, widgets)
			if (widget)
				widget->loadConfiguration();
	}
}

void MainConfigurationWindow::unregisterUiFile(const QString &uiFile)
{
	UiFiles.removeAll(uiFile);
	if (Instance)
		Instance->widget()->removeUiFile(uiFile);
}

void MainConfigurationWindow::registerUiHandler(ConfigurationUiHandler *uiHandler)
{
	ConfigurationUiHandlers.append(uiHandler);
	if (Instance)
		uiHandler->mainConfigurationWindowCreated(Instance);
}

void MainConfigurationWindow::unregisterUiHandler(ConfigurationUiHandler *uiHandler)
{
	ConfigurationUiHandlers.removeAll(uiHandler);
}

void MainConfigurationWindow::instanceCreated()
{
	ChatStylesManager::instance()->mainConfigurationWindowCreated(Instance);
	foreach (const QString &uiFile, UiFiles)
		Instance->widget()->appendUiFile(uiFile);

	foreach (ConfigurationUiHandler *uiHandler, ConfigurationUiHandlers)
		if (uiHandler)
			uiHandler->mainConfigurationWindowCreated(Instance);
}

MainConfigurationWindow::MainConfigurationWindow()
	: ConfigurationWindow("main", tr("Kadu configuration"), InstanceDataManager), lookChatAdvanced(0)
{
	widget()->appendUiFile(dataPath("kadu/configuration/dialog.ui"));

#ifndef DEBUG_ENABLED
	((QWidget *)(widget()->widgetById("debug")->parent()))->hide();
#endif

#ifndef Q_OS_WIN
	((QWidget *)(widget()->widgetById("startup")->parent()))->hide();
#endif

	onStartupSetLastDescription = dynamic_cast<QCheckBox *>(widget()->widgetById("onStartupSetLastDescription"));
	QLineEdit *disconnectDescription = dynamic_cast<QLineEdit *>(widget()->widgetById("disconnectDescription"));
	QLineEdit *onStartupSetDescription = dynamic_cast<QLineEdit *>(widget()->widgetById("onStartupSetDescription"));
//TODO 0.6.6:
	Account * account = AccountManager::instance()->defaultAccount();
	if ( 0 != account && 0 != account->protocol())
	{
	//	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(account->protocol());
	//	disconnectDescription->setMaxLength(gadu->maxDescriptionLength());
	//	onStartupSetDescription->setMaxLength(gadu->maxDescriptionLength());
	}

	connect(widget()->widgetById("disconnectWithCurrentDescription"), SIGNAL(toggled(bool)), disconnectDescription, SLOT(setDisabled(bool)));
	connect(onStartupSetLastDescription, SIGNAL(toggled(bool)), onStartupSetDescription, SLOT(setDisabled(bool)));
	connect(widget()->widgetById("foldLink"), SIGNAL(toggled(bool)), widget()->widgetById("linkFoldTreshold"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("chatPrune"), SIGNAL(toggled(bool)), widget()->widgetById("chatPruneLen"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("chatCloseTimer"), SIGNAL(toggled(bool)), widget()->widgetById("chatCloseTimerPeriod"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("startupStatus"), SIGNAL(activated(int)), this, SLOT(onChangeStartupStatus(int)));
	connect(widget()->widgetById("showDescription"), SIGNAL(toggled(bool)), widget()->widgetById("multilineDescription"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("useDefaultServers"), SIGNAL(toggled(bool)), widget()->widgetById("serverList"), SLOT(setDisabled(bool)));
	connect(widget()->widgetById("openChatOnMessage"), SIGNAL(toggled(bool)), widget()->widgetById("openChatOnMessageWhenOnline"), SLOT(setEnabled(bool)));

	emoticonsStyleComboBox = dynamic_cast<ConfigComboBox *>(widget()->widgetById("emoticonsStyle"));
	emoticonsThemeComboBox = dynamic_cast<ConfigComboBox *>(widget()->widgetById("emoticonsTheme"));
	connect(emoticonsStyleComboBox, SIGNAL(activated(int)), this, SLOT(onChangeEmoticonsStyle(int)));
	connect(widget()->widgetById("emoticonsPaths"), SIGNAL(changed()), this, SLOT(setEmoticonThemes()));

	QWidget *showInformationPanel = widget()->widgetById("showInformationPanel");
	connect(showInformationPanel, SIGNAL(toggled(bool)), widget()->widgetById("showVerticalScrollbar"), SLOT(setEnabled(bool)));
	connect(showInformationPanel, SIGNAL(toggled(bool)), widget()->widgetById("showEmoticonsInPanel"), SLOT(setEnabled(bool)));

	browserComboBox = dynamic_cast<ConfigComboBox *>(widget()->widgetById("browser"));
	browserCommandLineEdit = dynamic_cast<ConfigLineEdit *>(widget()->widgetById("browserPath"));
	connect(browserComboBox, SIGNAL(activated(int)), this, SLOT(onChangeBrowser(int)));

	mailComboBox = dynamic_cast<ConfigComboBox *>(widget()->widgetById("mail"));
	mailCommandLineEdit = dynamic_cast<ConfigLineEdit *>(widget()->widgetById("mailPath"));
	connect(mailComboBox, SIGNAL(activated(int)), this, SLOT(onChangeMail(int)));

	QWidget *useProxy = widget()->widgetById("useProxy");
	QLineEdit *proxyPassword = (dynamic_cast<QLineEdit *>(widget()->widgetById("proxyPassword")));
	proxyPassword->setEchoMode(QLineEdit::Password);

	connect(useProxy, SIGNAL(toggled(bool)), widget()->widgetById("proxyHost"), SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), widget()->widgetById("proxyPort"), SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), widget()->widgetById("proxyUser"), SLOT(setEnabled(bool)));
	connect(useProxy, SIGNAL(toggled(bool)), proxyPassword, SLOT(setEnabled(bool)));

	connect(widget()->widgetById("lookChatAdvanced"), SIGNAL(clicked()), this, SLOT(showLookChatAdvanced()));

	Preview *infoPanelSyntaxPreview = dynamic_cast<Preview *>(widget()->widgetById("infoPanelSyntaxPreview"));
	infoPanelSyntaxPreview->setResetBackgroundColor(config_file.readEntry("Look", "InfoPanelBgColor"));
	connect(infoPanelSyntaxPreview, SIGNAL(needFixup(QString &)), Core::instance()->kaduWindow()->infoPanel(), SLOT(styleFixup(QString &)));
	connect(widget()->widgetById("infoPanelSyntax"), SIGNAL(syntaxChanged(const QString &)), infoPanelSyntaxPreview, SLOT(syntaxChanged(const QString &)));
	connect(widget()->widgetById("infoPanelSyntax"), SIGNAL(onSyntaxEditorWindowCreated(SyntaxEditorWindow *)),
		this, SLOT(onInfoPanelSyntaxEditorWindowCreated(SyntaxEditorWindow *)));

 	connect(widget()->widgetById("iconPaths"), SIGNAL(changed()), this, SLOT(setIconThemes()));

	connect(widget()->widgetById("ignoreMessagesFromAnonymous"), SIGNAL(toggled(bool)), widget()->widgetById("ignoreMessagesFromAnonymousInConferences"), SLOT(setEnabled(bool)));

	QWidget *useUserboxBackground = widget()->widgetById("useUserboxBackground");
	connect(useUserboxBackground, SIGNAL(toggled(bool)), widget()->widgetById("userboxBackground"), SLOT(setEnabled(bool)));
	connect(useUserboxBackground, SIGNAL(toggled(bool)), widget()->widgetById("userboxBackgroundDisplayStyle"), SLOT(setEnabled(bool)));

	widget()->widgetById("parseStatus")->setToolTip(qApp->translate("@default", SyntaxText));
	(dynamic_cast<ConfigSyntaxEditor *>(widget()->widgetById("infoPanelSyntax")))->setSyntaxHint(qApp->translate("@default", SyntaxText));

	loadWindowGeometry(this, "General", "ConfigGeometry", 0, 50, 790, 480);
}

MainConfigurationWindow::~MainConfigurationWindow()
{
 	saveWindowGeometry(this, "General", "ConfigGeometry");
	Instance = 0;
}

void MainConfigurationWindow::show()
{
	if (!isVisible())
	{
		setLanguages();
		setQtThemes();
		setIconThemes();
		setEmoticonThemes();
		setToolTipClasses();
	}

	ConfigurationWindow::show();
}

void MainConfigurationWindow::onChangeStartupStatus(int index)
{
	onStartupSetLastDescription->setEnabled(index != 4);
	widget()->widgetById("startupStatusInvisibleWhenLastWasOffline")->setEnabled(index == 0);
	widget()->widgetById("onStartupSetDescription")->setEnabled(!onStartupSetLastDescription->isChecked() && index != 4);
}

void MainConfigurationWindow::setLanguages()
{
	ConfigComboBox *languages = dynamic_cast<ConfigComboBox *>(widget()->widgetById("languages"));

	QStringList files;
	files.append("kadu_pl.qm");
	files.append("kadu_en.qm");
	files.append("kadu_de.qm");
	files.append("kadu_it.qm");

	QStringList itemValues;
	QStringList itemCaptions;

	foreach(const QString &file, files)
	{
		QString itemValue = file.mid(5, file.length() - 8);
		QString itemCaption = translateLanguage(qApp, itemValue, true);

		itemValues.append(itemValue);
		itemCaptions.append(itemCaption);
	}

	languages->setItems(itemValues, itemCaptions);
}

void MainConfigurationWindow::setQtThemes()
{
	ConfigComboBox *qtThemes = dynamic_cast<ConfigComboBox *>(widget()->widgetById("qtThemes"));

	QStringList themes = QStyleFactory::keys();
	QString currentStyle = QApplication::style()->objectName();

	foreach (const QString& it, themes)
		if (it.toLower() == currentStyle.toLower())
		{
			currentStyle = it;
			break;
		}

	if (!themes.contains(currentStyle))
		themes.append(currentStyle);

	qtThemes->setItems(themes, themes);
	// hack, this setting will by used in ConfigComboBox::loadConfiguration()
	config_file.writeEntry("Look", "QtStyle", currentStyle);
}

void MainConfigurationWindow::setIconThemes()
{
	ConfigComboBox *iconThemes = dynamic_cast<ConfigComboBox *>(widget()->widgetById("iconThemes"));
	IconsManager::instance()->setPaths((dynamic_cast<PathListEdit *>(widget()->widgetById("iconPaths")))->pathList());

	QT_TRANSLATE_NOOP("@default", "default");
	QStringList themes = IconsManager::instance()->themes();
	QStringList captions;
	themes.sort();

	foreach(const QString &theme, themes)
		captions.append(qApp->translate("@default", theme.toAscii().data()));

	iconThemes->setItems(themes, captions);
	iconThemes->setEditText(IconsManager::instance()->theme());
}

void MainConfigurationWindow::setEmoticonThemes()
{
	ConfigComboBox *emoticonsTheme = dynamic_cast<ConfigComboBox *>(widget()->widgetById("emoticonsTheme"));
	EmoticonsManager::instance()->setPaths((dynamic_cast<PathListEdit *>(widget()->widgetById("emoticonsPaths")))->pathList());

	QStringList themes = EmoticonsManager::instance()->themes();
	themes.sort();

	emoticonsTheme->setItems(themes, themes);
	emoticonsTheme->setEditText(EmoticonsManager::instance()->theme());
}

void MainConfigurationWindow::setToolTipClasses()
{
	QStringList captions;
	QStringList values;
	captions << tr("None");
	values << "";

	QStringList toolTipClasses = ToolTipClassManager::instance()->getToolTipClasses();
	foreach(const QString &toolTipClass, toolTipClasses)
	{
		captions << qApp->translate("@default", toolTipClass.toAscii().data());
		values << toolTipClass;
	}

	dynamic_cast<ConfigComboBox *>(widget()->widgetById("toolTipClasses"))->setItems(values, captions);
}

QString MainConfigurationWindow::findExecutable(const QStringList &paths, const QStringList &executableNames)
{
	QFileInfo fi;

	foreach(const QString &path, paths)
		foreach(const QString &executableName, executableNames)
		{
			fi.setFile(path + "/" + executableName);
			if (fi.isExecutable())
				return path + "/" + executableName;
		}

	return QString::null;
}

void MainConfigurationWindow::onChangeEmoticonsStyle(int index)
{
	emoticonsThemeComboBox->setEnabled(emoticonsStyleComboBox->currentItemValue() != "0");
}

QString MainConfigurationWindow::getBrowserExecutable(int browserIndex)
{
	QStringList searchPath = QString(getenv("PATH")).split(':');
	QStringList executableName;
// 	QStringList options;

	QString parameters;
	QString prefix;

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
#ifdef Q_OS_MAC
			searchPath.append("/Applications");
			executableName.append("Opera.app");
			prefix = "open -a ";
#else
			searchPath.append("/opt/opera");
			executableName.append("opera");

			// Dorr: on Mac always opens on new tab
			if (browserIndex == 3)
				parameters = "-newpage";

// 			options << tr("Open in new window") << tr("Open in new tab") << tr("Open in background tab");
#endif
			break;
		}
		case 4: // mozilla
		{
#ifdef Q_OS_MAC
			searchPath.append("/Applications");
			executableName.append("Mozilla.app");
			prefix = "open -a ";
#else
			QString homePath = getenv("HOME");
			QStringList dirList = QDir("/usr/lib").entryList(QStringList("mozilla*"), QDir::AllEntries, QDir::Name | QDir::Reversed);
			foreach(const QString &dir, dirList)
				searchPath.append("/usr/lib/" + dir);

			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append(homePath + "/Mozilla");
			searchPath.append(homePath + "/mozilla");
			executableName.append("mozilla");
// it is for old mozillas, unsupported
// 			executableName.append("mozilla-xremote-client");

//			parameters = "";

// 			options << tr("Open in new window") << tr("Open in new tab");
#endif
			break;
		}
		case 5: // seamonkey
		{
#ifdef Q_OS_MAC
			searchPath.append("/Applications");
			executableName.append("SeaMonkey.app");
			prefix = "open -a ";
#else
			QString homePath = getenv("HOME");
			QStringList dirList = QDir("/usr/lib").entryList(QStringList("seamonkey*"), QDir::AllEntries, QDir::Name | QDir::Reversed);
			foreach(const QString &dir, dirList)
				searchPath.append("/usr/lib/" + dir);

			searchPath.append("/usr/local/Seamonkey");
    			searchPath.append("/usr/local/seamonkey");
			searchPath.append(homePath + "/Seamonkey");
			searchPath.append(homePath + "/seamonkey");
			executableName.append("seamonkey");

			// options << tr("Open in new window") << tr("Open in new tab");
#endif
			break;
		}
		case 6: // firefox
		{
#ifdef Q_OS_MAC
			searchPath.append("/Applications");
			executableName.append("Firefox.app");
			prefix = "open -a ";
#else
			QString homePath = getenv("HOME");

			QStringList dirList = QDir("/usr/lib").entryList(QStringList("mozilla-firefox*"), QDir::AllEntries, QDir::Name | QDir::Reversed);
			foreach(const QString &dir, dirList)
				searchPath.append("/usr/lib/" + dir);
			dirList = QDir("/usr/lib").entryList(QStringList("firefox*"), QDir::AllEntries, QDir::Name | QDir::Reversed);
			foreach(const QString &dir, dirList)
				searchPath.append("/usr/lib/" + dir);

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

			dirList = QDir("/usr/lib").entryList(QStringList("mozilla*"), QDir::AllEntries, QDir::Name | QDir::Reversed);
			foreach(const QString &dir, dirList)
				searchPath.append("/usr/lib/" + dir);
#endif
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
			prefix = "open -a ";
			break;
		}
		case 10: // Camino
		{
			searchPath.append("/Applications");
			executableName.append("Camino.app");
			prefix = "open -a ";
			break;
		}
	}

// 	browserOptionComboBox->clear();
// 	browserOptionComboBox->setItems(options, options);

	if (browserIndex != 0)
	{
		QString executable = findExecutable(searchPath, executableName);
		if (!executable.isNull())
			return prefix + executable + " " + parameters;
		else
			return QString::null;
	}
	else
		return QString::null;
}

void MainConfigurationWindow::onChangeBrowser(int index)
{
	QString browser = getBrowserExecutable(index);
	browserCommandLineEdit->setEnabled(index == 0);
	browserCommandLineEdit->setText(browser);

	if (index != 0 && browser.isEmpty())
		if (!browserComboBox->currentText().contains(tr("Not found")))
			browserComboBox->setItemText(browserComboBox->currentIndex(),
					browserComboBox->currentText() + " (" + tr("Not found") + ")");
}

// void MainConfigurationWindow::onChangeBrowserOption(int index)
// {
// }

QString MainConfigurationWindow::getEMailExecutable(int emailIndex)
{
	QStringList searchPath = QString(getenv("PATH")).split(':');
	QStringList executableName;
	QString parameters;
	QString prefix = "";

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
		case 5: //mail
		{
			searchPath.append("/Applications");
			executableName.append("Mail.app");
			parameters = "mailto:";
			prefix = "open -a ";
		}
	}

	if (emailIndex != 0)
	{
		QString executable = findExecutable(searchPath, executableName);
		if (!executable.isNull())
			return (prefix + executable + " " + parameters);
		else
			return QString::null;
	}
	else
		return QString::null;
}

void MainConfigurationWindow::onChangeMail(int index)
{
	QString mail = getEMailExecutable(index);

	mailCommandLineEdit->setEnabled(index == 0);
	mailCommandLineEdit->setText(mail);

	if (index != 0 && mail.isEmpty())
		if (!mailComboBox->currentText().contains(tr("Not found")))
			mailComboBox->setItemData(mailComboBox->currentIndex(),
					mailComboBox->currentText() + " (" + tr("Not found") + ")", index);
}

QString MainConfigurationWindow::browserIndexToString(int browserIndex)
{
	switch (browserIndex)
	{
		case 0:  return "Specify path";
		case 1:  return "Konqueror";
		case 2:  return "Opera";
		case 3:  return "Opera (new tab)";
		case 4:  return "Mozilla";
		case 5:  return "SeaMonkey";
		case 6:  return "Mozilla Firefox";
		case 7:  return "Dillo";
		case 8:  return "Galeon";
		case 9:  return "Safari";
		case 10: return "Camino";
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
		case 5: return "Mail";
		default: return QString::null;
	}
}

void MainConfigurationWindow::onInfoPanelSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow)
{
	connect(syntaxEditorWindow->preview(), SIGNAL(needFixup(QString &)), Core::instance()->kaduWindow()->infoPanel(), SLOT(styleFixup(QString &)));
}

void MainConfigurationWindow::showLookChatAdvanced()
{
	if (!lookChatAdvanced)
	{
		lookChatAdvanced = new ConfigurationWindow("dialog-look-chat-advanced", tr("Advenced chat's look configuration"), InstanceDataManager);
		lookChatAdvanced->widget()->appendUiFile(dataPath("kadu/configuration/dialog-look-chat-advanced.ui"));

		connect(lookChatAdvanced->widget()->widgetById("removeServerTime"), SIGNAL(toggled(bool)), lookChatAdvanced->widget()->widgetById("maxTimeDifference"), SLOT(setEnabled(bool)));
		connect(lookChatAdvanced->widget()->widgetById("noHeaderRepeat"), SIGNAL(toggled(bool)), lookChatAdvanced->widget()->widgetById("noHeaderInterval"), SLOT(setEnabled(bool)));

		lookChatAdvanced->widget()->widgetById("chatSyntax")->setToolTip(qApp->translate("@default", SyntaxText));
		lookChatAdvanced->widget()->widgetById("conferencePrefix")->setToolTip(qApp->translate("@default", SyntaxText));
		lookChatAdvanced->widget()->widgetById("conferenceSyntax")->setToolTip(qApp->translate("@default", SyntaxText));

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
