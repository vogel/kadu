/*
 * %kadu copyright begin%
 * Copyright 2010 Przemys�aw Rudy (prudy1@o2.pl)
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
 *
 * Objectives:
 * - simple view hides everything from the main window except the contact list,
 *   which can be still used to contact buddies. For any other action
 *   switching back to the normal view is required
 * - quickly accessible switching mechanism between views
 */
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QTabBar>
#include <QtGui/QSplitter>

#include <stdio.h>

#include "core/core.h"
#include "configuration/configuration-file.h"
#include "misc/path-conversion.h"
#include "gui/hot-key.h"

#include "modules/docking/docking.h"
#include "simpleview-config-ui.h"
#include "simpleview.h"

SimpleView *SimpleView::Instance = 0;

SimpleView::SimpleView() :
	SimpleViewActive(false)
{
	SimpleViewConfigUi::createInstance();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/simpleview.ui"));
	MainConfigurationWindow::registerUiHandler(SimpleViewConfigUi::instance());

	DockAction = DockingManager::instance()->dockMenu()->addAction(IconsManager::instance()->iconByPath("16x16/view-refresh.png"), tr("Simple view"), this, SLOT(simpleViewToggle()));
	DockAction->setShortcut(HotKey::shortCutFromFile("ShortCuts", "kadu_simpleview"));

	kaduWindow = Core::instance()->kaduWindow();
	mainWindow = kaduWindow->findMainWindow(kaduWindow);
	buddiesListView = dynamic_cast<BuddiesListView *>(mainWindow->contactsListView());
	buddiesListWidget = dynamic_cast<BuddiesListWidget *>(buddiesListView->parent());
	groupTabBar = kaduWindow->findChild<GroupTabBar *>();
	statusButton = kaduWindow->findChild<StatusButtons *>();
}

SimpleView::~SimpleView()
{
	if (SimpleViewActive)
		simpleViewToggle();

	DockingManager::instance()->dockMenu()->removeAction(DockAction);
	delete DockAction;

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/simpleview.ui"));
	MainConfigurationWindow::unregisterUiHandler(SimpleViewConfigUi::instance());
	SimpleViewConfigUi::destroyInstance();
}

void SimpleView::createInstance()
{
	if (!Instance)
		Instance = new SimpleView();
}

void SimpleView::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

void SimpleView::simpleViewToggle()
{
	/* This is very 'el hache', but we do not change the main code.
	 * The parent-child tree is:
	 * MainWindow/KaduWindow
	 * +-QWidget (MainWidget)
	 *   =QVBoxLayout (MainLayout)
	 *   +-QSplitter (Split)
	 *     +-QWidget (hbox)
	 *     |=QHBoxLayout (hboxLayout)
	 *     |+-GroupTabBar (GroupBar)
	 *     |+-BuddiesListWidget (ContactsWidget)
	 *     |  =QVBoxLayout (layout)
	 *     |  +-BuddiesListView (ContactsWidget->view())
	 *     |  +-FilterWidget (nameFilterWidget())
	 *     +-BuddyInfoPanel (InfoPanel)
	 */
	Qt::WindowFlags flags;
	QPoint p, cp;
	QSize s, cs;

	flags = mainWindow->windowFlags();
	cp = mainWindow->pos();
	cs = mainWindow->size();

	SimpleViewActive = !SimpleViewActive;

	if (SimpleViewActive)
	{
		p = buddiesListWidget->view()->mapToGlobal(buddiesListWidget->view()->rect().topLeft());
		s = buddiesListWidget->view()->rect().size();
		BackupPosition = p - cp;
		BackupSize = cs - s;
		mainWindow->hide();

		/* 1. Toolbars */
		foreach (QObject *object, mainWindow->children())
		{
			QToolBar *toolBar = dynamic_cast<QToolBar *>(object);
			if (toolBar)
				toolBar->setVisible(false);
		}

		/* 2. Menu bar */
		kaduWindow->menuBar()->hide();

		/* 3. GroupBar */
		groupTabBar->hide();

		/* 4. Filter */
		buddiesListWidget->nameFilterWidget()->hide();

		/* 5. Info panel*/
		kaduWindow->infoPanel()->hide();

		/* 6. Status button */
		statusButton->hide();

		mainWindow->setWindowFlags(flags | Qt::FramelessWindowHint);

		if(SimpleViewConfigUi::instance()->keepSize())
		{
			mainWindow->move(p);
			mainWindow->resize(s);
		}
	}
	else
	{
		mainWindow->hide();
		if(SimpleViewConfigUi::instance()->keepSize())
		{
			BackupPosition = cp - BackupPosition;
			mainWindow->move(BackupPosition);
			mainWindow->resize(cs + BackupSize);
		}
		mainWindow->setWindowFlags(flags & ~(Qt::FramelessWindowHint));

		/* 1. Status button */
		statusButton->setVisible(config_file.readBoolEntry("Look", "ShowStatusButton"));

		/* 2. Info panel*/
		if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
			kaduWindow->infoPanel()->show();

		/* 3. Filter */
		buddiesListWidget->nameFilterWidget()->show();

		/* 4. GroupBar */
		if (config_file.readBoolEntry("Look", "DisplayGroupTabs", true))
			groupTabBar->show();

		/* 5. Menu bar */
		kaduWindow->menuBar()->show();

		/* 6. Toolbars */
		foreach (QObject *object, mainWindow->children())
		{
			QToolBar *toolBar = dynamic_cast<QToolBar *>(object);
			if (toolBar)
				toolBar->setVisible(true);
		}
	}
	mainWindow->show();
}

void SimpleView::compositingEnabled()
{
	/* Give the kadu update the GUI */
	if (SimpleViewActive)
		simpleViewToggle();
}
void SimpleView::compositingDisabled()
{
	/* Give the kadu update the GUI */
	if (SimpleViewActive)
		simpleViewToggle();
}

