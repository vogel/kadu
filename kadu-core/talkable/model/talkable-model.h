/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"
#include "model/kadu-merged-proxy-model.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyListModel;
class BuddyManagerAdapter;
class ChatListModel;
class ChatManager;
class InjectedFactory;
class Myself;

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class TalkableModel
 * @short Proxy model than merges BuddyListModel and ChatListModel with Manager adapters.
 *
 * This proxy model merges BuddyListModel and ChatListModel and displays registered instances of Buddy and Chat classes.
 * Chat instances are displayed before Buddy ones.
 */
class TalkableModel : public KaduMergedProxyModel
{
    Q_OBJECT

public:
    /**
     * @short Create new instance of TalkableModel with given parent.
     * @param parent QObject parent of new object
     */
    explicit TalkableModel(QObject *parent = nullptr);
    virtual ~TalkableModel();

    /**
     * @short Change value of includeMyself property.
     * @param includeMyself new value of includeMyself property
     *
     * Change value of includeMyself property. If includeMyself is true, then Myself buddy will be
     * available in this model.
     */
    void setIncludeMyself(bool includeMyself);

    /**
     * @short Return value of includeMyself property.
     * @return value of includeMyself property
     *
     * Returns value of includeMyself property. If includeMyself is true, then Myself buddy will be
     * available in this model.
     */
    bool includeMyself() const;

private:
    QPointer<ChatManager> m_chatManager;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<Myself> m_myself;

    owned_qptr<ChatListModel> m_chats;
    owned_qptr<BuddyListModel> m_buddies;
    owned_qptr<BuddyManagerAdapter> m_buddiesAdapter;
    bool m_includeMyself;

private slots:
    INJEQT_SET void setChatManager(ChatManager *chatManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setMyself(Myself *myself);
    INJEQT_INIT void init();
};

/**
 * @}
 */
