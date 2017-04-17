/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "actions/action.h"
#include "actions/action-description.h"
#include "core/session-service.h"
#include "windows/main-window.h"

#include "actions.h"
#include "actions.moc"

Actions::Actions(QObject *parent) : QObject{parent}, BlockSignals{false}
{
}

Actions::~Actions()
{
}

void Actions::setSessionService(SessionService *sessionService)
{
    m_sessionService = sessionService;
}

bool Actions::insert(ActionDescription *action)
{
    if (contains(action->name()))
        return false;

    QMap<QString, ActionDescription *>::insert(action->name(), action);

    if (!BlockSignals)
        emit actionLoaded(action);
    return true;
}

void Actions::remove(ActionDescription *action)
{
    QMap<QString, ActionDescription *>::remove(action->name());

    if (!m_sessionService->isClosing())
        emit actionUnloaded(action);
}

QAction *Actions::createAction(const QString &name, ActionContext *context, QObject *parent)
{
    if (!contains(name))
        return nullptr;

    auto v = value(name);
    if (!v)
        return nullptr;

    auto result = v->createAction(context, parent);

    emit actionCreated(result);

    return result;
}
