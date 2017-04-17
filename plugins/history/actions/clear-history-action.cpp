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

#include "clear-history-action.h"
#include "clear-history-action.moc"

#include "history.h"

#include "actions/action-context.h"
#include "actions/action.h"
#include "chat/chat.h"
#include "contacts/contact-set.h"
#include "core/myself.h"
#include "talkable/talkable.h"

ClearHistoryAction::ClearHistoryAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"kadu_icons/clear-history"});
    setName(QStringLiteral("clearHistoryAction"));
    setText(tr("Clear History"));
    setType(ActionDescription::TypeUser);
}

ClearHistoryAction::~ClearHistoryAction()
{
}

void ClearHistoryAction::setHistory(History *history)
{
    m_history = history;
}

void ClearHistoryAction::setMyself(Myself *myself)
{
    m_myself = myself;
}

void ClearHistoryAction::actionTriggered(QAction *sender, bool)
{
    if (!m_history->currentStorage())
        return;

    auto action = qobject_cast<Action *>(sender);
    if (!action)
        return;

    if (action->context()->chat())
        m_history->currentStorage()->clearChatHistory(action->context()->chat());
}

void ClearHistoryAction::updateActionState(Action *action)
{
    action->setEnabled(false);
    auto const &contacts = action->context()->contacts();

    if (contacts.isEmpty())
        return;

    for (auto const &contact : contacts)
        if (m_myself->buddy() == contact.ownerBuddy())
            return;

    action->setEnabled(true);
}
