#include <qstyle.h>
#include <qstylefactory.h>

#include "config_file.h"
#include "configuration_window_widgets.h"
#include "debug.h"
#include "emoticons.h"
#include "icons_manager.h"
#include "kadu.h"

#include "main_configuration_window.h"

MainConfigurationWindow::MainConfigurationWindow()
	: ConfigurationWindow(), lookChatAdvanced(0), lookUserboxBackground(0)
{
	import_0_5_0_configuration();

	appendUiFile(dataPath("kadu/configuration/dialog.ui"));

	onStartupSetLastDescription = dynamic_cast<QCheckBox *>(widgetById("onStartupSetLastDescription"));
	QLineEdit *disconnectDescription = dynamic_cast<QLineEdit *>(widgetById("disconnectDescription"));
	QLineEdit *onStartupSetDescription = dynamic_cast<QLineEdit *>(widgetById("onStartupSetDescription"));
	disconnectDescription->setMaxLength(GG_STATUS_DESCR_MAXSIZE);
	onStartupSetDescription->setMaxLength(GG_STATUS_DESCR_MAXSIZE);

	connect(widgetById("disconnectWithCurrentDescription"), SIGNAL(toggled(bool)), disconnectDescription, SLOT(setDisabled(bool)));
	connect(onStartupSetLastDescription, SIGNAL(toggled(bool)), onStartupSetDescription, SLOT(setDisabled(bool)));
	connect(widgetById("removeServerTime"), SIGNAL(toggled(bool)), widgetById("maxTimeDifference"), SLOT(setEnabled(bool)));
	connect(widgetById("receiveImages"), SIGNAL(toggled(bool)), widgetById("receiveImagesDuringInvisibility"), SLOT(setEnabled(bool)));
	connect(widgetById("startupStatus"), SIGNAL(activated(int)), this, SLOT(onChangeStartupStatus(int)));
	connect(widgetById("showDescription"), SIGNAL(toggled(bool)), widgetById("multilineDescription"), SLOT(setEnabled(bool)));
	connect(widgetById("chatPrune"), SIGNAL(toggled(bool)), widgetById("chatPruneLen"), SLOT(setEnabled(bool)));
	connect(widgetById("foldLink"), SIGNAL(toggled(bool)), widgetById("linkFoldTreshold"), SLOT(setEnabled(bool)));
	connect(widgetById("chatCloseTimer"), SIGNAL(toggled(bool)), widgetById("chatCloseTimerPeriod"), SLOT(setEnabled(bool)));
	connect(widgetById("useDefaultServers"), SIGNAL(toggled(bool)), widgetById("serverList"), SLOT(setDisabled(bool)));

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
	connect(widgetById("lookUserboxBackground"), SIGNAL(clicked()), this, SLOT(showLookUserboxBackground()));

// 	connect(widgetById("iconPaths"), SIGNAL(changed()), this, SLOT(setIconThemes()));

	loadGeometry(this, "General", "ConfigGeometry", 0, 30, 790, 480);
}

MainConfigurationWindow::~MainConfigurationWindow()
{
	saveGeometry(this, "General", "ConfigGeometry");
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

void MainConfigurationWindow::import_0_5_0_configuration()
{
// 	config_file.removeVariable("General", "ShowAnonymousWithMsgs");

	int maxImageSize = config_file.readNumEntry("Chat", "MaxImageSize", -1);
	if (maxImageSize != -1)
		config_file.writeEntry("Chat", "ReceiveImages", maxImageSize != 0);
	config_file.removeVariable("Chat", "MaxImageSize");

	int defaultStatusIndex = config_file.readNumEntry("General", "DefaultStatusIndex", -1);
	if (defaultStatusIndex != -1)
	{
		QString startupStatus;
		switch (defaultStatusIndex)
		{
			case 0:
			case 1: startupStatus = "Online";
			        break;
			case 2:
			case 3: startupStatus = "Busy";
			        break;
			case 4:
			case 5: startupStatus = "Invisible";
			        break;
			case 6: startupStatus = "Offline";
			        break;
			case 7:
			case 8: startupStatus = "LastStatus";
			        break;
		}
		config_file.writeEntry("General", "StartupStaus", startupStatus);
	}
	config_file.removeVariable("General", "DefaultStatusIndex");

	QString infoPanelSyntax = config_file.readEntry("Look", "PanelContents", "nothing");
	if (infoPanelSyntax != "nothing")
	{
		config_file.writeEntry("Look", "InfoPanelSyntaxFile", "custom");
		SyntaxList infoPanelList("infopanel");
		infoPanelList.updateSyntax("custom", infoPanelSyntax);
	}
	config_file.removeVariable("Look", "PanelContents");

	if (config_file.readBoolEntry("Look", "MultiColumnUserbox", false))
	{
		int columns = (kadu->userbox()->width() - 20) / config_file.readNumEntry("Look", "MultiColumnUserboxWidth", (kadu->userbox()->width() - 20));
		config_file.writeEntry("Look", "UserBoxColumnCount", columns);
	}
	config_file.removeVariable("Look", "MultiColumnUserbox");
	config_file.removeVariable("Look", "MultiColumnUserboxWidth");
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
	ConfigComboBox *emoticonThemes = dynamic_cast<ConfigComboBox *>(widgetById("emoticonThemes"));

	QStringList themes = emoticons->themes();

	emoticonThemes->setItems(themes, themes);
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

void MainConfigurationWindow::onChangeBrowser(int index)
{
	QStringList searchPath = QStringList::split(":", QString(getenv("PATH")));
	QStringList executableName;
	QStringList options;

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

			options << tr("Open in new window") << tr("Open in new tab") << tr("Open in background tab");
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

			options << tr("Open in new window") << tr("Open in new tab");
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

			options << tr("Open in new window") << tr("Open in new tab");
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

void MainConfigurationWindow::showLookChatAdvanced()
{
	if (!lookChatAdvanced)
	{
		lookChatAdvanced = new ConfigurationWindow();
		lookChatAdvanced->appendUiFile(dataPath("kadu/configuration/dialog-look-chat-advanced.ui"));

		connect(lookChatAdvanced, SIGNAL(configurationUpdated()), this, SIGNAL(configurationUpdated()));
		connect(lookChatAdvanced, SIGNAL(destroyed()), this, SLOT(lookChatAdvancedDestroyed()));
	}

	lookChatAdvanced->show();
}

void MainConfigurationWindow::lookChatAdvancedDestroyed()
{
	lookChatAdvanced = 0;
}

void MainConfigurationWindow::showLookUserboxBackground()
{
	if (!lookUserboxBackground)
	{
		lookUserboxBackground = new ConfigurationWindow();
		lookUserboxBackground->appendUiFile(dataPath("kadu/configuration/dialog-look-userbox-background.ui"));

		QWidget *userboxBackgroundMove = lookUserboxBackground->widgetById("userboxBackgroundMove");
		connect(userboxBackgroundMove, SIGNAL(toggled(bool)), lookUserboxBackground->widgetById("userboxBackgroundSX"), SLOT(setEnabled(bool)));
		connect(userboxBackgroundMove, SIGNAL(toggled(bool)), lookUserboxBackground->widgetById("userboxBackgroundSY"), SLOT(setEnabled(bool)));
		connect(userboxBackgroundMove, SIGNAL(toggled(bool)), lookUserboxBackground->widgetById("userboxBackgroundSW"), SLOT(setEnabled(bool)));
		connect(userboxBackgroundMove, SIGNAL(toggled(bool)), lookUserboxBackground->widgetById("userboxBackgroundSH"), SLOT(setEnabled(bool)));

		connect(lookUserboxBackground, SIGNAL(configurationUpdated()), this, SIGNAL(configurationUpdated()));
		connect(lookUserboxBackground, SIGNAL(destroyed()), this, SLOT(lookUserboxBackgroundDestroyed()));
	}

	lookUserboxBackground->show();
}

void MainConfigurationWindow::lookUserboxBackgroundDestroyed()
{
	lookUserboxBackground = 0;
}
