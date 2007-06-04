/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlistbox.h>
#include <qobjectlist.h>
#include <qtabwidget.h>

#include "config_file.h"
#include "configuration_window_widgets.h"
#include "debug.h"
#include "emoticons.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"

#include "configuration_window.h"

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

ConfigurationWindow::ConfigurationWindow()
	: currentSection(0)
{
	setWFlags(getWFlags() || Qt::WDestructiveClose);

	QHBox *center = new QHBox(this);
	center->setMargin(10);
	center->setSpacing(10);
	center->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	left = new QVBox(center);
	left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	left->hide();

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

	loadConfiguration(this);
}

ConfigurationWindow::~ConfigurationWindow()
{
}

void ConfigurationWindow::show()
{
	if (!isShown())
	{
		sectionsListBox->setCurrentItem(0);
		loadConfiguration(this);
		QVBox::show();
	}
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
	else if (tagName == "action-button")
		widget = new ConfigActionButton(configGroupBox);
	else if (tagName == "select-file")
		widget = new ConfigSelectFile(configGroupBox);
	else if (tagName == "preview")
		widget = new ConfigPreview(configGroupBox);
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

	if (configSections.count() > 1)
		left->show();

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

#ifdef HAVE_OPENSSL
// 	ConfigDialog::addCheckBox("Network", "servergrid",
// 		QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
