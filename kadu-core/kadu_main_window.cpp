/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

void KaduMainWindow::loadToolBarsFromConfig(const QString &configName, Qt::ToolBarArea area)
{
	kdebugf();

	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (toolbarsConfig.isNull())
		return;

// 	setBlockToolbars(toolbars_elem.attribute("blocked").toInt());

	QDomElement dockareaConfig = xml_config_file->findElementByProperty(toolbarsConfig, "DockArea", "name", configName);
	if (dockareaConfig.isNull())
		return;

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

		addToolBar(area, toolbar);
	}

	toolbarsConfig.removeChild(dockareaConfig);

	kdebugf2();
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
		toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "ToolBar");

	writeToolBarsToConfig(toolbarsConfig, realPrefix + "topDockArea", Qt::TopToolBarArea);
	writeToolBarsToConfig(toolbarsConfig, realPrefix + "leftDockArea", Qt::LeftToolBarArea);
	writeToolBarsToConfig(toolbarsConfig, realPrefix + "bottomDockArea", Qt::BottomToolBarArea);
	writeToolBarsToConfig(toolbarsConfig, realPrefix + "rightDockArea", Qt::RightToolBarArea);
}

void KaduMainWindow::writeToolBarsToConfig(QDomElement parentConfig, const QString &configName, Qt::ToolBarArea area)
{
	QDomElement dockAreaConfig = xml_config_file->createElement(parentConfig, "DockArea");
	dockAreaConfig.setAttribute("name", configName);

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
