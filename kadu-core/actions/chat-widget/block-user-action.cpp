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

#include "block-user-action.h"

#include "actions/action.h"
#include "buddies/buddy-set.h"
#include "core/myself.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-edit-widget.h"
#include "widgets/chat-widget/chat-widget.h"
#include "widgets/custom-input.h"

BlockUserAction::BlockUserAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setCheckable(true);
    setIcon(KaduIcon{"kadu_icons/block-buddy"});
    setName(QStringLiteral("blockUserAction"));
    setText(tr("Block Buddy"));
    setType(ActionDescription::TypeUser);
}

BlockUserAction::~BlockUserAction()
{
}

void BlockUserAction::setMyself(Myself *myself)
{
    m_myself = myself;
}

void BlockUserAction::actionInstanceCreated(Action *action)
{
    auto chatEditBox = qobject_cast<ChatEditBox *>(action->parent());
    if (!chatEditBox)
        return;

    connect(chatEditBox->inputBox(), SIGNAL(textChanged()), action, SLOT(checkState()));
    connect(chatEditBox->chatWidget()->chat(), SIGNAL(connected()), action, SLOT(checkState()));
    connect(chatEditBox->chatWidget()->chat(), SIGNAL(disconnected()), action, SLOT(checkState()));
}

void BlockUserAction::actionTriggered(QAction *sender, bool)
{
    auto action = qobject_cast<Action *>(sender);
    if (!action)
        return;

    auto buddies = action->context()->buddies();
    if (buddies.isEmpty())
        return;

    auto on = std::any_of(std::begin(buddies), std::end(buddies), [](auto const &buddy) { return !buddy.isBlocked(); });
    for (auto const &buddy : buddies)
    {
        buddy.setBlocked(on);
        updateBlockingActions(buddy);
    }
}

void BlockUserAction::updateActionState(Action *action)
{
    auto buddies = action->context()->buddies();

    if (!buddies.count() || buddies.contains(m_myself->buddy()))
    {
        action->setEnabled(false);
        return;
    }

    if (action->context()->buddies().isAnyTemporary())
    {
        action->setEnabled(false);
        return;
    }

    action->setEnabled(!action->context()->roles().contains(ContactRole));

    auto on = std::any_of(std::begin(buddies), std::end(buddies), [](auto const &buddy) { return buddy.isBlocked(); });
    action->setChecked(on);
}

void BlockUserAction::updateBlockingActions(Buddy buddy)
{
    auto buddyContacts = buddy.contacts();

    for (auto action : actions())
    {
        auto contact = action->context()->contacts().toContact();
        if (contact)
            if (buddyContacts.contains(contact))
                action->setChecked(buddy.isBlocked());
    }
}

#include "moc_block-user-action.cpp"
