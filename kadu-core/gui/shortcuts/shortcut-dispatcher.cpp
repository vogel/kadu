/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QObject>
#include <QtGui/QKeyEvent>

#include "gui/actions/action-context.h"

#include "shortcut.h"
#include "shortcut-provider.h"
#include "shortcut-repository.h"

#include "shortcut-dispatcher.h"

class ShortcutDispatcherImpl : public ShortcutDispatcher
{

	ShortcutRepository *Repository;

public:
	void init();
	void setShortcutRepository(ShortcutRepository *repository);

	bool registerShortcut(const Shortcut &shortcut);
	void unregisterShortcut(const QString &name);
	
	bool registerShortcutProvider(ShortcutProvider *provider);
	void unregisterShortcutProvider();

	bool handleKeyEvent(ActionContext *actionSource, QKeyEvent *e);
};

void ShortcutDispatcherImpl::init()
{

}

void ShortcutDispatcherImpl::setShortcutRepository(ShortcutRepository *repository)
{

}

bool ShortcutDispatcherImpl::registerShortcut(const Shortcut &shortcut)
{
	
}

void ShortcutDispatcherImpl::unregisterShortcut(const QString &name)
{
	
}

bool ShortcutDispatcherImpl::registerShortcutProvider(ShortcutProvider *provider)
{
	
}

bool ShortcutDispatcherImpl::handleKeyEvent(ActionContext *actionSource, QKeyEvent *e)
{

}


void ShortcutDispatcherImpl::unregisterShortcutProvider()
{
	
}

ShortcutDispatcher * ShortcutDispatcher::Instance = 0;

ShortcutDispatcher *ShortcutDispatcher::instance()
{
	if (0 == Instance)
	{
		ShortcutDispatcherImpl *instance = new ShortcutDispatcherImpl();
		
		instance->init();
		instance->setShortcutRepository(new ShortcutRepository());
		
		Instance = instance;
	}
	
	return Instance;
	
}

ShortcutDispatcher::ShortcutDispatcher()
{
}

ShortcutDispatcher::~ShortcutDispatcher()
{
}
