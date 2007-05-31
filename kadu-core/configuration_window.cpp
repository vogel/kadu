/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qdom.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"

#include "configuration_window.h"

ConfigurationWindow *configuration_window = 0;

class ConfigGroupBox
{
	QGroupBox *groupBox;
	QWidget *container;
	QGridLayout *gridLayout;

public:
	ConfigGroupBox(QGroupBox *groupBox);
	~ConfigGroupBox();

	QWidget * widget() { return container; }
	QGridLayout * layout() { return gridLayout; }

	bool empty();

};

class ConfigTab
{
	QMap<QString, ConfigGroupBox *> configGroupBoxes;

	QWidget *mainWidget;
	QVBoxLayout *mainLayout;

public:
	ConfigTab(QTabWidget *parentConfigGroupBoxWidget, const QString &name);
	~ConfigTab();

	ConfigGroupBox * configGroupBox(const QString &name);

	bool empty();

};

class ConfigSection
{
	QListBoxItem *listBoxItem;
	QMap<QString, ConfigTab *> configTabs;

	QTabWidget *mainWidget;

	ConfigTab *configTab(const QString &name);

public:
	ConfigSection(QListBoxItem *listBoxItem, QWidget *parentConfigGroupBoxWidget);
	~ConfigSection();

	void show() { mainWidget->show(); mainWidget->setCurrentPage(0); }
	void hide() { mainWidget->hide(); }

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox);

	bool empty();

};

ConfigGroupBox::ConfigGroupBox(QGroupBox *groupBox)
	: groupBox(groupBox)
{
	container = new QWidget(groupBox);

	gridLayout = new QGridLayout(container);
	gridLayout->setAutoAdd(false);
	gridLayout->setSpacing(5);
	gridLayout->setColStretch(1, 100);
}

ConfigGroupBox::~ConfigGroupBox()
{
}

bool ConfigGroupBox::empty()
{
	return false;
}

ConfigTab::ConfigTab(QTabWidget *parentConfigGroupBoxWidget, const QString &name)
{
	mainWidget = new QWidget(parentConfigGroupBoxWidget);
	mainLayout = new QVBoxLayout(mainWidget, 10, 10);
	mainLayout->addStretch(1);

	parentConfigGroupBoxWidget->addTab(mainWidget, QObject::tr(name));
}

ConfigTab::~ConfigTab()
{
	delete mainWidget;
}

ConfigGroupBox *ConfigTab::configGroupBox(const QString &name)
{
	if (configGroupBoxes.contains(name))
		return configGroupBoxes[name];

	QGroupBox *groupBox = new QGroupBox(1, Qt::Horizontal, QObject::tr(name), mainWidget);
	groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	mainLayout->insertWidget(configGroupBoxes.count(), groupBox);

	ConfigGroupBox *newConfigGroupBox = new ConfigGroupBox(groupBox);
	configGroupBoxes[name] = newConfigGroupBox;

	return newConfigGroupBox;
}

bool ConfigTab::empty()
{
	return false;
}

ConfigSection::ConfigSection(QListBoxItem *listBoxItem, QWidget *parentConfigGroupBoxWidget)
	: listBoxItem(listBoxItem)
{
	mainWidget = new QTabWidget(parentConfigGroupBoxWidget);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->hide();
}

ConfigSection::~ConfigSection()
{
	delete mainWidget;
}

ConfigGroupBox * ConfigSection::configGroupBox(const QString &tab, const QString &groupBox)
{
	return configTab(tab)->configGroupBox(groupBox);
}

ConfigTab *ConfigSection::configTab(const QString &name)
{
	if (configTabs.contains(name))
		return configTabs[name];

	ConfigTab *newConfigTab = new ConfigTab(mainWidget, name);
	configTabs[name] = newConfigTab;

	return newConfigTab;
}

bool ConfigSection::empty()
{
	return false;
}

void ConfigurationWindow::initModule()
{
	configuration_window = new ConfigurationWindow();
}

void ConfigurationWindow::closeModule()
{
	delete configuration_window;
	configuration_window = 0;
}

ConfigurationWindow::ConfigurationWindow()
	: currentSection(0)
{
	import_0_5_0_configuration();

	QHBox *center = new QHBox(this);
	center->setMargin(10);
	center->setSpacing(10);
	center->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBox *left = new QVBox(center);
	left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	container = new QHBox(center);
	container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QHBox *buttons = new QHBox(this);
	buttons->setMargin(10);
	buttons->setSpacing(5);
	(new QWidget(buttons))->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	QPushButton *okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), buttons);
	QPushButton *applyButton = new QPushButton(icons_manager->loadIcon("ApplyWindowButton"), tr("Apply"), buttons);
	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), buttons);

	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	sectionsListBox = new QListBox(left);
	sectionsListBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	connect(sectionsListBox, SIGNAL(highlighted(const QString &)), this, SLOT(changeSection(const QString &)));

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
	(dynamic_cast<QLineEdit *>(widgetById("proxyPassword")))->setEchoMode(QLineEdit::Password);

	QWidget *showInformationPanel = widgetById("showInformationPanel");
	connect(showInformationPanel, SIGNAL(toggled(bool)), widgetById("showVerticalScrollbar"), SLOT(setEnabled(bool)));
	connect(showInformationPanel, SIGNAL(toggled(bool)), widgetById("showEmoticonsInPanel"), SLOT(setEnabled(bool)));

	browserComboBox = dynamic_cast<ConfigComboBox *>(widgetById("browser"));
// 	browserOptionComboBox = dynamic_cast<ConfigComboBox *>(widgetById("browserOption"));
	browserCommandLineEdit = dynamic_cast<ConfigLineEdit *>(widgetById("browserPath"));
	connect(browserComboBox, SIGNAL(activated(int)), this, SLOT(onChangeBrowser(int)));
// 	connect(browserOptionComboBox, SIGNAL(activated(int)), this, SLOT(onChangeBrowserOption(int)));

	mailComboBox = dynamic_cast<ConfigComboBox *>(widgetById("mail"));
	mailCommandLineEdit = dynamic_cast<ConfigLineEdit *>(widgetById("mailPath"));
	connect(mailComboBox, SIGNAL(activated(int)), this, SLOT(onChangeMail(int)));

// 	connect(widgetById("iconPaths"), SIGNAL(changed()), this, SLOT(setIconThemes()));

	loadGeometry(this, "General", "ConfigGeometry", 0, 30, 790, 480);
	loadConfiguration(this);
}

ConfigurationWindow::~ConfigurationWindow()
{
	saveGeometry(this, "General", "ConfigGeometry");
}

void ConfigurationWindow::show()
{
	if (!isShown())
	{
		setLanguages();
		setQtThemes();
		setIconThemes();
		setEmoticonThemes();
		setToolTipClasses();

		sectionsListBox->setCurrentItem(0);
		loadConfiguration(this);
		QVBox::show();
	}
}

void ConfigurationWindow::import_0_5_0_configuration()
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

void ConfigurationWindow::onChangeStartupStatus(int index)
{
	onStartupSetLastDescription->setEnabled(index != 4);
	widgetById("onStartupSetDescription")->setEnabled(!onStartupSetLastDescription->isChecked() && index != 4);
}

void ConfigurationWindow::setLanguages()
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

void ConfigurationWindow::setQtThemes()
{
	ConfigComboBox *qtThemes = dynamic_cast<ConfigComboBox *>(widgetById("qtThemes"));

	QStringList themes = QStyleFactory::keys();
	QString currentStyle = QApplication::style().name();
	if (!themes.contains(currentStyle))
		themes.append(currentStyle);

	qtThemes->setItems(themes, themes);
	qtThemes->setCurrentText(currentStyle);
}

void ConfigurationWindow::setIconThemes()
{
	ConfigComboBox *iconThemes = dynamic_cast<ConfigComboBox *>(widgetById("iconThemes"));

	QStringList themes = icons_manager->themes();

	iconThemes->setItems(themes, themes);
	iconThemes->setCurrentText(icons_manager->theme());
}

void ConfigurationWindow::setEmoticonThemes()
{
	ConfigComboBox *emoticonThemes = dynamic_cast<ConfigComboBox *>(widgetById("emoticonThemes"));

	QStringList themes = emoticons->themes();

	emoticonThemes->setItems(themes, themes);
}

void ConfigurationWindow::setToolTipClasses()
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

QString ConfigurationWindow::findExecutable(const QStringList &paths, const QStringList &executableNames)
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

void ConfigurationWindow::onChangeBrowser(int index)
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

// void ConfigurationWindow::onChangeBrowserOption(int index)
// {
// }

void ConfigurationWindow::onChangeMail(int index)
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

void ConfigurationWindow::appendUiFile(const QString &fileName)
{
	kdebugf();

	QFile file(fileName);

	QDomDocument uiFile;
	file.open(IO_ReadOnly);

	if (!uiFile.setContent(&file))
	{
		kdebugf2();
		file.close();
		return;
	}

	file.close();

	QDomElement kaduConfigurationUi = uiFile.documentElement();
	if (kaduConfigurationUi.tagName() != "configuration-ui")
	{
		kdebugf2();
		return;
	}

	QDomNodeList children = kaduConfigurationUi.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		appendUiSectionFromDom(children.item(i));

	kdebugf2();
}

void ConfigurationWindow::appendUiSectionFromDom(QDomNode sectionNode)
{
	kdebugf();

	if (!sectionNode.isElement())
	{
		kdebugf2();
		return;
	}

	const QDomElement &sectionElement = sectionNode.toElement();
	if (sectionElement.tagName() != "section")
	{
		kdebugf2();
		return;
	}

	const QString &sectionName = sectionElement.attribute("name");
	if (sectionName.isEmpty())
	{
		kdebugf2();
		return;
	}

	const QDomNodeList children = sectionElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		appendUiTabFromDom(children.item(i), sectionName);

	kdebugf2();
}

void ConfigurationWindow::appendUiTabFromDom(QDomNode tabNode, const QString &sectionName)
{
	kdebugf();

	if (!tabNode.isElement())
	{
		kdebugf2();
		return;
	}

	const QDomElement &tabElement = tabNode.toElement();
	if (tabElement.tagName() != "tab")
	{
		kdebugf2();
		return;
	}

	const QString tabName = tabElement.attribute("name");
	if (tabName.isEmpty())
	{
		kdebugf2();
		return;
	}

	const QDomNodeList &children = tabElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		appendUiGroupBoxFromDom(children.item(i), sectionName, tabName);

	kdebugf2();
}

void ConfigurationWindow::appendUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName)
{
	kdebugf();

	if (!groupBoxNode.isElement())
	{
		kdebugf2();
		return;
	}

	const QDomElement &groupBoxElement = groupBoxNode.toElement();
	if (groupBoxElement.tagName() != "group-box")
	{
		kdebugf2();
		return;
	}

	const QString groupBoxName = groupBoxElement.attribute("name");
	if (groupBoxName.isEmpty())
	{
		kdebugf2();
		return;
	}

	ConfigGroupBox *configGroupBoxWidget = configGroupBox(sectionName, tabName, groupBoxName);

	const QDomNodeList &children = groupBoxElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		appendUiElementFromDom(children.item(i), configGroupBoxWidget);

	kdebugf2();
}

void ConfigurationWindow::appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
	kdebugf();

	if (!uiElementNode.isElement())
	{
		kdebugf2();
		return;
	}

	const QDomElement &uiElement = uiElementNode.toElement();
	const QString &tagName = uiElement.tagName();
	ConfigWidget *widget = 0;

	if (tagName == "line-edit")
		widget = new ConfigLineEdit(configGroupBox);
	else if (tagName == "gg-password-edit")
		widget = new ConfigGGPasswordEdit(configGroupBox);
	else if (tagName == "check-box")
		widget = new ConfigCheckBox(configGroupBox);
	else if (tagName == "spin-box")
		widget = new ConfigSpinBox(configGroupBox);
	else if (tagName == "combo-box")
		widget = new ConfigComboBox(configGroupBox);
	else if (tagName == "hot-key-edit")
		widget = new ConfigHotKeyEdit(configGroupBox);
	else if (tagName == "path-list-edit")
		widget = new ConfigPathListEdit(configGroupBox);
	else if (tagName == "color-button")
		widget = new ConfigColorButton(configGroupBox);
	else if (tagName == "select-font")
		widget = new ConfigSelectFont(configGroupBox);
	else if (tagName == "syntax-editor")
		widget = new ConfigSyntaxEditor(configGroupBox);
	else
	{
		kdebugf2();
		return;
	}

	if (!widget->fromDomElement(uiElement))
	{
		delete widget;
		kdebugf2();
		return;
	}

	QString id = uiElement.attribute("id");
	if (!id.isEmpty())
		widgets[id] = dynamic_cast<QWidget *>(widget);

	kdebugf2();
}

QWidget * ConfigurationWindow::widgetById(const QString &id)
{
	if (widgets.contains(id))
		return widgets[id];

	return 0;
}

ConfigGroupBox * ConfigurationWindow::configGroupBox(const QString &section, const QString &tab, const QString &groupBox)
{
	return configSection(section)->configGroupBox(tab, groupBox);
}

ConfigSection *ConfigurationWindow::configSection(const QString &name)
{
	if (configSections.contains(name))
		return configSections[name];

	QListBoxItem *newConfigSectionListBoxItem = new QListBoxText(sectionsListBox, tr(name));

	ConfigSection *newConfigSection = new ConfigSection(newConfigSectionListBoxItem, container);
	configSections[name] = newConfigSection;

	return newConfigSection;
}

void ConfigurationWindow::loadConfiguration(QObject *object)
{
	kdebugf();

	if (!object)
		return;

	const QObjectList *children = object->children();
	if (children)
		FOREACH(child, *children)
			loadConfiguration(*child);

	ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
	if (configWidget)
		configWidget->loadConfiguration();
}

void ConfigurationWindow::saveConfiguration(QObject *object)
{
	kdebugf();

	if (!object)
		return;

	const QObjectList *children = object->children();
	if (children)
		FOREACH(child, *children)
			saveConfiguration(*child);

	ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
	if (configWidget)
		configWidget->saveConfiguration();
}

void ConfigurationWindow::updateAndCloseConfig()
{
	updateConfig();
	close();
}

void ConfigurationWindow::updateConfig()
{
	saveConfiguration(this);
	emit configurationUpdated();
}

void ConfigurationWindow::toggleConfigurationWindow()
{
	if (isVisible())
	{
		hide();
	}
	else
	{
		loadConfiguration(this);
		show();
	}
}

void ConfigurationWindow::changeSection(const QString &newSectionName)
{
	if (!configSections.contains(newSectionName))
		return;

	ConfigSection *newSection = configSections[newSectionName];
	if (newSection == currentSection)
		return;

	if (currentSection)
		currentSection->hide();

	currentSection = newSection;
	newSection->show();
}

ConfigWidget::ConfigWidget(ConfigGroupBox *parentConfigGroupBox)
	: parentConfigGroupBox(parentConfigGroupBox)
{
}

bool ConfigWidget::fromDomElement(QDomElement domElement)
{
	createWidgets();
	return true;
}

ConfigWidgetValue::ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox)
	: ConfigWidget(parentConfigGroupBox)
{
}

ConfigWidgetValue::ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox, const QString &widgetCaption, const QString &section, const QString &item)
	: ConfigWidget(parentConfigGroupBox), widgetCaption(widgetCaption), section(section), item(item)
{
}

bool ConfigWidgetValue::fromDomElement(QDomElement domElement)
{
	widgetCaption = domElement.attribute("caption");
	section = domElement.attribute("config-section");
	item = domElement.attribute("config-item");

	if (widgetCaption.isEmpty() || section.isEmpty() || item.isEmpty())
		return false;

	return ConfigWidget::fromDomElement(domElement);
}

ConfigLineEdit::ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigLineEdit::ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigLineEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigLineEdit::loadConfiguration()
{
	setText(config_file.readEntry(section, item));
}

void ConfigLineEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, text());
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: ConfigLineEdit(section, item, widgetCaption, parentConfigGroupBox, name)
{
	setEchoMode(QLineEdit::Password);
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: ConfigLineEdit(parentConfigGroupBox, name)
{
	setEchoMode(QLineEdit::Password);
}

void ConfigGGPasswordEdit::loadConfiguration()
{
	setText(pwHash(config_file.readEntry(section, item)));
}

void ConfigGGPasswordEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, pwHash(text()));
}

ConfigCheckBox::ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QCheckBox(widgetCaption, parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigCheckBox::ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QCheckBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigCheckBox::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();

	setText(widgetCaption);

	int numRows = layout->numRows();
	layout->addMultiCellWidget(this, numRows, numRows, 0, 1);
}

void ConfigCheckBox::loadConfiguration()
{
	setChecked(config_file.readBoolEntry(section, item));
	emit toggled(isChecked());
}

void ConfigCheckBox::saveConfiguration()
{
	config_file.writeEntry(section, item, isChecked());
}

ConfigSpinBox::ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption,
		int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(minValue, maxValue, step, parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
}

ConfigSpinBox::ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigSpinBox::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigSpinBox::loadConfiguration()
{
	setValue(config_file.readNumEntry(section, item));
}

void ConfigSpinBox::saveConfiguration()
{
	config_file.writeEntry(section, item, value());
}


bool ConfigSpinBox::fromDomElement(QDomElement domElement)
{
	QString minValue = domElement.attribute("min-value");
	QString maxValue = domElement.attribute("max-value");
	QString step = domElement.attribute("step");
	setSuffix(domElement.attribute("suffix"));

	bool ok;

	setMinValue(minValue.toInt(&ok));
	if (!ok)
		return false;

	setMaxValue(maxValue.toInt(&ok));
	if (!ok)
		return false;

	setLineStep(step.toInt(&ok));
	if (!ok)
		return false;

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigComboBox::ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QStringList &itemValues,
	const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigComboBox::ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigComboBox::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	insertStringList(itemCaptions);
}

void ConfigComboBox::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, widgetCaption + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);

	clear();
	insertStringList(itemCaptions);
}

void ConfigComboBox::loadConfiguration()
{
	QString currentValue = config_file.readEntry(section, item);
	setCurrentItem(itemValues.findIndex(currentValue));

	emit activated(currentItem());
}

void ConfigComboBox::saveConfiguration()
{
	config_file.writeEntry(section, item, itemValues[currentItem()]);
}

bool ConfigComboBox::fromDomElement(QDomElement domElement)
{
	QDomNodeList children = domElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
	{
		QDomNode node = children.item(i);
		if (node.isElement())
		{
			QDomElement element = node.toElement();
			if (element.tagName() != "item")
				continue;

			itemValues.append(element.attribute("value"));
			itemCaptions.append(element.attribute("caption"));

			insertItem(tr(element.attribute("caption")));
		}
	}

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigHotKeyEdit::ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigHotKeyEdit::ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigHotKeyEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigHotKeyEdit::loadConfiguration()
{
	setShortCut(config_file.readEntry(section, item));
}

void ConfigHotKeyEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, shortCutString());
}

ConfigPathListEdit::ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: PathListEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigPathListEdit::ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: PathListEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigPathListEdit::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigPathListEdit::loadConfiguration()
{
	setPathList(QStringList::split(QRegExp("(;|:)"), config_file.readEntry(section, item)));
}

void ConfigPathListEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, pathList().join(":"));
}

ConfigColorButton::ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigColorButton::ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigColorButton::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigColorButton::loadConfiguration()
{
	setColor(config_file.readColorEntry(section, item));
}

void ConfigColorButton::saveConfiguration()
{
	config_file.writeEntry(section, item, color());
}

ConfigSelectFont::ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFont(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigSelectFont::ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SelectFont(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigSelectFont::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight);
	layout->addWidget(this, numRows, 1);
}

void ConfigSelectFont::loadConfiguration()
{
	setFont(config_file.readFontEntry(section, item));
}

void ConfigSelectFont::saveConfiguration()
{
	config_file.writeEntry(section, item, font());
}

ConfigSyntaxEditor::ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox, widgetCaption, section, item)
{
	createWidgets();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigSyntaxEditor::createWidgets()
{
	kdebugf();

	QGridLayout *layout = parentConfigGroupBox->layout();
	int numRows = layout->numRows();

	QLabel *label = new QLabel(this, tr(widgetCaption) + ":", parentConfigGroupBox->widget());

	layout->addWidget(label, numRows, 0, Qt::AlignRight | Qt::AlignTop);
	layout->addWidget(this, numRows, 1);
}

void ConfigSyntaxEditor::loadConfiguration()
{
	setCurrentSyntax(config_file.readEntry(section, item));
}

void ConfigSyntaxEditor::saveConfiguration()
{
	config_file.writeEntry(section, item, currentSyntax());
}

bool ConfigSyntaxEditor::fromDomElement(QDomElement domElement)
{
	QString category = domElement.attribute("category");
	if (category.isEmpty())
		return false;

	setCategory(category);

	return ConfigWidgetValue::fromDomElement(domElement);
}

#ifdef HAVE_OPENSSL
// 	ConfigDialog::addCheckBox("Network", "servergrid",
// 		QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
