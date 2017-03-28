/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "actions/action-description.h"
#include "injeqt-type-roles.h"
#include "model/roles.h"

#include <QtCore/QPointer>
#include <QtWidgets/QAction>
#include <injeqt/injeqt.h>

class Buddy;
class ChatTypeManager;
class Chat;
class IconsManager;
class Myself;

class DeleteTalkableAction : public ActionDescription
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(ACTION)

public:
    Q_INVOKABLE explicit DeleteTalkableAction(QObject *parent = nullptr);
    virtual ~DeleteTalkableAction();

    void trigger(ActionContext *context);

protected:
    virtual void actionInstanceCreated(Action *action);
    virtual void updateActionState(Action *action);
    virtual void triggered(QWidget *widget, ActionContext *context, bool toggled);

private:
    QPointer<ChatTypeManager> m_chatTypeManager;
    QPointer<IconsManager> m_iconsManager;
    QPointer<Myself> m_myself;

    int actionRole(ActionContext *context) const;
    Chat actionChat(ActionContext *context) const;
    Buddy actionBuddy(ActionContext *context) const;

    void setChatActionTitleAndIcon(Action *action);
    void setBuddyActionTitleAndIcon(Action *action);

    void updateChatActionState(Action *action);
    void updateBuddyActionState(Action *action);

    void chatActionTriggered(ActionContext *context);
    void buddyActionTriggered(ActionContext *context);

private slots:
    INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setMyself(Myself *myself);
};
