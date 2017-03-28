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

#pragma once

#include "chat/chat.h"
#include "exports.h"
#include "misc/iterator.h"

#include <QtCore/QObject>
#include <map>
#include <memory>

class ChatWidget;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetRepository
 * @short Repository of ChatWidget instances.
 *
 * This repository holds instances of ChatWidget. Instances are not owned
 * by repository and must be deleted elsewhere.
 *
 * Instances are added by addChatWidget(ChatWidget*) It also emits
 * chatWidgetAdded(ChatWidget*) signals.
 *
 * Instances are removed by removeChatWidget(ChatWidget*) methods. Signal
 * chatWidgetRemoved(ChatWidget*) is emitted. This method does not delete
 * ChatWidget instance. It is also automatically called when ChatWidget
 * is deleted elsewhere.
 *
 * Instances can be enumerated as ChatWidget* in standard for-range loop as
 * begin() and end() iterators are provided.
 */
class KADUAPI ChatWidgetRepository : public QObject
{
    Q_OBJECT

public:
    using Storage = std::map<Chat, ChatWidget *>;
    using WrappedIterator = Storage::iterator;
    using Iterator = IteratorWrapper<WrappedIterator, ChatWidget *>;

    explicit ChatWidgetRepository(QObject *parent = nullptr);
    virtual ~ChatWidgetRepository();

    /**
     * @short Begin iterator that returns ChatWidget *.
     */
    virtual Iterator begin() = 0;

    /**
     * @short End iterator that returns ChatWidget *.
     */
    virtual Iterator end() = 0;

    /**
     * @short Add new chatWidget to repository.
     *
     * Add new chatWidget to repository only if it is valid and chat widget for given
     * chat is not already in repository. In other case nothing happens.
     *
     * In case of successfull addition ownership chatWidgetAdded(ChatWidget*) signal is emitted.
     */
    virtual void addChatWidget(ChatWidget *chatWidget) = 0;

    /**
     * @short Return true if repository has chat widget for given chat.
     */
    virtual bool hasWidgetForChat(const Chat &chat) const = 0;

    /**
     * @short Return ChatWidget for given chat.
     * @param chat chat to get ChatWidget for
     * @return ChatWidget for given chat
     *
     * If chat is null then nullptr is returned. If repository does contain ChatWidget then
     * it is returned. Else nullptr is returned.
     */
    virtual ChatWidget *widgetForChat(const Chat &chat) = 0;

public slots:
    /**
     * @short Remove chatWidget from repository
     *
     * Remove chatWidget from repository only if it is  already in repository.
     * Signal chatWidgetRemoved(ChatWidget*) is emitted after successfull removal.
     * ChatWidget instance is not destroyed after removal.
     */
    virtual void removeChatWidget(ChatWidget *chatWidget) = 0;

    /**
     * @short Remove ChatWidget for given chat from repository and destroy it.
     *
     * Remove chatWidget for given chat from repository only if it is  already in repository.
     * Signal chatWidgetRemoved(ChatWidget*) is emitted after successfull removal.
     * ChatWidget instance is not destroyed after removal.
     */
    virtual void removeChatWidget(Chat chat) = 0;

signals:
    /**
     * @short Signal emitted when new ChatWidget was added to this repository.
     * @param chatWidget newly added ChatWidget instance
     */
    void chatWidgetAdded(ChatWidget *chatWidget);

    /**
     * @short Signal emitted when new ChatWidget was removed from this repository.
     * @param chatWidget removed ChatWidget instance
     */
    void chatWidgetRemoved(ChatWidget *chatWidget);
};

inline ChatWidgetRepository::Iterator begin(ChatWidgetRepository *chatWidgetRepository)
{
    return chatWidgetRepository->begin();
}

inline ChatWidgetRepository::Iterator end(ChatWidgetRepository *chatWidgetRepository)
{
    return chatWidgetRepository->end();
}

/**
 * @}
 */
