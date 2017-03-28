/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "stop-search-action.h"

#include "windows/search-window.h"

StopSearchAction::StopSearchAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"dialog-cancel"});
    setName(QStringLiteral("stopSearchAction"));
    setText(tr("Stop"));
    setType(ActionDescription::TypeSearch);
}

StopSearchAction::~StopSearchAction()
{
}

void StopSearchAction::actionInstanceCreated(Action *action)
{
    auto search = qobject_cast<SearchWindow *>(action->parentWidget());
    if (!search || !search->SearchInProgress)
        action->setEnabled(false);
}

void StopSearchAction::actionTriggered(QAction *sender, bool)
{
    auto search = qobject_cast<SearchWindow *>(sender->parentWidget());
    if (search)
        search->stopSearch();
}

#include "moc_stop-search-action.cpp"
