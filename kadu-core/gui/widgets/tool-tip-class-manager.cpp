/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-or-contact.h"
#include "configuration/configuration-file.h"

#include "tool-tip-class-manager.h"

ToolTipClassManager * ToolTipClassManager::Instance = 0;

ToolTipClassManager * ToolTipClassManager::instance()
{
	if (!Instance)
		Instance = new ToolTipClassManager();

	return Instance;
}

ToolTipClassManager::ToolTipClassManager() :
		CurrentToolTipClass(0)
{
	config_file.addVariable("Look", "UserboxToolTipStyle", "Hints");
	configurationUpdated();
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

bool ToolTipClassManager::showToolTip(const QPoint &where, BuddyOrContact buddyOrContact)
{
	if (CurrentToolTipClass)
	{
		CurrentToolTipClass->showToolTip(where, buddyOrContact);
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

void ToolTipClassManager::configurationUpdated()
{
	useToolTipClass(config_file.readEntry("Look", "UserboxToolTipStyle", "Hints"));
}
