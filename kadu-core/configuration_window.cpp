/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qlistbox.h>
#include <qobjectlist.h>
#include <qscrollview.h>
#include <qtabwidget.h>

#include "config_file.h"
#include "configuration_aware_object.h"
#include "configuration_window_widgets.h"
#include "debug.h"
#include "emoticons.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"

#include "configuration_window.h"

class ConfigSection;

class ConfigTab
{
	QString name;
	ConfigSection *configSection;

	QMap<QString, ConfigGroupBox *> configGroupBoxes;

	QScrollView *scrollView;
	QVBoxLayout *mainLayout;
	QWidget *mainWidget;

public:
	ConfigTab(const QString &name, ConfigSection *configSection, QTabWidget *parentConfigGroupBoxWidget);
	~ConfigTab();

	ConfigGroupBox * configGroupBox(const QString &name, bool create = true);

	void removedConfigGroupBox(const QString &groupBoxName);

	QWidget *widget() { return mainWidget; }

};

class ConfigSection
{
	QString name;
	ConfigurationWindow *configurationWindow;

	QListBoxItem *listBoxItem;
	QMap<QString, ConfigTab *> configTabs;

	QTabWidget *mainWidget;

	ConfigTab *configTab(const QString &name, bool create = true);

public:
	ConfigSection(const QString &name, ConfigurationWindow *configurationWindow, QListBoxItem *listBoxItem, QWidget *parentConfigGroupBoxWidget);
	~ConfigSection();

	void activate();

	void show() { mainWidget->show(); }
	void hide() { mainWidget->hide(); }

	ConfigGroupBox * configGroupBox(const QString &tab, const QString &groupBox, bool create = true);

	void removedConfigTab(const QString &configTabName);

};

ConfigGroupBox::ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox)
	: name(name), configTab(configTab), groupBox(groupBox)
{
	container = new QWidget(groupBox);

	gridLayout = new QGridLayout(container);
	gridLayout->setAutoAdd(false);
	gridLayout->setSpacing(5);
	gridLayout->setColStretch(1, 100);
}

ConfigGroupBox::~ConfigGroupBox()
{
	delete groupBox;

	configTab->removedConfigGroupBox(name);
}

bool ConfigGroupBox::empty()
{
	return container->children()->count() == 1;
}

void ConfigGroupBox::addWidget(QWidget *widget, bool fullSpace)
{
	int numRows = gridLayout->numRows();

	if (fullSpace)
		gridLayout->addMultiCellWidget(widget, numRows, numRows, 0, 1);
	else
		gridLayout->addWidget(widget, numRows, 1);
}

void ConfigGroupBox::addWidgets(QWidget *widget1, QWidget *widget2)
{
	int numRows = gridLayout->numRows();

	if (widget1)
		gridLayout->addWidget(widget1, numRows, 0, Qt::AlignRight);

	if (widget2)
		gridLayout->addWidget(widget2, numRows, 1);
}

ConfigTab::ConfigTab(const QString &name, ConfigSection *configSection, QTabWidget *parentConfigGroupBoxWidget)
	: name(name), configSection(configSection)
{
	scrollView = new QScrollView(parentConfigGroupBoxWidget);
	scrollView->setFrameStyle(QFrame::NoFrame);
	scrollView->setResizePolicy(QScrollView::AutoOneFit);
	scrollView->setVScrollBarMode(QScrollView::Auto);
	scrollView->setHScrollBarMode(QScrollView::AlwaysOff);

	mainWidget = new QWidget(scrollView->viewport());
	scrollView->addChild(mainWidget);

	mainLayout = new QVBoxLayout(mainWidget, 10, 10);
	mainLayout->addStretch(1);

	parentConfigGroupBoxWidget->addTab(scrollView, name);
}

ConfigTab::~ConfigTab()
{
	delete scrollView;
}

ConfigGroupBox *ConfigTab::configGroupBox(const QString &name, bool create)
{
	if (configGroupBoxes.contains(name))
		return configGroupBoxes[name];

	if (!create)
		return 0;

	QGroupBox *groupBox = new QGroupBox(1, Qt::Horizontal, name, mainWidget);
	groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	mainLayout->insertWidget(configGroupBoxes.count(), groupBox);

	ConfigGroupBox *newConfigGroupBox = new ConfigGroupBox(name, this, groupBox);
	configGroupBoxes[name] = newConfigGroupBox;

	groupBox->show();

	return newConfigGroupBox;
}

void ConfigTab::removedConfigGroupBox(const QString &groupBoxName)
{
	configGroupBoxes.remove(groupBoxName);

	if (!configGroupBoxes.count())
	{
		configSection->removedConfigTab(name);
		delete this;
	}
}

ConfigSection::ConfigSection(const QString &name, ConfigurationWindow *configurationWindow, QListBoxItem *listBoxItem, QWidget *parentConfigGroupBoxWidget)
	: name(name), configurationWindow(configurationWindow), listBoxItem(listBoxItem)
{
	mainWidget = new QTabWidget(parentConfigGroupBoxWidget);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->hide();
}

ConfigSection::~ConfigSection()
{
	config_file.writeEntry("General", "ConfigurationWindow_" + configurationWindow->name() + "_" + name,
		mainWidget->label(mainWidget->currentPageIndex()));
	delete mainWidget;
}

ConfigGroupBox * ConfigSection::configGroupBox(const QString &tab, const QString &groupBox, bool create)
{
	ConfigTab *ct = configTab(tab, create);
	if (!ct)
		return 0;

	return ct->configGroupBox(groupBox, create);
}

void ConfigSection::activate()
{
	listBoxItem->listBox()->setCurrentItem(listBoxItem);

	QString tab = config_file.readEntry("General", "ConfigurationWindow_" + configurationWindow->name() + "_" + name);
	if (configTabs.contains(tab))
		mainWidget->showPage(configTabs[tab]->widget());
}

ConfigTab *ConfigSection::configTab(const QString &name, bool create)
{
	if (configTabs.contains(name))
		return configTabs[name];

	if (!create)
		return 0;

	ConfigTab *newConfigTab = new ConfigTab(name, this, mainWidget);
	configTabs[name] = newConfigTab;

	return newConfigTab;
}

void ConfigSection::removedConfigTab(const QString &configTabName)
{
	mainWidget->removePage(configTabs[configTabName]->widget());

	configTabs.remove(configTabName);
	if (!configTabs.count())
	{
		delete this;
		configurationWindow->removedConfigSection(name);
	}
}

ConfigurationWindow::ConfigurationWindow(const QString &name, const QString &caption)
	: Name(name), currentSection(0)
{
	setWFlags(getWFlags() | Qt::WDestructiveClose);
	setCaption(tr(caption));

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
}

ConfigurationWindow::~ConfigurationWindow()
{
	config_file.writeEntry("General", "ConfigurationWindow_" + Name, sectionsListBox->currentText());

	FOREACH(configSection, configSections)
		delete *configSection;
}

void ConfigurationWindow::show()
{
	if (!isShown())
	{
		QString lastSection = config_file.readEntry("General", "ConfigurationWindow_" + Name);
		if (configSections.contains(lastSection))
			configSections[lastSection]->activate();
		else
			sectionsListBox->setCurrentItem(0);

		loadConfiguration(this);
		QVBox::show();
	}
	else
	{
		setActiveWindow();
		raise();
	}
}

QValueList<ConfigWidget *> ConfigurationWindow::appendUiFile(const QString &fileName, bool load)
{
	QValueList<ConfigWidget *> widgets = processUiFile(fileName);

	if (load)
		FOREACH(widget, widgets)
			if (*widget)
				(*widget)->loadConfiguration();

	return widgets;
}

void ConfigurationWindow::removeUiFile(const QString &fileName)
{
	processUiFile(fileName, false);
}

QValueList<ConfigWidget *>  ConfigurationWindow::processUiFile(const QString &fileName, bool append)
{
	kdebugf();

	QValueList<ConfigWidget *> result;
	QFile file(fileName);

	QDomDocument uiFile;
	file.open(IO_ReadOnly);

	if (!uiFile.setContent(&file))
	{
		kdebugf2();
		file.close();
		return result;
	}

	file.close();

	QDomElement kaduConfigurationUi = uiFile.documentElement();
	if (kaduConfigurationUi.tagName() != "configuration-ui")
	{
		kdebugf2();
		return result;
	}

	QDomNodeList children = kaduConfigurationUi.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiSectionFromDom(children.item(i), append);

	kdebugf2();
	return result;
}

QValueList<ConfigWidget *> ConfigurationWindow::processUiSectionFromDom(QDomNode sectionNode, bool append)
{
	kdebugf();

	QValueList<ConfigWidget *> result;
	if (!sectionNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &sectionElement = sectionNode.toElement();
	if (sectionElement.tagName() != "section")
	{
		kdebugf2();
		return result;
	}

	const QString &iconName = sectionElement.attribute("icon");

	const QString &sectionName = sectionElement.attribute("name");
	if (sectionName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	configSection(iconName, qApp->translate("@default", sectionName), true);

	const QDomNodeList children = sectionElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiTabFromDom(children.item(i), iconName, sectionName, append);

	kdebugf2();
	return result;
}

QValueList<ConfigWidget *> ConfigurationWindow::processUiTabFromDom(QDomNode tabNode, const QString &iconName,
	const QString &sectionName, bool append)
{
	kdebugf();

	QValueList<ConfigWidget *> result;
	if (!tabNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &tabElement = tabNode.toElement();
	if (tabElement.tagName() != "tab")
	{
		kdebugf2();
		return result;
	}

	const QString tabName = tabElement.attribute("name");
	if (tabName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	const QDomNodeList &children = tabElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiGroupBoxFromDom(children.item(i), sectionName, tabName, append);

	kdebugf2();
	return result;
}

QValueList<ConfigWidget *> ConfigurationWindow::processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append)
{
	kdebugf();

	QValueList<ConfigWidget *> result;
	if (!groupBoxNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &groupBoxElement = groupBoxNode.toElement();
	if (groupBoxElement.tagName() != "group-box")
	{
		kdebugf2();
		return result;
	}

	const QString groupBoxName = groupBoxElement.attribute("name");
	if (groupBoxName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	const QString groupBoxId = groupBoxElement.attribute("id");

	ConfigGroupBox *configGroupBoxWidget = configGroupBox(sectionName, tabName, groupBoxName, append);
	if (!configGroupBoxWidget)
	{
		kdebugf2();
		return result;
	}

	if (!groupBoxId.isEmpty())
		widgets[groupBoxId] = dynamic_cast<QWidget *>(configGroupBoxWidget->widget());

	const QDomNodeList &children = groupBoxElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		if (append)
			result.append(appendUiElementFromDom(children.item(i), configGroupBoxWidget));
		else
			removeUiElementFromDom(children.item(i), configGroupBoxWidget);

	kdebugf2();
	return result;
}

ConfigWidget * ConfigurationWindow::appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
	kdebugf();

	if (!uiElementNode.isElement())
	{
		kdebugf2();
		return 0;
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
	else if (tagName == "slider")
		widget = new ConfigSlider(configGroupBox);
	else if (tagName == "label")
		widget = new ConfigLabel(configGroupBox);
	else if (tagName == "list-box")
		widget = new ConfigListBox(configGroupBox);
	else
	{
		kdebugf2();
		return 0;
	}

	if (!widget->fromDomElement(uiElement))
	{
		delete widget;
		kdebugf2();
		return 0;
	}

	QString id = uiElement.attribute("id");
	if (!id.isEmpty())
		widgets[id] = dynamic_cast<QWidget *>(widget);

	widget->show();

	kdebugf2();
	return widget;
}

void ConfigurationWindow::removeUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
	kdebugf();

	if (!uiElementNode.isElement())
	{
		kdebugf2();
		return;
	}

	const QDomElement &uiElement = uiElementNode.toElement();
	const QString &caption = uiElement.attribute("caption");

	FOREACH(child, *configGroupBox->widget()->children())
	{
		ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(*child);
		if (!configWidget)
			continue;

		if (configWidget->widgetCaption == caption)
		{
			delete configWidget;
			break;
		}
	}

	if (configGroupBox->empty())
		delete configGroupBox;

	kdebugf2();
}

QWidget * ConfigurationWindow::widgetById(const QString &id)
{
	if (widgets.contains(id))
		return widgets[id];

	return 0;
}

ConfigGroupBox * ConfigurationWindow::configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create)
{
	ConfigSection *s = configSection(qApp->translate("@default", section));
	if (!s)
		return 0;

	return s->configGroupBox(qApp->translate("@default", tab), qApp->translate("@default", groupBox), create);
}

ConfigSection *ConfigurationWindow::configSection(const QString &name)
{
	return configSections[name];
}

ConfigSection *ConfigurationWindow::configSection(const QString &pixmap, const QString &name, bool create)
{
	if (configSections.contains(name))
		return configSections[name];

	if (!create)
		return 0;

	QListBoxItem *newConfigSectionListBoxItem = new QListBoxPixmap(sectionsListBox, icons_manager->loadIcon(pixmap), name);

	ConfigSection *newConfigSection = new ConfigSection(name, this, newConfigSectionListBoxItem, container);
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
	emit configurationWindowApplied();
	saveConfiguration(this);

	ConfigurationAwareObject::notifyAll();
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

void ConfigurationWindow::removedConfigSection(const QString &sectionName)
{
	// TODO: finish it
//	configSections.remove(sectionName);
// 	sectionsListBox->remove(tr(sectionName));
}

void ConfigurationWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Key_Escape)
	{
		e->accept();
		deleteLater();
	}
	else
		QVBox::keyPressEvent(e);
}

#ifdef HAVE_OPENSSL
// 	ConfigDialog::addCheckBox("Network", "servergrid", QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
