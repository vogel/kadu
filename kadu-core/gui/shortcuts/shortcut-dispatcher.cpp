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

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/random_access_index.hpp>

#include "gui/actions/action-context.h"

#include "shortcut.h"
#include "shortcut-handler.h"
#include "shortcut-provider.h"
#include "shortcut-repository.h"

#include "shortcut-dispatcher.h"

using boost::multi_index_container;
using boost::multi_index::const_mem_fun;
using boost::multi_index::hashed_unique;
using boost::multi_index::hashed_non_unique;
using boost::multi_index::indexed_by;
using boost::multi_index::random_access;

std::size_t hash_value(const QString &str)
{
	return qHash(str);
}


class ShortcutDispatcherImpl : public ShortcutDispatcher
{

	ShortcutRepository *Repository;

	class ordered_t;
	class name_t;
	class key_sequence_t;
	class provider_t;
	
	typedef multi_index_container<
		Shortcut,
		indexed_by<
			random_access<boost::multi_index::tag<ordered_t> >,
			hashed_unique<boost::multi_index::tag<name_t>,
				const_mem_fun<Shortcut,
				const QString &,
				&Shortcut::name> >,
			hashed_non_unique<boost::multi_index::tag<provider_t>,
				const_mem_fun<Shortcut,
				const QString &,
				&Shortcut::providerName> >,
			hashed_non_unique<boost::multi_index::tag<key_sequence_t>,
				const_mem_fun<Shortcut,
				const QString &,
				&Shortcut::keySequence> > > >
	ShortcutContainer;
	
	typedef ShortcutContainer::index<ordered_t>::type ordered_index;
	typedef ShortcutContainer::index<name_t>::type by_name_index;
	typedef ShortcutContainer::index<provider_t>::type by_provider_index;
	typedef ShortcutContainer::index<key_sequence_t>::type by_key_sequence_index;
	
	ShortcutContainer Shortcuts;

	QMap<QString, ShortcutProvider *> ShortcutsProviders;

	QString keyEventToString(QKeyEvent *e);
	
public:
	void init();
	void setShortcutRepository(ShortcutRepository *repository);

	bool registerShortcut(const Shortcut &shortcut);
	void unregisterShortcut(const QString &name, ShortcutProvider *provider);
	
	bool registerShortcutProvider(ShortcutProvider *provider);
	void unregisterShortcutProvider(ShortcutProvider *provider);

	bool handleKeyEvent(ActionContext *actionSource, QKeyEvent *e);
};

QString ShortcutDispatcherImpl::keyEventToString(QKeyEvent *e)
{
	QString result;
	if ((e->modifiers() & Qt::ControlModifier) || (e->key() == Qt::Key_Control))
		result = "Ctrl+";
	
	if ((e->modifiers() & Qt::MetaModifier) || (e->key() == Qt::Key_Meta))
		result += "Shift+Alt+";
	else
	{
		if ((e->modifiers() & Qt::ShiftModifier) || (e->key() == Qt::Key_Shift))
			result+= "Shift+";
		if ((e->modifiers() & Qt::AltModifier) || (e->key() == Qt::Key_Alt))
			result += "Alt+";
	}
	
	if (!((e->key() == Qt::Key_Control) ||
		(e->key() == Qt::Key_Shift) ||
		(e->key() == Qt::Key_Alt) ||
		(e->key() == Qt::Key_Meta)))
		result += QKeySequence(e->key()).toString();
	
	return result;
}


void ShortcutDispatcherImpl::init()
{

}

void ShortcutDispatcherImpl::setShortcutRepository(ShortcutRepository *repository)
{
	Repository = repository;
}

bool ShortcutDispatcherImpl::registerShortcut(const Shortcut &shortcut)
{
	Q_ASSERT(ShortcutsProviders.contains(shortcut.providerName()));
	
	by_name_index &byName = Shortcuts.get<name_t>();
	by_name_index::const_iterator found = byName.find(shortcut.name());

	Q_ASSERT(found != byName.end());

	Shortcut currentShortcut = Repository->getShortcut(shortcut.name());
	if (currentShortcut.isInitialized())
	{
		Q_ASSERT(currentShortcut.providerName() == shortcut.providerName());

		Shortcuts.push_back(currentShortcut);
		return true;
	}

	by_key_sequence_index &byKeySequence = Shortcuts.get<key_sequence_t>();
	
	std::pair<by_key_sequence_index::const_iterator, by_key_sequence_index::const_iterator> pit =
	byKeySequence.equal_range(shortcut.keySequence());
	
	by_key_sequence_index::iterator first = pit.first;
	by_key_sequence_index::iterator end = pit.second;
	
	while (first != end)
	{
		if ((*first).type() == shortcut.type())
			return false;
		++first;
	}
		
	Shortcuts.push_back(shortcut);
	
	return true;
	
}

void ShortcutDispatcherImpl::unregisterShortcut(const QString &name, ShortcutProvider *provider)
{
	Q_ASSERT(ShortcutsProviders.contains(provider->name()));
	
	by_name_index &byName = Shortcuts.get<name_t>();
	by_name_index::const_iterator found = byName.find(name);
	
	Q_ASSERT(found == byName.end());

	Q_ASSERT((*found).providerName() == provider->name());

	byName.erase(found);
}

bool ShortcutDispatcherImpl::registerShortcutProvider(ShortcutProvider *provider)
{
	if (ShortcutsProviders.contains(provider->name()))
		return false;

	ShortcutsProviders.insert(provider->name(), provider);
	return true;
}

void ShortcutDispatcherImpl::unregisterShortcutProvider(ShortcutProvider *provider)
{
	Q_ASSERT(ShortcutsProviders.contains(provider->name()));

	ShortcutProvider *p = ShortcutsProviders.value(provider->name());

	Q_ASSERT(p == provider);

	ShortcutsProviders.remove(provider->name());
}

bool ShortcutDispatcherImpl::handleKeyEvent(ActionContext *actionSource, QKeyEvent *e)
{
	QString keyEventString = keyEventToString(e);

	by_key_sequence_index &byKeySequence = Shortcuts.get<key_sequence_t>();
	
	std::pair<by_key_sequence_index::const_iterator, by_key_sequence_index::const_iterator> pit =
	byKeySequence.equal_range(keyEventString);
	
	by_key_sequence_index::iterator first = pit.first;
	by_key_sequence_index::iterator end = pit.second;
	
	while (first != end)
	{
		if (actionSource->supportsActionType((*first).type()))
		{
			ShortcutProvider *provider = ShortcutsProviders.value((*first).providerName());
			ShortcutHandler *handler = provider->getShortcutHandler((*first).name());
			if (!handler)
				continue;
		
			return handler->handle(actionSource);
		}
	}
	return false;
	
}

ShortcutDispatcher * ShortcutDispatcher::Instance = 0;

ShortcutDispatcher *ShortcutDispatcher::instance()
{
	if (0 == Instance)
	{
		ShortcutDispatcherImpl *instance = new ShortcutDispatcherImpl();
		
		instance->init();
//		instance->setShortcutRepository(ShortcutRepository::create());
		
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
