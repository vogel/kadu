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

#include "kadu-style-engine.h"

#include "chat-style/chat-style.h"
#include "chat-style/engine/chat-style-renderer-factory.h"
#include "chat-style/engine/kadu/kadu-chat-syntax.h"
#include "chat-style/engine/kadu/kadu-style-renderer-factory.h"
#include "core/injected-factory.h"
#include "misc/paths-provider.h"
#include "misc/syntax-list.h"

#include <QtCore/QFileInfo>

KaduStyleEngine::KaduStyleEngine(QObject *parent) : QObject{parent}
{
}

KaduStyleEngine::~KaduStyleEngine()
{
}

void KaduStyleEngine::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void KaduStyleEngine::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void KaduStyleEngine::init()
{
    syntaxList = QSharedPointer<SyntaxList>(m_injectedFactory->makeInjected<SyntaxList>("chat"));
}

QString KaduStyleEngine::isStyleValid(QString stylePath)
{
    QFileInfo fi;
    fi.setFile(stylePath);
    return fi.suffix() == "syntax" ? fi.completeBaseName() : QString();
}

std::unique_ptr<ChatStyleRendererFactory> KaduStyleEngine::createRendererFactory(const ChatStyle &chatStyle)
{
    QString chatSyntax = SyntaxList::readSyntax(
        m_pathsProvider, "chat", chatStyle.name(),
        "<p style=\"background-color: #{backgroundColor};\">#{separator}"
        "<font color=\"#{fontColor}\"><kadu:header><b><font color=\"#{nickColor}\">%a</font> :: "
        "#{receivedDate}[ / S #{sentDate}]</b><br /></kadu:header>"
        "#{message}</font></p>");

    return m_injectedFactory->makeUnique<KaduStyleRendererFactory>(std::make_shared<KaduChatSyntax>(chatSyntax));
}
