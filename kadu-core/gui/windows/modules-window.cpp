/*
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
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
#include "QtGui/QPushButton"
#include "QtGui/QTreeWidget"
#include "QtGui/QTreeWidgetItem"
#include "QtGui/QVBoxLayout"
#include "QtGui/QWidget"
#include <QtGui/QKeyEvent>
#include <QtGui/QScrollBar>

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "misc/misc.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugins-manager.h"
#include "icons-manager.h"
#include "debug.h"

#include "modules-window.h"

ModulesWindow::ModulesWindow(QWidget *parent)
	: QWidget(parent, Qt::Window),
	lv_modules(0), l_moduleinfo(0)
{
	kdebugf();

	setWindowRole("kadu-modules");

	setWindowTitle(tr("Manage Modules"));
	setAttribute(Qt::WA_DeleteOnClose);

#ifndef Q_WS_MAEMO_5
	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget(this);
	QVBoxLayout *leftLayout = new QVBoxLayout(left);
	leftLayout->setMargin(10);
	leftLayout->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	leftLayout->addWidget(l_icon);
	leftLayout->addStretch();
#endif

	QWidget *center = new QWidget(this);
	QVBoxLayout *centerLayout = new QVBoxLayout(center);
	centerLayout->setMargin(10);
	centerLayout->setSpacing(10);

#ifndef Q_WS_MAEMO_5
	QLabel *l_info = new QLabel(center);
	l_icon->setPixmap(IconsManager::instance()->iconByPath("kadu_icons/plugins").pixmap(32, 32));
	l_info->setText(tr("This dialog box allows you to manage installed modules. Modules are responsible "
			"for numerous vital features like playing sounds or message encryption.\n"
			"You can load (or unload) them by double-clicking on their names."));
	l_info->setWordWrap(true);
#ifndef	Q_OS_MAC
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif /* Q_OS_MAC */
#endif /* Q_WS_MAEMO_5 */
	// end create main QLabel widgets (icon and app info)

	// our QListView
	lv_modules = new QTreeWidget(center);
	QStringList headers;
	headers << tr("Module name") << tr("Version") << tr("Module type") << tr("State");
	lv_modules->setHeaderLabels(headers);
	lv_modules->setSortingEnabled(true);
	lv_modules->setAllColumnsShowFocus(true);
	lv_modules->setIndentation(false);
	lv_modules->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));


	// end our QListView

#ifndef Q_WS_MAEMO_5
	//our QVGroupBox
	QGroupBox *vgb_info = new QGroupBox(center);
	QVBoxLayout *infoLayout = new QVBoxLayout(vgb_info);
	vgb_info->setTitle(tr("Info"));
	//end our QGroupBox

	l_moduleinfo = new QLabel(vgb_info);
	l_moduleinfo->setText(tr("<b>Module:</b><br/><b>Depends on:</b><br/><b>Conflicts with:</b><br/><b>Provides:</b><br/><b>Author:</b><br/><b>Version:</b><br/><b>Description:</b>"));
#ifndef	Q_OS_MAC
	l_moduleinfo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	l_moduleinfo->setWordWrap(true);

	infoLayout->addWidget(l_moduleinfo);
#endif

	// buttons
	QWidget *bottom = new QWidget(center);
	QHBoxLayout *bottomLayout = new QHBoxLayout(bottom);
	bottomLayout->setSpacing(5);

	QPushButton *pb_close = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("&Close"), bottom);

	bottomLayout->addStretch();
	bottomLayout->addWidget(pb_close);
#ifdef Q_OS_MAC
	bottom->setMaximumHeight(pb_close->height() + 5);
#endif
	// end buttons

#ifndef Q_WS_MAEMO_5
	centerLayout->addWidget(l_info);
#endif
	centerLayout->addWidget(lv_modules);
	centerLayout->setStretchFactor(lv_modules, 1);
#ifndef Q_WS_MAEMO_5
	centerLayout->addWidget(vgb_info);
#endif
	centerLayout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
#ifndef Q_WS_MAEMO_5
	layout->addWidget(left);
#endif
	layout->addWidget(center);

	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(lv_modules, SIGNAL(itemSelectionChanged()), this, SLOT(itemsChanging()));
	connect(lv_modules, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(moduleAction(QTreeWidgetItem *)));

	loadWindowGeometry(this, "General", "ModulesWindowGeometry", 0, 50, 600, 620);
	refreshList();
	lv_modules->sortByColumn(0, Qt::AscendingOrder);
	kdebugf2();
}

ModulesWindow::~ModulesWindow()
{
	kdebugf();
 	saveWindowGeometry(this, "General", "ModulesWindowGeometry");
	kdebugf2();
}

QTreeWidgetItem * ModulesWindow::getSelected()
{
	if (lv_modules->selectedItems().count())
		return lv_modules->selectedItems().at(0);
	else
		return 0;
}

void ModulesWindow::itemsChanging()
{
	if (lv_modules->selectedItems().count())
		getInfo();
}

void ModulesWindow::moduleAction(QTreeWidgetItem *)
{
	kdebugf();

	QTreeWidgetItem *selectedItem = getSelected();
	if (!selectedItem)
		return;

	// TODO: OH LOL
	if ((selectedItem->text(2) == tr("Dynamic")) && (selectedItem->text(3) == tr("Loaded")))
		unloadItem(selectedItem->text(0));
	else
		if ((selectedItem->text(2) == tr("Dynamic")) && (selectedItem->text(3) == tr("Not loaded")))
			loadItem(selectedItem->text(0));

	kdebugf2();
}

void ModulesWindow::loadItem(const QString &item)
{
	PluginsManager::instance()->activatePlugin(item);
	refreshList();

	ConfigurationManager::instance()->flush();
}

void ModulesWindow::unloadItem(const QString &item)
{
	PluginsManager::instance()->deactivatePlugin(item, true, false);
	refreshList();

	ConfigurationManager::instance()->flush();
}

void ModulesWindow::refreshList()
{
	kdebugf();

	int vScrollValue = lv_modules->verticalScrollBar()->value();

	QString s_selected;

	QTreeWidgetItem *selectedItem = getSelected();
	if (selectedItem)
		s_selected = selectedItem->text(0);

	lv_modules->clear();

	foreach (Plugin *plugin, PluginsManager::instance()->plugins())
		if (plugin->isValid())
		{
			QStringList strings;

			PluginInfo *pluginInfo = plugin->info();
			if (plugin->isActive())
				strings << plugin->name() << pluginInfo->version() << tr("Dynamic") << tr("Loaded");
			else
				strings << plugin->name() << pluginInfo->version() << tr("Dynamic") << tr("Not loaded");
			new QTreeWidgetItem(lv_modules, strings);
		}

	lv_modules->resizeColumnToContents(0);
// 	lv_modules->setSelected(lv_modules->findItem(s_selected, 0), true);

	lv_modules->verticalScrollBar()->setValue(vScrollValue);
	kdebugf2();
}

void ModulesWindow::getInfo()
{
	kdebugf();

	QTreeWidgetItem *selected = getSelected();
	if (!selected)
		return;

	if (PluginsManager::instance()->plugins().contains(selected->text(0)))
		return;

	PluginInfo *pluginInfo = PluginsManager::instance()->plugins().value(selected->text(0))->info();

	if (!pluginInfo)
	{
		kdebugf2();
		return;
	}

#ifndef Q_WS_MAEMO_5
	l_moduleinfo->setText(
		tr("<b>Module: </b>%1"
			"<br/><b>Depends on: </b>%2"
			"<br/><b>Conflicts with: </b>%3"
			"<br/><b>Provides: </b>%4"
			"<br/><b>Author: </b>%5"
			"<br/><b>Version: </b>%6"
			"<br/><b>Description: </b>%7")
			.arg(selected->text(0))
			.arg(pluginInfo->dependencies().join(", "))
			.arg(pluginInfo->conflicts().join(", "))
			.arg(pluginInfo->provides().join(", "))
			.arg(pluginInfo->author())
			.arg(pluginInfo->version())
			.arg(pluginInfo->description()));
#endif

	kdebugf2();
}

void ModulesWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		event->accept();
		close();
	}
	else
		QWidget::keyPressEvent(event);
}
