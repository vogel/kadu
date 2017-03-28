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

#include "exports.h"
#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatWidgetRepository;
class ChatWidget;
class OpenChatRepository;

/**
 * @addtogroup Chat
 * @{
 */

/**
 * @class OpenChatService
 * @short Service to keep OpenChatRepository in sync with ChatWidgetRepository
 */
class KADUAPI OpenChatService : public QObject
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(SERVICE)

public:
    Q_INVOKABLE explicit OpenChatService(QObject *parent = nullptr);
    virtual ~OpenChatService();

private:
    QPointer<ChatWidgetRepository> m_chatWidgetRepository;
    QPointer<OpenChatRepository> m_openChatRepository;

private slots:
    INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
    INJEQT_SET void setOpenChatRepository(OpenChatRepository *openChatRepository);
    INJEQT_INIT void init();

    void chatWidgetAdded(ChatWidget *chatWidget);
    void chatWidgetRemoved(ChatWidget *chatWidget);
};

/**
 * @}
 */
