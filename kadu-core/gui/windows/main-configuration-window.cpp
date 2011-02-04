/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Kermit (plaza.maciej@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtGui/QApplication>
#include <QtGui/QStyleFactory>

#include "configuration/config-file-data-manager.h"

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/buddy.h"
#include "buddies/buddy-shared.h"
#include "chat/chat-styles-manager.h"
#include "chat/message/message-render-info.h"
#include "chat/style-engines/chat-style-engine.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "emoticons/emoticons-manager.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/configuration/buddy-list-background-colors-widget.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-line-edit.h"
#include "gui/widgets/configuration/config-preview.h"
#include "gui/widgets/configuration/config-syntax-editor.h"
#include "gui/widgets/configuration/config-path-list-edit.h"
#include "gui/widgets/configuration/config-check-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/syntax-editor-window.h"
#include "misc/misc.h"
#include "status/status.h"
#include "themes/icon-theme-manager.h"
#include "themes/emoticon-theme-manager.h"

#include "debug.h"
#include "icons-manager.h"
#include "languages-manager.h"

#include "main-configuration-window.h"

MainConfigurationWindow *MainConfigurationWindow::Instance = 0;
ConfigFileDataManager *MainConfigurationWindow::InstanceDataManager = 0;
QList<QString> MainConfigurationWindow::UiFiles;
QList<ConfigurationUiHandler *> MainConfigurationWindow::ConfigurationUiHandlers;

const char *MainConfigurationWindow::SyntaxText = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size, %z - gender (0/1/2)\n"
);

const char *MainConfigurationWindow::SyntaxTextNotify = QT_TRANSLATE_NOOP
(
	"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
	"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
	"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size, %z - gender (0/1/2),\n"
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

bool MainConfigurationWindow::hasInstance()
{
	return Instance;
}

ConfigFileDataManager * MainConfigurationWindow::instanceDataManager()
{
	if (!InstanceDataManager)
		InstanceDataManager = new ConfigFileDataManager();

	return InstanceDataManager;
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
	// TODO: move this to separate class, like ChatStylesConfigurationUiHandler
	// and just register it here
	ChatStylesManager::instance()->mainConfigurationWindowCreated(Instance);
	foreach (const QString &uiFile, UiFiles)
		Instance->widget()->appendUiFile(uiFile);

	foreach (ConfigurationUiHandler *uiHandler, ConfigurationUiHandlers)
		if (uiHandler)
			uiHandler->mainConfigurationWindowCreated(Instance);
}

MainConfigurationWindow::MainConfigurationWindow() :
		ConfigurationWindow("MainConfiguration", tr("Kadu configuration"), "General", instanceDataManager()), lookChatAdvanced(0)
{
	setWindowRole("kadu-configuration");

	widget()->appendUiFile(dataPath("kadu/configuration/dialog.ui"));

#ifndef DEBUG_ENABLED
	((QWidget *)(widget()->widgetById("debug")->parent()))->hide();
#endif

#ifndef Q_OS_WIN
	((QWidget *)(widget()->widgetById("startup")))->hide();
#endif

#ifndef Q_WS_X11
	((QWidget *)(widget()->widgetById("windowActivationMethodGroup")))->hide();
	((QWidget *)(widget()->widgetById("notify/fullscreenSilentMode")))->hide();
#endif

	onStartupSetLastDescription = static_cast<QCheckBox *>(widget()->widgetById("onStartupSetLastDescription"));
	QLineEdit *disconnectDescription = static_cast<QLineEdit *>(widget()->widgetById("disconnectDescription"));
	QLineEdit *onStartupSetDescription = static_cast<QLineEdit *>(widget()->widgetById("onStartupSetDescription"));

	Account account = AccountManager::instance()->defaultAccount();
	if (!account.isNull() && account.protocolHandler())
	{
		disconnectDescription->setMaxLength(account.data()->maxDescriptionLength());
		onStartupSetDescription->setMaxLength(account.data()->maxDescriptionLength());
	}
//	connect(widget()->widgetById("advancedMode"), SIGNAL(toggled(bool)), widget()->widgetById("contactsWithIcons"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("showAvatars"), SIGNAL(toggled(bool)), widget()->widgetById("avatarBorder"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("showAvatars"), SIGNAL(toggled(bool)), widget()->widgetById("avatarGreyOut"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("disconnectWithCurrentDescription"), SIGNAL(toggled(bool)), disconnectDescription, SLOT(setDisabled(bool)));
	connect(onStartupSetLastDescription, SIGNAL(toggled(bool)), onStartupSetDescription, SLOT(setDisabled(bool)));
	connect(widget()->widgetById("foldLink"), SIGNAL(toggled(bool)), widget()->widgetById("linkFoldTreshold"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("chatPrune"), SIGNAL(toggled(bool)), widget()->widgetById("chatPruneLen"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("chatCloseTimer"), SIGNAL(toggled(bool)), widget()->widgetById("chatCloseTimerPeriod"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("startupStatus"), SIGNAL(activated(int)), this, SLOT(onChangeStartupStatus(int)));
	connect(widget()->widgetById("infoPanelBgFilled"), SIGNAL(toggled(bool)), widget()->widgetById("infoPanelBgColor"), SLOT(setEnabled(bool)));
	connect(widget()->widgetById("showDescription"), SIGNAL(toggled(bool)), widget()->widgetById("multilineDescription"), SLOT(setEnabled(bool)));
//	connect(widget()->widgetById("useDefaultServers"), SIGNAL(toggled(bool)), widget()->widgetById("serverList"), SLOT(setDisabled(bool)));
	connect(widget()->widgetById("openChatOnMessage"), SIGNAL(toggled(bool)), widget()->widgetById("openChatOnMessageWhenOnline"), SLOT(setEnabled(bool)));

	connect(widget()->widgetById("displayGroupTabs"), SIGNAL(toggled(bool)), widget()->widgetById("showGroupAll"), SLOT(setEnabled(bool)));

	emoticonsStyleComboBox = static_cast<ConfigComboBox *>(widget()->widgetById("emoticonsStyle"));
	emoticonsThemeComboBox = static_cast<ConfigComboBox *>(widget()->widgetById("emoticonsTheme"));
	emoticonsScalingComboBox = static_cast<ConfigComboBox *>(widget()->widgetById("emoticonsScaling"));
	connect(emoticonsThemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeEmoticonsTheme(int)));
	connect(widget()->widgetById("emoticonsPaths"), SIGNAL(changed()), this, SLOT(setEmoticonThemes()));

	QWidget *showInformationPanel = widget()->widgetById("showInformationPanel");
	connect(showInformationPanel, SIGNAL(toggled(bool)), widget()->widgetById("showVerticalScrollbar"), SLOT(setEnabled(bool)));
	connect(showInformationPanel, SIGNAL(toggled(bool)), widget()->widgetById("showEmoticonsInPanel"), SLOT(setEnabled(bool)));

	ConfigCheckBox *useDefaultBrowserCheckbox = static_cast<ConfigCheckBox *>(widget()->widgetById("useDefaultBrowser"));
	ConfigLineEdit *browserCommandLineEdit = static_cast<ConfigLineEdit *>(widget()->widgetById("browserPath"));
	connect(useDefaultBrowserCheckbox, SIGNAL(toggled(bool)), browserCommandLineEdit, SLOT(setDisabled(bool)));

	ConfigCheckBox *useDefaultEMailCheckbox = static_cast<ConfigCheckBox *>(widget()->widgetById("useDefaultEMail"));
	ConfigLineEdit *mailCommandLineEdit = static_cast<ConfigLineEdit *>(widget()->widgetById("mailPath"));
	connect(useDefaultEMailCheckbox, SIGNAL(toggled(bool)), mailCommandLineEdit, SLOT(setDisabled(bool)));

	connect(widget()->widgetById("lookChatAdvanced"), SIGNAL(clicked()), this, SLOT(showLookChatAdvanced()));

	Preview *infoPanelSyntaxPreview = static_cast<Preview *>(widget()->widgetById("infoPanelSyntaxPreview"));
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
	(static_cast<ConfigSyntaxEditor *>(widget()->widgetById("infoPanelSyntax")))->setSyntaxHint(qApp->translate("@default", SyntaxText));

	userboxTransparency = static_cast<QCheckBox *>(widget()->widgetById("userboxTransparency"));
	userboxAlpha = static_cast<QSlider *>(widget()->widgetById("userboxAlpha"));
	connect(userboxTransparency, SIGNAL(toggled(bool)), widget()->widgetById("userboxAlpha"), SLOT(setEnabled(bool)));

	buddyColors = new BuddyListBackgroundColorsWidget(this);

	triggerCompositingStateChanged();
}

MainConfigurationWindow::~MainConfigurationWindow()
{
	Instance = 0;
}

void MainConfigurationWindow::compositingEnabled()
{
	userboxTransparency->setEnabled(true);
	if (userboxTransparency->isChecked())
		userboxAlpha->setEnabled(true);
}

void MainConfigurationWindow::compositingDisabled()
{
	userboxTransparency->setEnabled(false);
	userboxAlpha->setEnabled(false);
}

void MainConfigurationWindow::show()
{
	if (!isVisible())
	{
		setLanguages();
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
	ConfigComboBox *languages = static_cast<ConfigComboBox *>(widget()->widgetById("languages"));

	languages->setItems(LanguagesManager::languageValues(), LanguagesManager::languageNames());
}

void MainConfigurationWindow::setIconThemes()
{
	ConfigComboBox *iconThemes = static_cast<ConfigComboBox *>(widget()->widgetById("iconThemes"));
	IconsManager::instance()->themeManager()->loadThemes((static_cast<PathListEdit *>(widget()->widgetById("iconPaths")))->pathList());

	(void)QT_TRANSLATE_NOOP("@default", "default");
	QList<Theme> themes = IconsManager::instance()->themeManager()->themes();

	QStringList values;
	QStringList captions;
	foreach (const Theme &theme, themes)
	{
		values.append(theme.path());
		captions.append(qApp->translate("@default", theme.name().toAscii().data()));
	}

	iconThemes->setItems(values, captions);
	iconThemes->setCurrentItem(IconsManager::instance()->themeManager()->currentTheme().path());
}

void MainConfigurationWindow::setEmoticonThemes()
{
	ConfigComboBox *emoticonsThemes = static_cast<ConfigComboBox *>(widget()->widgetById("emoticonsTheme"));
	EmoticonsManager::instance()->themeManager()->loadThemes((static_cast<PathListEdit *>(widget()->widgetById("emoticonsPaths")))->pathList());

	(void)QT_TRANSLATE_NOOP("@default", "default");
	QList<Theme> themes = EmoticonsManager::instance()->themeManager()->themes();

	QStringList values;
	QStringList captions;
	foreach (const Theme &theme, themes)
	{
		values.append(theme.path());
		captions.append(qApp->translate("@default", theme.name().toAscii().data()));
	}

	emoticonsThemes->setItems(values, captions);
	emoticonsThemes->setCurrentItem(EmoticonsManager::instance()->themeManager()->currentTheme().path());
}

void MainConfigurationWindow::setToolTipClasses()
{
	QStringList captions;
	QStringList values;
	captions << tr("None");
	values << QString();

	QStringList toolTipClasses = ToolTipClassManager::instance()->getToolTipClasses();
	foreach(const QString &toolTipClass, toolTipClasses)
	{
		captions << qApp->translate("@default", toolTipClass.toAscii().data());
		values << toolTipClass;
	}

	static_cast<ConfigComboBox *>(widget()->widgetById("toolTipClasses"))->setItems(values, captions);
}

void MainConfigurationWindow::onChangeEmoticonsTheme(int index)
{
	emoticonsStyleComboBox->setEnabled(index != 0);
	emoticonsScalingComboBox->setEnabled(index != 0);
}

void MainConfigurationWindow::onInfoPanelSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow)
{
	connect(syntaxEditorWindow->preview(), SIGNAL(needFixup(QString &)), Core::instance()->kaduWindow()->infoPanel(), SLOT(styleFixup(QString &)));
}

void MainConfigurationWindow::showLookChatAdvanced()
{
	if (!lookChatAdvanced)
	{
		lookChatAdvanced = new ConfigurationWindow("LookChatAdvanced", tr("Advenced chat's look configuration"), "General", instanceDataManager());
		lookChatAdvanced->widget()->appendUiFile(dataPath("kadu/configuration/dialog-look-chat-advanced.ui"));

		connect(lookChatAdvanced->widget()->widgetById("removeServerTime"), SIGNAL(toggled(bool)), lookChatAdvanced->widget()->widgetById("maxTimeDifference"), SLOT(setEnabled(bool)));
		connect(lookChatAdvanced->widget()->widgetById("noHeaderRepeat"), SIGNAL(toggled(bool)), lookChatAdvanced->widget()->widgetById("noHeaderInterval"), SLOT(setEnabled(bool)));

		lookChatAdvanced->widget()->widgetById("chatSyntax")->setToolTip(qApp->translate("@default", SyntaxText));
		lookChatAdvanced->widget()->widgetById("conferencePrefix")->setToolTip(qApp->translate("@default", SyntaxText));
		lookChatAdvanced->widget()->widgetById("conferenceSyntax")->setToolTip(qApp->translate("@default", SyntaxText));

		connect(lookChatAdvanced, SIGNAL(destroyed()), this, SLOT(lookChatAdvancedDestroyed()));

		connect(ChatStylesManager::instance(), SIGNAL(previewSyntaxChanged(QString)), this, SLOT(chatPreviewSyntaxChanged(QString)));
		if (ChatStylesManager::instance()->syntaxListCombo())
			chatPreviewSyntaxChanged(ChatStylesManager::instance()->syntaxListCombo()->currentText());
	}

	lookChatAdvanced->show();
}

void MainConfigurationWindow::lookChatAdvancedDestroyed()
{
	lookChatAdvanced = 0;
}

void MainConfigurationWindow::chatPreviewSyntaxChanged(const QString &syntaxName)
{
	if (!lookChatAdvanced)
		return;

	StyleInfo styleInfo = ChatStylesManager::instance()->chatStyleInfo(syntaxName);
	if (!styleInfo.engine)
	{
		lookChatAdvanced->deleteLater();
		return;
	}

	bool enableKaduFeatures = styleInfo.engine->engineName() == "Kadu";

	lookChatAdvanced->widget()->widgetById("chatHeaderSeparatorsHeight")->setEnabled(enableKaduFeatures);
	lookChatAdvanced->widget()->widgetById("messageSeparatorsHeight")->setEnabled(enableKaduFeatures);
	lookChatAdvanced->widget()->widgetById("removeServerTime")->setEnabled(enableKaduFeatures);
	lookChatAdvanced->widget()->widgetById("maxTimeDifference")->setEnabled(enableKaduFeatures);
}
