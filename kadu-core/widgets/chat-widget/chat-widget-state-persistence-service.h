/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ChatWidget;
class ChatWidgetRepository;

class ChatWidgetStatePersistenceService : public QObject
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(SERVICE)

public:
    Q_INVOKABLE ChatWidgetStatePersistenceService();
    virtual ~ChatWidgetStatePersistenceService();

private slots:
    INJEQT_SET void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);
    INJEQT_INIT void init();

    void storeChatWidgetState(ChatWidget *chatWidget);
    void restoreChatWidgetState(ChatWidget *chatWidget);

private:
    QPointer<ChatWidgetRepository> m_chatWidgetRepository;
};
