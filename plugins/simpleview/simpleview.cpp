/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2014 Przemek Rudy (prudy1@o2.pl)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/hot-key.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/group-tab-bar/group-tab-bar.h"
#include "gui/widgets/roster-widget.h"
#include "gui/widgets/status-buttons.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/windows/kadu-window-service.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-window.h"
#include "icons/kadu-icon.h"
#include "misc/paths-provider.h"
#include "plugin/plugin-repository.h"

#include "plugins/docking/docking.h"
#include "plugins/docking/docking-menu-action-repository.h"
#include "plugins/docking/docking-plugin-object.h"

#include "simpleview-config-ui.h"
#include "simpleview.h"

SimpleView::SimpleView(QObject *parent) :
	QObject{parent},
	SimpleViewActive(false)
{
}

SimpleView::~SimpleView()
{
	simpleViewToggle(false);
}

void SimpleView::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void SimpleView::setDockingMenuActionRepository(DockingMenuActionRepository *dockingMenuActionRepository)
{
	m_dockingMenuActionRepository = dockingMenuActionRepository;
}

void SimpleView::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void SimpleView::setKaduWindowService(KaduWindowService *kaduWindowService)
{
	m_kaduWindowService = kaduWindowService;
}

void SimpleView::init()
{
	SimpleViewConfigUi::createDefaultConfiguration();

	DockAction = new QAction(m_iconsManager->iconByPath(KaduIcon("view-refresh")), tr("Simple view"), this);
	DockAction->setCheckable(true);
	connect(DockAction, SIGNAL(triggered(bool)), this, SLOT(simpleViewToggle(bool)));

	KaduWindowHandle = m_kaduWindowService->kaduWindow();
	MainWindowHandle = KaduWindowHandle->findMainWindow(KaduWindowHandle);
	auto roster = KaduWindowHandle->findChild<RosterWidget *>();
	GroupTabBarHandle = roster->findChild<GroupTabBar *>();
	TalkableTreeViewHandle = roster->talkableTreeView();
	StatusButtonsHandle = KaduWindowHandle->findChild<StatusButtons *>();

	m_dockingMenuActionRepository->addAction(DockAction);

	DiffRect = m_configuration->deprecatedApi()->readRectEntry("Look", "SimpleViewGeometry");
	if (DiffRect != QRect(0,0,0,0))
		simpleViewToggle(true);
	configurationUpdated();
}

void SimpleView::done()
{
	m_configuration->deprecatedApi()->writeEntry("Look", "SimpleViewGeometry", DiffRect);
	m_dockingMenuActionRepository->removeAction(DockAction);
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
	 *     +-RosterWidget (Roster)
	 *     | +-GroupTabBar (GroupBar)
	 *     | + TalkableTreeView (TalkableTree)
	 *     |   This contains buddy list tree
	 *     +-BuddyInfoPanel (InfoPanel)
	 */
	if (activate != SimpleViewActive)
	{
		Qt::WindowFlags flags;
		QRect mr, r;

		SimpleViewActive = activate;

		flags = MainWindowHandle->windowFlags();
		mr = MainWindowHandle->geometry();

		if (SimpleViewActive)
		{
			if (DiffRect == QRect(0,0,0,0))
			{
				if (KeepSize)
				{
					r.setTopLeft(TalkableTreeViewHandle->mapToGlobal(TalkableTreeViewHandle->rect().topLeft()));
					r.setSize(TalkableTreeViewHandle->rect().size());
				}
				else
					r = MainWindowHandle->frameGeometry();

				DiffRect.setRect(mr.x() - r.x(), mr.y() - r.y(), mr.width() - r.width(), mr.height() - r.height());
			}
			else
				/* Since latest changes main window position is stored into the config
				 * BEFORE the plugins are done(). It means the modified position is stored
				 * instead original one.
				 * r.setRect(mr.x() - DiffRect.x(), mr.y() - DiffRect.y(), mr.width() - DiffRect.width(), mr.height() - DiffRect.height());
				 * So now simply use unchanged window position.
				 */
				 r = mr;

			if (Borderless)
				BuddiesListViewStyle = TalkableTreeViewHandle->styleSheet();

			MainWindowHandle->hide();

			/* Toolbars */
			foreach (QObject *object, MainWindowHandle->children())
			{
				QToolBar *toolBar = qobject_cast<QToolBar *>(object);
				if (toolBar)
					toolBar->setVisible(false);
			}

			/* Menu bar */
			KaduWindowHandle->menuBar()->hide();

			/* GroupBar */
			GroupTabBarHandle->setVisible(false);

			/* Filter */
			/* Note: filter hides/shows now automatically.
			 * BuddiesListWidgetHandle->nameFilterWidget()->hide();
			 */

			/* ScrollBar */
			if (NoScrollBar)
				TalkableTreeViewHandle->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

			/* Info panel*/
			KaduWindowHandle->infoPanel()->hide();

			/* Status button */
			StatusButtonsHandle->hide();

			MainWindowHandle->setWindowFlags(flags | Qt::FramelessWindowHint);

			MainWindowHandle->setGeometry(r);

			if (Borderless)
				TalkableTreeViewHandle->setStyleSheet(QString("QTreeView { border-style: none; }") + BuddiesListViewStyle);
		}
		else
		{
			MainWindowHandle->hide();

			if (Borderless)
				TalkableTreeViewHandle->setStyleSheet(BuddiesListViewStyle);

			r.setRect(mr.x() + DiffRect.x(), mr.y() + DiffRect.y(), mr.width() + DiffRect.width(), mr.height() + DiffRect.height());

			MainWindowHandle->setWindowFlags(flags & ~(Qt::FramelessWindowHint));

			MainWindowHandle->setGeometry(r);

			/* Status button */
			StatusButtonsHandle->setVisible(m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowStatusButton"));

			/* Info panel*/
			if (m_configuration->deprecatedApi()->readBoolEntry("Look", "ShowInfoPanel"))
				KaduWindowHandle->infoPanel()->show();

			/* ScrollBar */
			TalkableTreeViewHandle->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

			/* Filter */
			/* Note: filter hides/shows now automatically.
			 * BuddiesListWidgetHandle->nameFilterWidget()->show();
			 */

			/* GroupBar */
			if (m_configuration->deprecatedApi()->readBoolEntry("Look", "DisplayGroupTabs"))
				GroupTabBarHandle->setVisible(true);

			/* Menu bar */
			KaduWindowHandle->menuBar()->show();

			/* Toolbars */
			foreach (QObject *object, MainWindowHandle->children())
			{
				QToolBar *toolBar = qobject_cast<QToolBar *>(object);
				if (toolBar)
					toolBar->setVisible(true);
			}

			DiffRect = QRect(0,0,0,0);
		}
		MainWindowHandle->show();
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

	KeepSize = m_configuration->deprecatedApi()->readBoolEntry("Look", "SimpleViewKeepSize", true);
	NoScrollBar = m_configuration->deprecatedApi()->readBoolEntry("Look", "SimpleViewNoScrollBar", true);
	Borderless = m_configuration->deprecatedApi()->readBoolEntry("Look", "SimpleViewBorderless", true);

}

#include "moc_simpleview.cpp"
