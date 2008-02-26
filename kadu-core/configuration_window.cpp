/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <q3listbox.h>
#include <qobject.h>
#include <q3scrollview.h>
#include <qtabwidget.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3ValueList>
#include <QKeyEvent>
#include <Q3VBoxLayout>

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

	Q3ScrollView *scrollView;
	Q3VBoxLayout *mainLayout;
	QWidget *mainWidget;

public:
	ConfigTab(const QString &name, ConfigSection *configSection, QTabWidget *parentConfigGroupBoxWidget);
	~ConfigTab();

	ConfigGroupBox * configGroupBox(const QString &name, bool create = true);

	void removedConfigGroupBox(const QString &groupBoxName);

	QWidget *widget() { return mainWidget; }
	QWidget *tabWidget() { return scrollView; }

};

ConfigGroupBox::ConfigGroupBox(const QString &name, ConfigTab *configTab, Q3GroupBox *groupBox)
	: name(name), configTab(configTab), groupBox(groupBox)
{
	container = new QWidget(groupBox);

	gridLayout = new Q3GridLayout(container);
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
	return container->children().count() == 1;
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
	scrollView = new Q3ScrollView(parentConfigGroupBoxWidget);
	scrollView->setFrameStyle(Q3Frame::NoFrame);
	scrollView->setResizePolicy(Q3ScrollView::AutoOneFit);
	scrollView->setVScrollBarMode(Q3ScrollView::Auto);
	scrollView->setHScrollBarMode(Q3ScrollView::AlwaysOff);

	mainWidget = new QWidget(scrollView->viewport());
	scrollView->addChild(mainWidget);

	mainLayout = new Q3VBoxLayout(mainWidget, 10, 10);
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

	Q3GroupBox *groupBox = new Q3GroupBox(1, Qt::Horizontal, name, mainWidget);
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

ConfigSection::ConfigSection(const QString &name, ConfigurationWindow *configurationWindow, Q3ListBoxItem *listBoxItem, QWidget *parentConfigGroupBoxWidget,
		const QString &pixmap)
	: name(name), configurationWindow(configurationWindow), pixmap(pixmap), listBoxItem(listBoxItem), activated(false)
{
	mainWidget = new QTabWidget(parentConfigGroupBoxWidget);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->hide();

	connect(icons_manager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
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

	if (activated)
		return;

	QString tab = config_file.readEntry("General", "ConfigurationWindow_" + configurationWindow->name() + "_" + name);
	if (configTabs.contains(tab))
		mainWidget->setCurrentPage(mainWidget->indexOf(configTabs[tab]->tabWidget()));
	activated = true;
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
		configurationWindow->removedConfigSection(name);
// 		delete this;
	}
}

void ConfigSection::iconThemeChanged()
{
	Q3ListBox *listBox = listBoxItem->listBox();
	bool current = listBoxItem->isSelected();
	delete listBoxItem;

	listBoxItem = new Q3ListBoxPixmap(listBox, icons_manager->loadIcon(pixmap), name);
	if (current)
		listBox->setCurrentItem(listBoxItem);
}

ConfigurationWindow::ConfigurationWindow(const QString &name, const QString &caption)
	: Name(name), currentSection(0)
{
//	setWFlags(getWFlags() | Qt::WDestructiveClose);
	setCaption(caption);

	Q3HBox *center = new Q3HBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	left = new Q3VBox(center);
	left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	left->hide();

	container = new Q3HBox(center);
	container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	Q3HBox *buttons = new Q3HBox(this);
	buttons->setMargin(10);
	buttons->setSpacing(5);
	(new QWidget(buttons))->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	QPushButton *okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), buttons);
	QPushButton *applyButton = new QPushButton(icons_manager->loadIcon("ApplyWindowButton"), tr("Apply"), buttons);
	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), buttons);

	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	sectionsListBox = new Q3ListBox(left);
	sectionsListBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	sectionsListBox->setHScrollBarMode(Q3ScrollView::AlwaysOff);
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
		Q3VBox::show();
	}
	else
	{
		setActiveWindow();
		raise();
	}
}

Q3ValueList<ConfigWidget *> ConfigurationWindow::appendUiFile(const QString &fileName, bool load)
{
	Q3ValueList<ConfigWidget *> widgets = processUiFile(fileName);

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

Q3ValueList<ConfigWidget *>  ConfigurationWindow::processUiFile(const QString &fileName, bool append)
{
	kdebugf();

	Q3ValueList<ConfigWidget *> result;
	QFile file(fileName);

	QDomDocument uiFile;
	file.open(QIODevice::ReadOnly);

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

Q3ValueList<ConfigWidget *> ConfigurationWindow::processUiSectionFromDom(QDomNode sectionNode, bool append)
{
	kdebugf();

	Q3ValueList<ConfigWidget *> result;
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

Q3ValueList<ConfigWidget *> ConfigurationWindow::processUiTabFromDom(QDomNode tabNode, const QString &iconName,
	const QString &sectionName, bool append)
{
	kdebugf();

	Q3ValueList<ConfigWidget *> result;
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

Q3ValueList<ConfigWidget *> ConfigurationWindow::processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append)
{
	kdebugf();

	Q3ValueList<ConfigWidget *> result;
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

	FOREACH(child, configGroupBox->widget()->children())
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

	Q3ListBoxItem *newConfigSectionListBoxItem = new Q3ListBoxPixmap(sectionsListBox, icons_manager->loadIcon(pixmap), name);

	ConfigSection *newConfigSection = new ConfigSection(name, this, newConfigSectionListBoxItem, container, pixmap);
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

	const QObjectList children = object->children();
	FOREACH(child, children)
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

	const QObjectList children = object->children();
	FOREACH(child, children)
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
	newSection->activate();
}

void ConfigurationWindow::removedConfigSection(const QString &sectionName)
{
	configSections.remove(sectionName);

	for (unsigned int i = 0; i < sectionsListBox->count(); i++)
		if (sectionsListBox->item(i)->text() == tr(sectionName))
		{
			sectionsListBox->removeItem(i);
			break;
		}
}

void ConfigurationWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		Q3VBox::keyPressEvent(e);
}

#ifdef HAVE_OPENSSL
// 	ConfigDialog::addCheckBox("Network", "servergrid", QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
