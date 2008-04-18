/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QContextMenuEvent>

#include "config_file.h"
#include "debug.h"
#include "toolbar.h"

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

bool offsetComparator(ToolBar *t1, ToolBar *t2)
{
	return t1->offset() < t2->offset();
}

bool KaduMainWindow::loadToolBarsFromConfig(const QString &configName, Qt::ToolBarArea area, bool remove)
{
	kdebugf();

	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (toolbarsConfig.isNull())
		return false;

// 	setBlockToolbars(toolbars_elem.attribute("blocked").toInt());

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

	qSort(toolBars.begin(), toolBars.end(), offsetComparator);
	foreach(ToolBar *toolBar, toolBars)
		addToolBar(area, toolBar);

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
