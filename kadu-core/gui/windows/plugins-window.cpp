/*
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "QtGui/QGroupBox"
#include "QtGui/QHBoxLayout"
#include "QtGui/QLabel"
#include "QtGui/QLineEdit"
#include "QtGui/QPushButton"
#include "QtGui/QListView"
#include "QtGui/QTreeWidgetItem"
#include "QtGui/QVBoxLayout"
#include "QtGui/QWidget"
#include <QtGui/QKeyEvent>
#include <QtGui/QScrollBar>

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "activate.h"
#include "debug.h"
// #include "plugins/plugin-delegate.h"
// #include "plugins/plugins-model.h"

// #include "plugins/crap/kcategorizedview.h"
// #include "plugins/crap/kpluginselector.h"
#include "gui/widgets/plugin-list-view.h"
// #include "plugins/crap/kcategorydrawer.h"

#include "plugins-window.h"

PluginsWindow * PluginsWindow::Instance = 0;

PluginsWindow * PluginsWindow::instance()
{
	return Instance;
}

void PluginsWindow::show()
{
	if (!Instance)
		Instance = new PluginsWindow();

	_activateWindow(Instance);
}

PluginsWindow::PluginsWindow()
	: QWidget(0, Qt::Window), DesktopAwareObject(this),
	PluginsList(0), ModuleInfo(0)
{
	kdebugf();

	setWindowRole("kadu-modules");

	setWindowTitle(tr("Manage plugins"));
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(10);
	layout->setSpacing(10);

	PluginListView *PluginsList = new PluginListView(this);

	layout->addWidget(PluginsList);

	setLayout(layout);
	// end our QListView
/*
#ifndef Q_WS_MAEMO_5
	//our QVGroupBox
	QGroupBox *vgb_info = new QGroupBox(center);
	QVBoxLayout *infoLayout = new QVBoxLayout(vgb_info);
	vgb_info->setTitle(tr("Info"));
	//end our QGroupBox

	ModuleInfo = new QLabel(vgb_info);
	ModuleInfo->setText(tr("<b>Module:</b><br/><b>Depends on:</b><br/><b>Conflicts with:</b><br/><b>Provides:</b><br/><b>Author:</b><br/><b>Version:</b><br/><b>Description:</b>"));
#ifndef	Q_OS_MAC
	ModuleInfo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	ModuleInfo->setWordWrap(true);

	infoLayout->addWidget(ModuleInfo);
#endif*/

// // 	// buttons
// // 	QWidget *bottom = new QWidget(center);
// // 	QHBoxLayout *bottomLayout = new QHBoxLayout(bottom);
// // 	bottomLayout->setSpacing(5);
// // 
// // 	QPushButton *pb_close = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("&Close"), bottom);
// // 
// // 	bottomLayout->addStretch();
// // 	bottomLayout->addWidget(pb_close);
// // #ifdef Q_OS_MAC
// // 	bottom->setMaximumHeight(pb_close->height() + 5);
// // #endif
	// end buttons

// 	centerLayout->addWidget(ModulesList);
// 	centerLayout->setStretchFactor(ModulesList, 1);
// #ifndef Q_WS_MAEMO_5
// 	centerLayout->addWidget(vgb_info);
// #endif
// 	centerLayout->addWidget(bottom);
// 
// 	QHBoxLayout *layout = new QHBoxLayout(this);
// 	layout->addWidget(center);
// 
// 	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
//	connect(ModulesList, SIGNAL(itemSelectionChanged()), this, SLOT(itemsChanging()));
//	connect(ModulesList, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(moduleAction(QTreeWidgetItem *)));

	loadWindowGeometry(this, "General", "PluginsWindowGeometry", 0, 50, 600, 620);
//	refreshList();
//	ModulesList->sortByColumn(0, Qt::AscendingOrder);
	kdebugf2();
}

PluginsWindow::~PluginsWindow()
{
	kdebugf();
	Instance = 0;
 	saveWindowGeometry(this, "General", "PluginsWindowGeometry");
	kdebugf2();
}

// QTreeWidgetItem * PluginsWindow::getSelected()
// {
// 	return PluginsList->currentItem();
// }
// 
// // void PluginsWindow::itemsChanging()
// {
// 	if (PluginsList->currentItem())
// 		getInfo();
// }

// void PluginsWindow::moduleAction(QTreeWidgetItem *)
// {
// 	kdebugf();
// 
// 	QTreeWidgetItem *selectedItem = getSelected();
// 	if ((!selectedItem) || (selectedItem->text(0).isEmpty()))
// 		return;
// 
// 	if (!PluginsManager::instance()->plugins().contains(selectedItem->text(0)))
// 		return;
// 
// 	Plugin *plugin = PluginsManager::instance()->plugins().value(selectedItem->text(0));
// 
// 	if (plugin->isActive())
// 		unloadItemPlugin(plugin);
// 	else
// 		loadItemPlugin(plugin);
// 
// 	kdebugf2();
// }

void PluginsWindow::loadItemPlugin(Plugin *itemPlugin)
{
	PluginsManager::instance()->activatePlugin(itemPlugin, PluginActivationReasonUserRequest);

	// do it unconditionally as dependent plugins might have been loaded even if the requested one failed to do so
	//refreshList();

	ConfigurationManager::instance()->flush();
}

void PluginsWindow::unloadItemPlugin(Plugin *itemPlugin)
{
	if (PluginsManager::instance()->deactivatePlugin(itemPlugin, PluginDeactivationReasonUserRequest))
	{
	//	refreshList();

		ConfigurationManager::instance()->flush();
	}
}

// void PluginsWindow::refreshList()
// {
// 	kdebugf();
// 
// 	int vScrollValue = ModulesList->verticalScrollBar()->value();
// 
// 	QString s_selected;
// 
// 	QTreeWidgetItem *selectedItem = getSelected();
// 	if (selectedItem)
// 		s_selected = selectedItem->text(0);
// 
// 	ModulesList->clear();
// 
// 	foreach (Plugin *plugin, PluginsManager::instance()->plugins())
// 		if (plugin->isValid())
// 		{
// 			QStringList strings;
// 
// 			PluginInfo *pluginInfo = plugin->info();
// 			if (plugin->isActive())
// 				strings << plugin->name() << pluginInfo->version() << tr("Loaded");
// 			else
// 				strings << plugin->name() << pluginInfo->version() << tr("Not loaded");
// 			new QTreeWidgetItem(ModulesList, strings);
// 		}
// 
// 	ModulesList->resizeColumnToContents(0);
// // 	ModulesList->setSelected(ModulesList->findItem(s_selected, 0), true);
// 
// 	ModulesList->verticalScrollBar()->setValue(vScrollValue);
// 	kdebugf2();
// }

// void PluginsWindow::getInfo()
// {
// 	kdebugf();
// 
// 	QTreeWidgetItem *selected = getSelected();
// 	if (!selected)
// 		return;
// 
// 	if (!PluginsManager::instance()->plugins().contains(selected->text(0)))
// 		return;
// 
// 	PluginInfo *pluginInfo = PluginsManager::instance()->plugins().value(selected->text(0))->info();
// 
// 	if (!pluginInfo)
// 	{
// 		kdebugf2();
// 		return;
// 	}
// 
// #ifndef Q_WS_MAEMO_5
// 	ModuleInfo->setText(
// 		tr("<b>Module: </b>%1"
// 			"<br/><b>Depends on: </b>%2"
// 			"<br/><b>Conflicts with: </b>%3"
// 			"<br/><b>Provides: </b>%4"
// 			"<br/><b>Author: </b>%5"
// 			"<br/><b>Version: </b>%6"
// 			"<br/><b>Description: </b>%7")
// 			.arg(selected->text(0))
// 			.arg(pluginInfo->dependencies().join(", "))
// 			.arg(pluginInfo->conflicts().join(", "))
// 			.arg(pluginInfo->provides().join(", "))
// 			.arg(pluginInfo->author())
// 			.arg(pluginInfo->version())
// 			.arg(pluginInfo->description()));
// #endif
// 
// 	kdebugf2();
// }

void PluginsWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		event->accept();
		close();
	}
	else
		QWidget::keyPressEvent(event);
}
