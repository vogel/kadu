/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "message-html-renderer-service.h"
#include "message-html-renderer-service.moc"

#include "dom/dom-processor-service.h"
#include "html/normalized-html-string.h"
#include "message/message.h"

MessageHtmlRendererService::MessageHtmlRendererService(QObject *parent) : QObject(parent)
{
}

MessageHtmlRendererService::~MessageHtmlRendererService()
{
}

void MessageHtmlRendererService::setDomProcessorService(DomProcessorService *domProcessorService)
{
    m_domProcessorService = domProcessorService;
}

QString MessageHtmlRendererService::renderMessage(const Message &message)
{
    return QString{R"(<span style="white-space: pre-wrap;">%1</span>)"}.arg(
        m_domProcessorService->process(message.content().string()));
}
