/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact.h"

#include "tool-tip-class-manager.h"

ToolTipClassManager * ToolTipClassManager::Instance = 0;

ToolTipClassManager * ToolTipClassManager::instance()
{
	if (!Instance)
		Instance = new ToolTipClassManager();

	return Instance;
}

ToolTipClassManager::ToolTipClassManager()
	: CurrentToolTipClass(0)
{
}

ToolTipClassManager::~ToolTipClassManager()
{
	if (CurrentToolTipClass)
		CurrentToolTipClass->hideToolTip();
}

void ToolTipClassManager::registerToolTipClass(const QString &toolTipClassName, AbstractToolTip *toolTipClass)
{
	ToolTipClasses[toolTipClassName] = toolTipClass;

	if (ToolTipClassName == toolTipClassName)
		CurrentToolTipClass = toolTipClass;
}

void ToolTipClassManager::unregisterToolTipClass(const QString &toolTipClassName)
{
	if (ToolTipClassName == toolTipClassName && CurrentToolTipClass)
	{
		CurrentToolTipClass->hideToolTip();
		CurrentToolTipClass = 0;
	}

	if (ToolTipClasses.contains(ToolTipClassName))
		ToolTipClasses.remove(ToolTipClassName);
}

QStringList ToolTipClassManager::getToolTipClasses()
{
	return ToolTipClasses.keys();
}

void ToolTipClassManager::useToolTipClass(const QString &toolTipClassName)
{
	if (CurrentToolTipClass)
		CurrentToolTipClass->hideToolTip();

	ToolTipClassName = toolTipClassName;

	if (ToolTipClasses.contains(ToolTipClassName))
		CurrentToolTipClass = ToolTipClasses[ToolTipClassName];
	else
		CurrentToolTipClass = 0;
}

bool ToolTipClassManager::showToolTip(const QPoint &where, Contact contact)
{
	if (CurrentToolTipClass)
	{
		CurrentToolTipClass->showToolTip(where, contact);
		return true;
	}

	return false;
}

bool ToolTipClassManager::hideToolTip()
{
	if (CurrentToolTipClass)
	{
		CurrentToolTipClass->hideToolTip();
		return true;
	}

	return false;
}
