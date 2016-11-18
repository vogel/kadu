/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "action-description.h"

#include "core/injected-factory.h"
#include "configuration/configuration.h"
#include "gui/actions/action.h"
#include "gui/hot-key.h"
#include "gui/windows/main-window.h"

#include <QtCore/QLatin1String>
#include <QtCore/QMetaMethod>

ActionDescription::ActionDescription( QObject *parent) :
		QObject(parent),
		Type(TypeAll), Checkable(false), ShortcutContext(Qt::WidgetShortcut)
{
	Deleting = false;
}

ActionDescription::~ActionDescription()
{
	Deleting = true;

	qDeleteAll(MappedActions);
	MappedActions.clear();
}

void ActionDescription::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ActionDescription::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

Configuration * ActionDescription::configuration() const
{
	return m_configuration;
}

InjectedFactory * ActionDescription::injectedFactory() const
{
	return m_injectedFactory;
}

void ActionDescription::actionAboutToBeDestroyed(Action *action)
{
	if (Deleting)
		return;

	if (action && MappedActions.contains(action->context()))
		MappedActions.remove(action->context());
}

void ActionDescription::setType(ActionType type)
{
	Type = type;
}

void ActionDescription::setName(const QString &name)
{
	Name = name;
}

void ActionDescription::setIcon(const KaduIcon &icon)
{
	Icon = icon;
}

void ActionDescription::setText(const QString &text)
{
	Text = text;
}

void ActionDescription::setCheckable(bool checkable)
{
	Checkable = checkable;
}

void ActionDescription::setShortcut(QString configItem, Qt::ShortcutContext context)
{
	ShortcutItem = configItem;
	ShortcutContext = context;

	configurationUpdated();
}

void ActionDescription::actionTriggeredSlot(QAction *sender, bool toggled)
{
	actionTriggered(sender, toggled);

	Action *action = qobject_cast<Action *>(sender);
	if (action)
		triggered(sender->parentWidget(), action->context(), toggled);
}

QMenu * ActionDescription::menuForAction(Action *action)
{
	Q_UNUSED(action)

	return 0;
}

void ActionDescription::actionInstanceCreated(Action *action)
{
	QMenu *menu = menuForAction(action);
	if (menu)
		action->setMenu(menu);
}

void ActionDescription::updateActionStates()
{
	for (auto &&action : actions())
		updateActionState(action);
}

Action * ActionDescription::createAction(ActionContext *context, QObject *parent)
{
	Action *result = MappedActions.value(context);
	if (result)
	{
		if (result->parent() != parent)
			qWarning("ActionDescription::createAction(): requested action for already known context but with different parent\n");

		return result;
	}

	result = m_injectedFactory->makeInjected<Action>(this, context, parent);
	MappedActions.insert(context, result);

	actionInstanceCreated(result);
	emit actionCreated(result);

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		result->setShortcut(HotKey::shortCutFromFile(m_configuration, "ShortCuts", ShortcutItem));
		result->setShortcutContext(ShortcutContext);
	}
	else if (MappedActions.count() == 1)
	{
		result->setShortcut(HotKey::shortCutFromFile(m_configuration, "ShortCuts", ShortcutItem));
		result->setShortcutContext(ShortcutContext);
	}

	connect(result, SIGNAL(triggered(QAction *, bool)), this, SLOT(actionTriggeredSlot(QAction *, bool)));
	connect(result, SIGNAL(aboutToBeDestroyed(Action *)), this, SLOT(actionAboutToBeDestroyed(Action *)));

	return result;
}

QList<Action *> ActionDescription::actions()
{
	return MappedActions.values();
}

Action * ActionDescription::action(ActionContext *context)
{
	if (MappedActions.contains(context))
		return MappedActions.value(context);
	else
		return 0;
}

void ActionDescription::configurationUpdated()
{
	if (ShortcutItem.isEmpty())
		return;

	if (ShortcutContext != Qt::ApplicationShortcut)
	{
		foreach (Action *action, MappedActions)
		{
			action->setShortcut(HotKey::shortCutFromFile(m_configuration, "ShortCuts", ShortcutItem));
			action->setShortcutContext(ShortcutContext);
		}
	}
	else if (!MappedActions.isEmpty())
	{
		MappedActions.values().at(0)->setShortcut(HotKey::shortCutFromFile(m_configuration, "ShortCuts", ShortcutItem));
		MappedActions.values().at(0)->setShortcutContext(ShortcutContext);
	}
}

void ActionDescription::connectNotify(const QMetaMethod &signal)
{
	QObject::connectNotify(signal);

	if (signal == QMetaMethod::fromSignal(&ActionDescription::actionCreated))
		foreach (Action *action, MappedActions)
			emit actionCreated(action);
}

#include "moc_action-description.cpp"
