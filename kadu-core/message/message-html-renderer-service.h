/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class DomProcessorService;
class Message;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class MessageHtmlRendererService
 * @short Service for rendering messages into HTML.
 * @author Rafał 'Vogel' Malinowski
 *
 * This service is used to render Message into HTML fragment.
 */
class KADUAPI MessageHtmlRendererService : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MessageHtmlRendererService(QObject *parent = nullptr);
    virtual ~MessageHtmlRendererService();

    /**
     * @short Render Message into HTML fragment,
     * @author Rafał 'Vogel' Malinowski
     * @param message to render
     * @return HTML representation of Message
     */
    QString renderMessage(const Message &message);

private:
    QPointer<DomProcessorService> m_domProcessorService;

private slots:
    /**
     * @short Set domProcessorService,
     * @author Rafał 'Vogel' Malinowski
     * @param domProcessorService domProcessorService
     *
     * This service will be used to process DOM representation of HTML version of messages.
     */
    INJEQT_SET void setDomProcessorService(DomProcessorService *domProcessorService);
};

/**
 * @}
 */
