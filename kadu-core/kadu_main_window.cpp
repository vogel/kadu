/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

#include "config_file.h"
#include "debug.h"
#include "toolbar.h"
#include "userbox.h"

#include "kadu_main_window.h"

KaduMainWindow::KaduMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
}

KaduMainWindow::~KaduMainWindow()
{
}

void KaduMainWindow::loadToolBarsFromConfig(const QString &prefix)
{
	QString realPrefix;
	if (prefix.isEmpty())
		realPrefix = "";
	else
		realPrefix = prefix + "_";

	loadToolBarsFromConfig(realPrefix + "topDockArea", Qt::TopToolBarArea);
	loadToolBarsFromConfig(realPrefix + "leftDockArea", Qt::LeftToolBarArea);
	loadToolBarsFromConfig(realPrefix + "bottomDockArea", Qt::BottomToolBarArea);
	loadToolBarsFromConfig(realPrefix + "rightDockArea", Qt::RightToolBarArea);
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

bool KaduMainWindow::loadToolBarsFromConfig(const QString &configName, Qt::ToolBarArea area, bool remove)
{
	kdebugf();

	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (toolbarsConfig.isNull())
		return false;

	QDomElement dockareaConfig = xml_config_file->findElementByProperty(toolbarsConfig, "DockArea", "name", configName);
	if (dockareaConfig.isNull())
		return false;

	QList<ToolBar *> toolBars;
	for (QDomNode n = dockareaConfig.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &toolbarConfig = n.toElement();
		if (toolbarConfig.isNull())
			continue;
		if (toolbarConfig.tagName() != "ToolBar")
			continue;

		ToolBar* toolbar = new ToolBar(this);
// 		moveDockWindow(toolbar);
		toolbar->loadFromConfig(toolbarConfig);
		toolbar->show();
// 		setAcceptDockWindow(toolbar, true);

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

	if (remove)
		toolbarsConfig.removeChild(dockareaConfig);

	kdebugf2();

	return true;
}

void KaduMainWindow::writeToolBarsToConfig(const QString &prefix)
{
	QString realPrefix;
	if (prefix.isEmpty())
		realPrefix = "";
	else
		realPrefix = prefix + "_";

	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");
	if (toolbarsConfig.isNull())
		toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "ToolBars");

	writeToolBarsToConfig(toolbarsConfig, realPrefix + "topDockArea", Qt::TopToolBarArea);
	writeToolBarsToConfig(toolbarsConfig, realPrefix + "leftDockArea", Qt::LeftToolBarArea);
	writeToolBarsToConfig(toolbarsConfig, realPrefix + "bottomDockArea", Qt::BottomToolBarArea);
	writeToolBarsToConfig(toolbarsConfig, realPrefix + "rightDockArea", Qt::RightToolBarArea);
}

void KaduMainWindow::writeToolBarsToConfig(QDomElement parentConfig, const QString &configName, Qt::ToolBarArea area)
{
	QDomElement dockAreaConfig = xml_config_file->findElementByProperty(parentConfig, "DockArea", "name", configName);
	if (dockAreaConfig.isNull())
	{
		dockAreaConfig = xml_config_file->createElement(parentConfig, "DockArea");
		dockAreaConfig.setAttribute("name", configName);
	}
	else
		xml_config_file->removeChildren(dockAreaConfig);


	// TODO: laaaaame
	foreach(QObject *child, children())
	{
		ToolBar *toolBar = dynamic_cast<ToolBar *>(child);
		if (!toolBar)
			continue;

		if (toolBarArea(toolBar) != area)
			continue;

		toolBar->writeToConfig(dockAreaConfig);
	}
}

void KaduMainWindow::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu(this);
	menu->addAction(tr("Create new toolbar"), this, SLOT(addTopToolbar()));

	menu->exec(event->globalPos());
}

void KaduMainWindow::addTopToolbar()
{
	addToolBar(Qt::TopToolBarArea, new ToolBar(this));
}

void KaduMainWindow::addBottomToolbar()
{
	addToolBar(Qt::BottomToolBarArea, new ToolBar(this));
}

void KaduMainWindow::addLeftToolbar()
{
	addToolBar(Qt::LeftToolBarArea, new ToolBar(this));
}

void KaduMainWindow::addRightToolbar()
{
	addToolBar(Qt::RightToolBarArea, new ToolBar(this));
}

void KaduMainWindow::actionAdded(KaduAction *action)
{
	if (userBox())
		connect(userBox(), SIGNAL(userListChanged()), action, SLOT(checkIfEnabled()));
}
