/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QApplication>
#include <QtGui/QListWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>

#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-tab.h"
#include "gui/widgets/configuration/config-widget.h"

#include "icons-manager.h"

ConfigSection::ConfigSection(const QString &name, ConfigurationWidget *configurationWidget,
		QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget, const QString &iconPath) :
		Name(name), MyConfigurationWidget(configurationWidget), IconPath(iconPath),
		ListWidgetItem(listWidgetItem), Activated(false), ParentConfigGroupBoxWidget(parentConfigGroupBoxWidget)
{
	MainWidget = new QWidget(ParentConfigGroupBoxWidget);
	new QHBoxLayout(MainWidget);
	MainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	TabWidget = 0;

	ParentConfigGroupBoxWidget->layout()->addWidget(MainWidget);
	MainWidget->hide();

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

ConfigSection::~ConfigSection()
{
	if (TabWidget)
		config_file.writeEntry("General", "ConfigurationWindow_" + MyConfigurationWidget->name() + "_" + Name,
							   TabWidget->tabText(TabWidget->currentIndex()));
	delete MainWidget;
}

void ConfigSection::switchTabView(bool tabView)
{
	if (tabView == (0 != TabWidget))
		return;

	bool isVisible = MainWidget->isVisible();

	if (tabView)
	{
		TabWidget = new QTabWidget(ParentConfigGroupBoxWidget);
		ParentConfigGroupBoxWidget->layout()->addWidget(TabWidget);
		TabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		MainWidget->hide();
		foreach (ConfigTab *configTab, ConfigTabs)
			TabWidget->addTab(configTab->tabWidget(), configTab->name());

		MainWidget->deleteLater();
		MainWidget = TabWidget;
	}
	else
	{
		MainWidget = new QWidget(ParentConfigGroupBoxWidget);
		new QHBoxLayout(MainWidget);
		ParentConfigGroupBoxWidget->layout()->addWidget(MainWidget);
		MainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		foreach (ConfigTab *configTab, ConfigTabs)
			configTab->tabWidget()->setParent(MainWidget);

		TabWidget->deleteLater();
		TabWidget = 0;
	}

	MainWidget->setVisible(isVisible);
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
	ListWidgetItem->listWidget()->setCurrentItem(ListWidgetItem);

	if (Activated)
	{
		// #1400 workaround
		if (TabWidget && TabWidget->currentWidget())
		{
			QApplication::processEvents();
			TabWidget->currentWidget()->show();
		}
		return;
	}

	QString tab = config_file.readEntry("General", "ConfigurationWindow_" + MyConfigurationWidget->name() + "_" + Name);
	if (TabWidget && ConfigTabs.contains(tab))
		TabWidget->setCurrentWidget(ConfigTabs[tab]->tabWidget());
	Activated = true;
}

ConfigTab * ConfigSection::configTab(const QString &name, bool create)
{
	if (ConfigTabs.contains(name))
		return ConfigTabs[name];

	if (!create)
		return 0;

	switchTabView(ConfigTabs.size() >= 1);

	ConfigTab *newConfigTab = new ConfigTab(name, this, MainWidget);
	ConfigTabs[name] = newConfigTab;

	if (TabWidget)
		TabWidget->addTab(newConfigTab->scrollWidget(), newConfigTab->name());
	else
		MainWidget->layout()->addWidget(newConfigTab->scrollWidget());

	return newConfigTab;
}

void ConfigSection::removedConfigTab(const QString &configTabName)
{
	if (TabWidget)
		TabWidget->removeTab(TabWidget->indexOf(ConfigTabs[configTabName]->widget()));

	ConfigTabs.remove(configTabName);
	if (!ConfigTabs.count())
	{
		MyConfigurationWidget->removedConfigSection(Name);
// 		delete this;
	}

	switchTabView(ConfigTabs.size() > 1);
}

void ConfigSection::iconThemeChanged()
{
	QListWidget *listWidget = ListWidgetItem->listWidget();
	bool current = ListWidgetItem->isSelected();
	delete ListWidgetItem;

	ListWidgetItem = new QListWidgetItem(IconsManager::instance()->iconByPath(IconPath).pixmap(32, 32), Name, listWidget);
	if (current)
		listWidget->setCurrentItem(ListWidgetItem);
}
