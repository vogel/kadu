/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QMenu>

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration-manager.h"
#include "configuration/configuration.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"
#include "core/injected-factory.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/configuration/toolbar-configuration-manager.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/widgets/toolbar.h"
#include "debug.h"

#include "main-window.h"

#if defined(Q_OS_UNIX)
#include <QtX11Extras/QX11Info>

#include "os/x11/x11tools.h" // this should be included as last one,
#undef KeyPress
#undef Status            // and Status defined by Xlib.h must be undefined
#endif

MainWindow * MainWindow::findMainWindow(QWidget *widget)
{
	while (widget)
	{
		MainWindow *window = qobject_cast<MainWindow *>(widget);
		if (window)
			return window;
		widget = widget->parentWidget();
	}

	return 0;
}

MainWindow::MainWindow(ActionContext *context, const QString &windowName, QWidget *parent) :
		QMainWindow(parent), DesktopAwareObject(this),  WindowName(windowName), TransparencyEnabled(false), BlurEnabled(false),
		Context(context)
{
}

MainWindow::~MainWindow()
{
	disconnect(m_actions, 0, this, 0);
	disconnect(m_configurationManager->toolbarConfigurationManager(), 0, this, 0);

	delete Context;
	Context = 0;
}

void MainWindow::setActions(Actions *actions)
{
	m_actions = actions;
}

void MainWindow::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void MainWindow::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

Configuration * MainWindow::configuration() const
{
	return m_configuration;
}

void MainWindow::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void MainWindow::init()
{
	connect(m_configurationManager->toolbarConfigurationManager(), SIGNAL(configurationUpdated()),
			this, SLOT(refreshToolBars()));
	connect(m_actions, SIGNAL(actionLoaded(ActionDescription*)),
			this, SLOT(actionLoadedOrUnloaded(ActionDescription*)));
	connect(m_actions, SIGNAL(actionUnloaded(ActionDescription*)),
			this, SLOT(actionLoadedOrUnloaded(ActionDescription*)));
}

InjectedFactory * MainWindow::injectedFactory() const
{
	return m_injectedFactory;
}

void MainWindow::setActionContext(ActionContext *actionContext)
{
	Context = actionContext;
}

void MainWindow::loadToolBarsFromConfig()
{
	// lame, i know

	foreach (QObject *object, children())
	{
		QToolBar *toolBar = qobject_cast<QToolBar *>(object);
		if (toolBar)
		{
			removeToolBar(toolBar);
			delete toolBar;
		}
	}

	loadToolBarsFromConfig(Qt::TopToolBarArea);
	loadToolBarsFromConfig(Qt::LeftToolBarArea);
	loadToolBarsFromConfig(Qt::BottomToolBarArea);
	loadToolBarsFromConfig(Qt::RightToolBarArea);
}

bool horizontalToolbarComparator(ToolBar *t1, ToolBar *t2)
{
	if (t1->yOffset() < t2->yOffset())
		return true;
	if (t1->yOffset() > t2->yOffset())
		return false;
	return t1->xOffset() < t2->xOffset();
}

bool verticalToolbarComparator(ToolBar *t1, ToolBar *t2)
{
	if (t1->xOffset() < t2->xOffset())
		return true;
	if (t1->xOffset() > t2->xOffset())
		return false;
	return t1->yOffset() < t2->yOffset();
}

void MainWindow::loadToolBarsFromConfigNode(QDomElement dockareaConfig, Qt::ToolBarArea area)
{
	QList<ToolBar *> toolBars;
	for (QDomNode n = dockareaConfig.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &toolbarConfig = n.toElement();
		if (toolbarConfig.isNull())
			continue;
		if (toolbarConfig.tagName() != "ToolBar")
			continue;

		ToolBar* toolbar = newToolbar(this);
		toolbar->loadFromConfig(toolbarConfig);
		toolbar->show();
		/* show() resets the WA_NoSystemBackground and AutoFillBackground */
		toolbar->setAttribute(Qt::WA_NoSystemBackground, !TransparencyEnabled);
		toolbar->setAutoFillBackground(TransparencyEnabled);

		toolBars.append(toolbar);
	}

	int currentLine = 0;
	if (area == Qt::LeftToolBarArea || area == Qt::RightToolBarArea)
	{
		qSort(toolBars.begin(), toolBars.end(), verticalToolbarComparator);
		foreach(ToolBar *toolBar, toolBars)
		{
			if (toolBar->xOffset() != currentLine)
				addToolBarBreak(area);

			addToolBar(area, toolBar);
			currentLine = toolBar->xOffset();
		}
	}
	else
	{
		qSort(toolBars.begin(), toolBars.end(), horizontalToolbarComparator);
		foreach(ToolBar *toolBar, toolBars)
		{
			if (toolBar->yOffset() != currentLine)
				addToolBarBreak(area);

			addToolBar(area, toolBar);
			currentLine = toolBar->yOffset();
		}
	}
}

void MainWindow::loadToolBarsFromConfig(Qt::ToolBarArea area)
{
	QDomElement dockareaConfig = getDockAreaConfigElement(area);
	loadToolBarsFromConfigNode(dockareaConfig, area);
}

bool MainWindow::loadOldToolBarsFromConfig(const QString &configName, Qt::ToolBarArea area)
{
	QDomElement toolbarsConfig = m_configuration->api()->findElement(m_configuration->api()->rootElement(), "Toolbars");

	if (toolbarsConfig.isNull())
		return false;

	QDomElement dockareaConfig = m_configuration->api()->findElementByProperty(toolbarsConfig, "DockArea", "name", configName);
	if (dockareaConfig.isNull())
		return false;

	loadToolBarsFromConfigNode(dockareaConfig, area);

	dockareaConfig.parentNode().removeChild(dockareaConfig);
	return true;
}

QDomElement MainWindow::getToolbarsConfigElement(Configuration *configuration)
{
	QDomElement toolbarsConfig = configuration->api()->findElement(configuration->api()->rootElement(), "Toolbars");
	if (toolbarsConfig.isNull())
		toolbarsConfig = configuration->api()->createElement(configuration->api()->rootElement(), "Toolbars");

	return toolbarsConfig;
}

QDomElement MainWindow::getDockAreaConfigElement(Qt::ToolBarArea area)
{
	QString realPrefix;
	if (!WindowName.isEmpty())
		realPrefix = WindowName + '_';

	QString suffix;

	switch (area)
	{
		case Qt::TopToolBarArea:
			suffix = "topDockArea";
			break;
		case Qt::LeftToolBarArea:
			suffix = "leftDockArea";
			break;
		case Qt::RightToolBarArea:
			suffix = "rightDockArea";
			break;
		case Qt::BottomToolBarArea:
			suffix = "bottomDockArea";
			break;
		default:
			return QDomElement();
	}

	return getDockAreaConfigElement(m_configuration, getToolbarsConfigElement(m_configuration), realPrefix + suffix);
}

QDomElement MainWindow::getDockAreaConfigElement(Configuration *configuration, QDomElement toolbarsConfig, const QString &name)
{
	QDomElement dockAreaConfig = configuration->api()->findElementByProperty(toolbarsConfig, "DockArea", "name", name);
	if (dockAreaConfig.isNull())
	{
		dockAreaConfig = configuration->api()->createElement(toolbarsConfig, "DockArea");
		dockAreaConfig.setAttribute("name", name);
	}

	return dockAreaConfig;
}

void MainWindow::addToolButton(Configuration *configuration, QDomElement toolbarConfig, const QString &actionName, Qt::ToolButtonStyle style)
{
	QDomElement buttonConfig = configuration->api()->findElementByProperty(toolbarConfig, "ToolButton", "action_name", actionName);
//don't add element if exists
	if (!buttonConfig.isNull())
		return;
	buttonConfig = configuration->api()->createElement(toolbarConfig, "ToolButton");
	buttonConfig.setAttribute("action_name", actionName);
	buttonConfig.setAttribute("toolbutton_style", style);
}

QDomElement MainWindow::findExistingToolbarOnArea(Configuration *configuration, const QString &areaName)
{
	QDomElement dockAreaConfig = configuration->api()->findElementByProperty(getToolbarsConfigElement(configuration), "DockArea", "name", areaName);
	QDomElement nullResult;

	if (dockAreaConfig.isNull())
		return nullResult;

	QDomElement toolbarElement = configuration->api()->findElement(dockAreaConfig, "ToolBar");
	if (toolbarElement.isNull())
		return nullResult;

	return toolbarElement;
}

QDomElement MainWindow::findExistingToolbar(Configuration *configuration, const QString &prefix)
{
	QString realPrefix;
	if (!prefix.isEmpty())
		realPrefix = prefix + '_';

	QDomElement toolbarElement = findExistingToolbarOnArea(configuration, realPrefix + "topDockArea");
	if (!toolbarElement.isNull())
		return toolbarElement;

	toolbarElement = findExistingToolbarOnArea(configuration, realPrefix + "leftDockArea");
	if (!toolbarElement.isNull())
		return toolbarElement;

	toolbarElement = findExistingToolbarOnArea(configuration, realPrefix + "rightDockArea");
	if (!toolbarElement.isNull())
		return toolbarElement;

	toolbarElement = findExistingToolbarOnArea(configuration, realPrefix + "bottomDockArea");
	if (!toolbarElement.isNull())
		return toolbarElement;

	QDomElement dockAreaConfig = getDockAreaConfigElement(configuration, getToolbarsConfigElement(configuration), realPrefix + "topDockArea");
	return configuration->api()->createElement(dockAreaConfig, "ToolBar");
}

void MainWindow::writeToolBarsToConfig()
{
	writeToolBarsToConfig(Qt::TopToolBarArea);
	writeToolBarsToConfig(Qt::LeftToolBarArea);
	writeToolBarsToConfig(Qt::BottomToolBarArea);
	writeToolBarsToConfig(Qt::RightToolBarArea);
}

void MainWindow::writeToolBarsToConfig(Qt::ToolBarArea area)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(area);
	m_configuration->api()->removeChildren(dockAreaConfig);

	// TODO: laaaaame
	foreach(QObject *child, children())
	{
		ToolBar *toolBar = qobject_cast<ToolBar *>(child);
		if (!toolBar)
			continue;

		if (toolBarArea(toolBar) != area)
			continue;

		toolBar->writeToConfig(dockAreaConfig);
	}
}

void MainWindow::actionLoadedOrUnloaded(ActionDescription *action)
{
	if (supportsActionType(action->type()))
		refreshToolBars();
}

void MainWindow::refreshToolBars()
{
	// We don't need it when closing.
	// BTW, on Mac it caused crashes on exit. TODO: check out why, as there is probably a bug somewhere.
	if (Core::instance() && Core::instance()->isClosing())
		return;

	loadToolBarsFromConfig();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	if (!ToolBar::isBlockToolbars(m_configuration))
	{
		QMenu menu;
		menu.addAction(tr("Create new toolbar"), this, SLOT(addTopToolbar()));
		menu.exec(event->globalPos());
	}
}

ToolBar *MainWindow::newToolbar(QWidget *parent)
{
	auto toolBar = m_injectedFactory->makeInjected<ToolBar>(parent);
	toolBar->setAttribute(Qt::WA_NoSystemBackground, !TransparencyEnabled);
	toolBar->setAutoFillBackground(TransparencyEnabled);

	connect(toolBar, SIGNAL(updated()), this, SLOT(toolbarUpdated()));
	connect(toolBar, SIGNAL(removed(ToolBar*)), this, SLOT(toolbarRemoved(ToolBar*)));

	return toolBar;
}

void MainWindow::addTopToolbar()
{
	addToolBar(Qt::TopToolBarArea, newToolbar(this));
	toolbarUpdated();
}

void MainWindow::addBottomToolbar()
{
	addToolBar(Qt::BottomToolBarArea, newToolbar(this));
	toolbarUpdated();
}

void MainWindow::addLeftToolbar()
{
	addToolBar(Qt::LeftToolBarArea, newToolbar(this));
	toolbarUpdated();
}

void MainWindow::addRightToolbar()
{
	addToolBar(Qt::RightToolBarArea, newToolbar(this));
	toolbarUpdated();
}

bool MainWindow::hasAction(const QString &actionName, ToolBar *exclude)
{
	foreach (QObject *object, children())
	{
		ToolBar *toolBar = qobject_cast<ToolBar *>(object);
		if (toolBar && toolBar != exclude && toolBar->hasAction(actionName))
			return true;
	}

	return false;
}

Contact MainWindow::contact()
{
	ContactSet contactSet = actionContext()->contacts();
	return 1 == contactSet.count()
			? *contactSet.constBegin()
			: Contact::null;
}

Buddy MainWindow::buddy()
{
	BuddySet buddySet = actionContext()->buddies();
	return 1 == buddySet.count()
			? *buddySet.constBegin()
			: Buddy::null;
}

void MainWindow::setTransparency(bool enable)
{
	/* 1. Do not make MainWindow related to the CompositingAwareObject class
	 *    as not every child wants to be compositing aware
	 * 2. Allow child to decide if and when to use transparency or not and
	 *    provide means to do this.
	 * Enabling transparency sets main window background transparent whilst
	 * toolbars are opaque, so the central widget can enjoy the transparency.
	 */
	TransparencyEnabled = enable;
	if (TransparencyEnabled)
	{
		setAttribute(Qt::WA_TranslucentBackground, true);

		foreach (QObject *object, children())
		{
			QToolBar *toolBar = qobject_cast<QToolBar *>(object);
			if (toolBar)
			{
				toolBar->setAttribute(Qt::WA_NoSystemBackground, false);
				toolBar->setAutoFillBackground(true);
			}
		}
	}
	else
	{
		foreach (QObject *object, children())
		{
			QToolBar *toolBar = qobject_cast<QToolBar *>(object);
			if (toolBar)
				toolBar->setAutoFillBackground(false);
		}
		setAttribute(Qt::WA_TranslucentBackground, false);
		setAttribute(Qt::WA_NoSystemBackground, false);
	}
}

void MainWindow::toolbarUpdated()
{
	writeToolBarsToConfig();

	m_configurationManager->toolbarConfigurationManager()->notifyConfigurationUpdated();
}

void MainWindow::toolbarRemoved(ToolBar *toolBar)
{
	toolBar->hide();
	toolBar->setParent(0); // remove it from this window
	toolBar->deleteLater();

	toolbarUpdated();
}

ActionContext * MainWindow::actionContext()
{
	return Context;
}

void MainWindow::setBlur(bool enable)
{
#if !defined(Q_OS_UNIX)
	Q_UNUSED(enable);
#else
	BlurEnabled = enable;
	X11_setBlur(QX11Info::display(), winId(), enable);
#endif
}


void MainWindow::showEvent(QShowEvent * event)
{
	if (BlurEnabled)
		setBlur(true);
	QMainWindow::showEvent(event);
}

#include "moc_main-window.cpp"
