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
#include "gui/widgets/buddies-list-view.h"

#include "modules/docking/docking.h"
#include "simpleview-config-ui.h"
#include "simpleview.h"

SimpleView *SimpleView::Instance = 0;

SimpleView::SimpleView() :
	SimpleViewActive(false)
{
	BuddiesListView *buddiesListViewHandle;
	GroupTabBar *groupBarTabHandle;

	SimpleViewConfigUi::createInstance();

	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/simpleview.ui"));
	MainConfigurationWindow::registerUiHandler(SimpleViewConfigUi::instance());

	DockAction = DockingManager::instance()->dockMenu()->addAction(IconsManager::instance()->iconByPath("16x16/view-refresh.png"), tr("Simple view"), this, SLOT(simpleViewToggle()));
	DockAction->setShortcut(HotKey::shortCutFromFile("ShortCuts", "kadu_simpleview"));

	KaduWindowHandle = Core::instance()->kaduWindow();
	MainWindowHandle = KaduWindowHandle->findMainWindow(KaduWindowHandle);
	buddiesListViewHandle = dynamic_cast<BuddiesListView *>(MainWindowHandle->contactsListView());
	BuddiesListWidgetHandle = dynamic_cast<BuddiesListWidget *>(buddiesListViewHandle->parent());
	groupBarTabHandle = KaduWindowHandle->findChild<GroupTabBar *>();
	GroupBarWidgetHandle = dynamic_cast<QWidget *>(groupBarTabHandle->parent());
	StatusButtonsHandle = KaduWindowHandle->findChild<StatusButtons *>();
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
	/* This is very 'el hache',
	 * but this way we do not change the main code.
	 *
	 * The parent-child tree is:
	 * MainWindow/KaduWindow
	 * +-QWidget (MainWidget)
	 *   =QVBoxLayout (MainLayout)
	 *   +-QSplitter (Split)
	 *     +-QWidget (hbox)
	 *     |=QHBoxLayout (hboxLayout)
	 *     |+-QWidget (GroupBarWidget)
	 *     |  =QVBoxLayout (GroupBarLayout)
	 *     |  +-GroupTabBar (GroupBar)
	 *     |+-BuddiesListWidget (ContactsWidget)
	 *     |  =QVBoxLayout (layout)
	 *     |  +-BuddiesListView (ContactsWidget->view())
	 *     |  +-FilterWidget (nameFilterWidget())
	 *     +-BuddyInfoPanel (InfoPanel)
	 */
	Qt::WindowFlags flags;
	QPoint p, cp;
	QSize s, cs;

	flags = MainWindowHandle->windowFlags();
	cp = MainWindowHandle->pos();
	cs = MainWindowHandle->size();

	SimpleViewActive = !SimpleViewActive;

	if (SimpleViewActive)
	{
		p = BuddiesListWidgetHandle->view()->mapToGlobal(BuddiesListWidgetHandle->view()->rect().topLeft());
		s = BuddiesListWidgetHandle->view()->rect().size();
		BackupPosition = p - cp;
		BackupSize = cs - s;
		MainWindowHandle->hide();

		/* 1. Toolbars */
		foreach (QObject *object, MainWindowHandle->children())
		{
			QToolBar *toolBar = dynamic_cast<QToolBar *>(object);
			if (toolBar)
				toolBar->setVisible(false);
		}

		/* 2. Menu bar */
		KaduWindowHandle->menuBar()->hide();

		/* 3. GroupBar */
		GroupBarWidgetHandle->hide();

		/* 4. Filter */
		BuddiesListWidgetHandle->nameFilterWidget()->hide();

		/* 5. Info panel*/
		KaduWindowHandle->infoPanel()->hide();

		/* 6. Status button */
		StatusButtonsHandle->hide();

		MainWindowHandle->setWindowFlags(flags | Qt::FramelessWindowHint);

		if(SimpleViewConfigUi::instance()->keepSize())
		{
			MainWindowHandle->move(p);
			MainWindowHandle->resize(s);
		}
	}
	else
	{
		MainWindowHandle->hide();
		if(SimpleViewConfigUi::instance()->keepSize())
		{
			BackupPosition = cp - BackupPosition;
			MainWindowHandle->move(BackupPosition);
			MainWindowHandle->resize(cs + BackupSize);
		}
		MainWindowHandle->setWindowFlags(flags & ~(Qt::FramelessWindowHint));

		/* 1. Status button */
		StatusButtonsHandle->setVisible(config_file.readBoolEntry("Look", "ShowStatusButton"));

		/* 2. Info panel*/
		if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
			KaduWindowHandle->infoPanel()->show();

		/* 3. Filter */
		BuddiesListWidgetHandle->nameFilterWidget()->show();

		/* 4. GroupBar */
		GroupBarWidgetHandle->show();

		/* 5. Menu bar */
		KaduWindowHandle->menuBar()->show();

		/* 6. Toolbars */
		foreach (QObject *object, MainWindowHandle->children())
		{
			QToolBar *toolBar = dynamic_cast<QToolBar *>(object);
			if (toolBar)
				toolBar->setVisible(true);
		}
	}
	MainWindowHandle->show();
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

