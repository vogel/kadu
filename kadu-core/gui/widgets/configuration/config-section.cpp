/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

ConfigSection::ConfigSection(const QString &name, ConfigurationWidget *configurationWidget,
		QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget, const KaduIcon &icon) :
		QObject(configurationWidget), Name(name), MyConfigurationWidget(configurationWidget), Icon(icon),
		ListWidgetItem(listWidgetItem), Activated(false), ParentConfigGroupBoxWidget(parentConfigGroupBoxWidget)
{
	TabWidget = new KaduTabWidget(ParentConfigGroupBoxWidget);
	ParentConfigGroupBoxWidget->layout()->addWidget(TabWidget);
	TabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	TabWidget->hide();

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

ConfigSection::~ConfigSection()
{
	/* NOTE: It's needed to call ConfigurationWidget::configSectionDestroyed() before this
	 * ConfigSection will be destroyed. If we relied on QObject to send this signal,
	 * it'd be called after destroying all ConfigSection data but we need that data.
	 */
	blockSignals(false);
	emit destroyed(this);

	config_file.writeEntry("General", "ConfigurationWindow_" + MyConfigurationWidget->name() + '_' + Name,
			TabWidget->tabText(TabWidget->currentIndex()));

	// delete them here, since they manually delete child widgets of our TabWidget
	// qDeleteAll() won't work here because of connection to destroyed() signal
	foreach (const ConfigTab *ct, ConfigTabs)
	{
		disconnect(ct, SIGNAL(destroyed(QObject *)), this, SLOT(configTabDestroyed(QObject *)));
		delete ct;
	}

	delete ListWidgetItem;
	ListWidgetItem = 0;

	delete TabWidget;
	TabWidget = 0;
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
		return;

	QString tab = config_file.readEntry("General", "ConfigurationWindow_" + MyConfigurationWidget->name() + '_' + Name);
	if (ConfigTabs.contains(tab))
		TabWidget->setCurrentWidget(ConfigTabs.value(tab)->widget());
	Activated = true;
}

ConfigTab * ConfigSection::configTab(const QString &name, bool create)
{
	if (ConfigTabs.contains(name))
		return ConfigTabs.value(name);

	if (!create)
		return 0;

	ConfigTab *newConfigTab = new ConfigTab(name, this, TabWidget);
	ConfigTabs.insert(name, newConfigTab);
	connect(newConfigTab, SIGNAL(destroyed(QObject *)), this, SLOT(configTabDestroyed(QObject *)));

	TabWidget->addTab(newConfigTab->widget(), newConfigTab->name());

	TabWidget->setTabBarVisible(ConfigTabs.count() > 1);

	return newConfigTab;
}

void ConfigSection::configTabDestroyed(QObject *obj)
{
	// see ConfigTab::~ConfigTab()
	disconnect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(configTabDestroyed(QObject *)));

	QMap<QString, ConfigTab *>::iterator i = ConfigTabs.find(static_cast<ConfigTab *>(obj)->name());

	if (TabWidget)
		TabWidget->removeTab(TabWidget->indexOf((*i)->widget()));
	ConfigTabs.erase(i);

	TabWidget->setTabBarVisible(ConfigTabs.count() > 1);

	if (ConfigTabs.count() == 0)
		deleteLater();
}

void ConfigSection::iconThemeChanged()
{
	ListWidgetItem->setIcon(Icon.icon());
}
