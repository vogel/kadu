/*
 * %kadu copyright begin%
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
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
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QTabBar>
#include <QtGui/QSplitter>

#include "core/core.h"
#include "configuration/configuration-file.h"
#include "misc/path-conversion.h"
#include "gui/hot-key.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/group-tab-bar.h"
#include "gui/widgets/status-buttons.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-window.h"

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

	DockAction = new QAction(IconsManager::instance()->iconByPath("view-refresh"), tr("Simple view"), this);
	DockAction->setCheckable(true);
	connect(DockAction, SIGNAL(triggered(bool)), this, SLOT(simpleViewToggle(bool)));
	DockingManager::instance()->dockMenu()->insertAction(DockingManager::instance()->dockMenu()->actions().last(), DockAction);

	KaduWindowHandle = Core::instance()->kaduWindow();
	MainWindowHandle = KaduWindowHandle->findMainWindow(KaduWindowHandle);
	buddiesListViewHandle = dynamic_cast<BuddiesListView *>(MainWindowHandle->buddiesListView());
	BuddiesListWidgetHandle = dynamic_cast<BuddiesListWidget *>(buddiesListViewHandle->parent());
	groupBarTabHandle = KaduWindowHandle->findChild<GroupTabBar *>();
	GroupBarWidgetHandle = dynamic_cast<QWidget *>(groupBarTabHandle->parent());
	StatusButtonsHandle = KaduWindowHandle->findChild<StatusButtons *>();

	configurationUpdated();
}

SimpleView::~SimpleView()
{
	simpleViewToggle(false);

	if (!Core::instance()->isClosing())
		DockingManager::instance()->dockMenu()->removeAction(DockAction);

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

void SimpleView::simpleViewToggle(bool activate)
{
	/* This is very 'el hacha',
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
	if(activate != SimpleViewActive)
	{
		Qt::WindowFlags flags;
		QPoint p, cp;
		QSize s, cs;

		SimpleViewActive = activate;

		flags = MainWindowHandle->windowFlags();
		cp = MainWindowHandle->pos();
		cs = MainWindowHandle->size();

		if (SimpleViewActive)
		{
			if (Borderless)
				BuddiesListViewStyle = BuddiesListWidgetHandle->view()->styleSheet();

			p = BuddiesListWidgetHandle->view()->mapToGlobal(BuddiesListWidgetHandle->view()->rect().topLeft());
			s = BuddiesListWidgetHandle->view()->rect().size();
			BackupPosition = p - cp;
			BackupSize = cs - s;
			MainWindowHandle->hide();

			/* Toolbars */
			foreach (QObject *object, MainWindowHandle->children())
			{
				QToolBar *toolBar = dynamic_cast<QToolBar *>(object);
				if (toolBar)
					toolBar->setVisible(false);
			}

			/* Menu bar */
			KaduWindowHandle->menuBar()->hide();

			/* GroupBar */
			GroupBarWidgetHandle->hide();

			/* Filter */
			BuddiesListWidgetHandle->nameFilterWidget()->hide();

			/* ScrollBar */
			if (NoScrollBar)
				BuddiesListWidgetHandle->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

			/* Info panel*/
			KaduWindowHandle->infoPanel()->hide();

			/* Status button */
			StatusButtonsHandle->hide();

			MainWindowHandle->setWindowFlags(flags | Qt::FramelessWindowHint);

			if(KeepSize)
			{
				MainWindowHandle->move(p);
				MainWindowHandle->resize(s);
			}

			if (Borderless)
				BuddiesListWidgetHandle->view()->setStyleSheet(QString("QTreeView { border-style: none; }") + BuddiesListViewStyle);
		}
		else
		{
			MainWindowHandle->hide();

			if (Borderless)
				BuddiesListWidgetHandle->view()->setStyleSheet(BuddiesListViewStyle);

			if(KeepSize)
			{
				BackupPosition = cp - BackupPosition;
				MainWindowHandle->move(BackupPosition);
				MainWindowHandle->resize(cs + BackupSize);
			}
			MainWindowHandle->setWindowFlags(flags & ~(Qt::FramelessWindowHint));

			/* Status button */
			StatusButtonsHandle->setVisible(config_file.readBoolEntry("Look", "ShowStatusButton"));

			/* Info panel*/
			if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
				KaduWindowHandle->infoPanel()->show();

			/* ScrollBar */
			BuddiesListWidgetHandle->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

			/* Filter */
			BuddiesListWidgetHandle->nameFilterWidget()->show();

			/* GroupBar */
			GroupBarWidgetHandle->show();

			/* Menu bar */
			KaduWindowHandle->menuBar()->show();

			/* Toolbars */
			foreach (QObject *object, MainWindowHandle->children())
			{
				QToolBar *toolBar = dynamic_cast<QToolBar *>(object);
				if (toolBar)
					toolBar->setVisible(true);
			}
		}
		MainWindowHandle->show();

		if (!Core::instance()->isClosing())
			DockAction->setChecked(SimpleViewActive);
	}
}

void SimpleView::compositingEnabled()
{
	/* Give the kadu update the GUI */
	simpleViewToggle(false);
}
void SimpleView::compositingDisabled()
{
	/* Give the kadu update the GUI */
	simpleViewToggle(false);
}

void SimpleView::configurationUpdated()
{
	/* Give the kadu update the GUI with old configuration */
	simpleViewToggle(false);

	KeepSize = config_file.readBoolEntry("Look", "SimpleViewKeepSize", true);
	NoScrollBar = config_file.readBoolEntry("Look", "SimpleViewNoScrollBar", true);
	Borderless = config_file.readBoolEntry("Look", "SimpleViewBorderless", true);

}
