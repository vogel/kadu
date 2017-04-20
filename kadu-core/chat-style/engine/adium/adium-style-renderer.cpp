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

#include "adium-style-renderer.h"
#include "adium-style-renderer.moc"

#include "avatars/avatar-id.h"
#include "avatars/avatars.h"
#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/adium/adium-style-engine.h"
#include "chat-style/engine/adium/adium-style.h"
#include "chat-style/engine/adium/adium-time-formatter.h"
#include "contacts/contact-global-id.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "html/normalized-html-string.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "message/message-html-renderer-service.h"
#include "message/message-render-info-factory.h"
#include "message/message-render-info.h"
#include "misc/date-time.h"
#include "misc/misc.h"
#include "misc/paths-provider.h"
#include "os/generic/system-info.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-image.h"

#include <QtCore/QFile>
#include <QtGui/QTextDocument>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebPage>

AdiumStyleRenderer::AdiumStyleRenderer(
    ChatStyleRendererConfiguration configuration, std::shared_ptr<AdiumStyle> style, QObject *parent)
        : ChatStyleRenderer{std::move(configuration), parent}, m_style{std::move(style)}
{
}

AdiumStyleRenderer::~AdiumStyleRenderer()
{
}

void AdiumStyleRenderer::setAvatars(Avatars *avatars)
{
    m_avatars = avatars;
}

void AdiumStyleRenderer::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
    m_chatConfigurationHolder = chatConfigurationHolder;
}

void AdiumStyleRenderer::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void AdiumStyleRenderer::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
    m_messageHtmlRendererService = messageHtmlRendererService;
}

void AdiumStyleRenderer::setSystemInfo(SystemInfo *systemInfo)
{
    m_systemInfo = systemInfo;
}

void AdiumStyleRenderer::init()
{
    configuration().webFrame().setHtml(preprocessStyleBaseHtml(configuration().useTransparency()));
    connect(&configuration().webFrame(), SIGNAL(loadFinished(bool)), this, SLOT(pageLoaded()));
}

void AdiumStyleRenderer::pageLoaded()
{
    configuration().webFrame().evaluateJavaScript(configuration().javaScript());
    configuration().webFrame().evaluateJavaScript("initStyle()");

    setReady();
}

void AdiumStyleRenderer::removeFirstMessage()
{
    configuration().webFrame().evaluateJavaScript("adium_removeFirstMessage()");
}

void AdiumStyleRenderer::clearMessages()
{
    configuration().webFrame().evaluateJavaScript("adium_clearMessages()");
}

void AdiumStyleRenderer::appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo)
{
    QString formattedMessageHtml;

    switch (message.type())
    {
    case MessageTypeReceived:
    {
        if (messageRenderInfo.includeHeader())
            formattedMessageHtml = m_style->incomingHtml();
        else
            formattedMessageHtml = m_style->nextIncomingHtml();
        break;
    }
    case MessageTypeSent:
    {
        if (messageRenderInfo.includeHeader())
            formattedMessageHtml = m_style->outgoingHtml();
        else
            formattedMessageHtml = m_style->nextOutgoingHtml();
        break;
    }
    case MessageTypeSystem:
    {
        formattedMessageHtml = m_style->statusHtml();
        break;
    }

    default:
        break;
    }

    formattedMessageHtml = replacedNewLine(
        replaceKeywords(m_style->baseHref(), formattedMessageHtml, message, messageRenderInfo.nickColor()),
        QStringLiteral(" "));
    formattedMessageHtml.replace('\\', QStringLiteral("\\\\"));
    formattedMessageHtml.replace('\'', QStringLiteral("\\'"));
    if (!message.id().isEmpty())
        formattedMessageHtml.prepend(QString("<span id=\"message_%1\">").arg(Qt::escape(message.id())));
    else
        formattedMessageHtml.prepend("<span>");
    formattedMessageHtml.append("</span>");

    if (messageRenderInfo.includeHeader())
        configuration().webFrame().evaluateJavaScript("appendMessage('" + formattedMessageHtml + "')");
    else
        configuration().webFrame().evaluateJavaScript("appendNextMessage('" + formattedMessageHtml + "')");
}

QString AdiumStyleRenderer::preprocessStyleBaseHtml(bool useTransparency)
{
    QString styleBaseHtml = m_style->templateHtml();
    styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, Qt::escape(PathsProvider::webKitPath(m_style->baseHref())));
    styleBaseHtml.replace(
        styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(m_style->baseHref(), m_style->footerHtml()));
    styleBaseHtml.replace(
        styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(m_style->baseHref(), m_style->headerHtml()));

    if (m_style->usesCustomTemplateHtml() && m_style->styleViewVersion() < 3)
    {
        if (m_style->currentVariant() != m_style->defaultVariant())
            styleBaseHtml.replace(
                styleBaseHtml.lastIndexOf("%@"), 2, "Variants/" + Qt::escape(m_style->currentVariant()));
        else
            styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, Qt::escape(m_style->mainHref()));
    }
    else
    {
        styleBaseHtml.replace(
            styleBaseHtml.lastIndexOf("%@"), 2,
            (m_style->styleViewVersion() < 3 && m_style->defaultVariant() == m_style->currentVariant())
                ? Qt::escape(PathsProvider::webKitPath(m_style->mainHref()))
                : "Variants/" + Qt::escape(m_style->currentVariant()));
        styleBaseHtml.replace(
            styleBaseHtml.lastIndexOf("%@"), 2,
            (m_style->styleViewVersion() < 3)
                ? QString()
                : QString("@import url( \"" + Qt::escape(PathsProvider::webKitPath(m_style->mainHref())) + "\" );"));
    }

    if (useTransparency && !m_style->defaultBackgroundIsTransparent())
        styleBaseHtml.replace(
            styleBaseHtml.lastIndexOf("==bodyBackground=="), static_cast<int>(qstrlen("==bodyBackground==")),
            "background-image: none; background: none; background-color: rgba(0, 0, 0, 0)");

    return styleBaseHtml;
}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumStyleRenderer::replaceKeywords(const QString &styleHref, const QString &style)
{
    if (!configuration().chat())
        return {};

    QString result = style;

    // TODO: get Chat name (contacts' nicks?)
    // Replace %chatName% //TODO. Find way to dynamic update this tag (add id ?)
    int contactsCount = configuration().chat().contacts().count();

    QString chatName;
    if (!configuration().chat().display().isEmpty())
        chatName = configuration().chat().display();
    else if (contactsCount > 1)
        chatName = tr("Conference [%1]").arg(contactsCount);
    else
        chatName = configuration().chat().name();

    result.replace(QString("%chatName%"), Qt::escape(chatName));

    // Replace %sourceName%
    result.replace(QString("%sourceName%"), Qt::escape(configuration().chat().chatAccount().accountIdentity().name()));
    // Replace %destinationName%
    result.replace(QString("%destinationName%"), Qt::escape(configuration().chat().name()));
    // For %timeOpened%, display the date and time. TODO: get real time
    result.replace(
        QString("%timeOpened%"),
        Qt::escape(printDateTime(m_chatConfigurationHolder->niceDateFormat(), QDateTime::currentDateTime())));

    // TODO 0.10.0: get real time!!!
    QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
    int pos = 0;
    while ((pos = timeRegExp.indexIn(result, pos)) != -1)
        result.replace(
            pos, timeRegExp.cap(0).length(), Qt::escape(AdiumTimeFormatter::convertTimeDate(
                                                 m_systemInfo, timeRegExp.cap(1), QDateTime::currentDateTime())));

    QString photoIncoming;
    QString photoOutgoing;

    int contactsSize = configuration().chat().contacts().size();
    if (contactsSize == 1)
    {
        auto const &contact = configuration().chat().contacts().toContact();
        auto path = m_avatars->path(avatarIds(contact));
        if (!path.isEmpty())
            photoIncoming = PathsProvider::webKitPath(path);
        else
            photoIncoming = PathsProvider::webKitPath(styleHref + QStringLiteral("Incoming/buddy_icon.png"));
    }
    else
        photoIncoming = PathsProvider::webKitPath(styleHref + QStringLiteral("Incoming/buddy_icon.png"));

    auto const &contact = configuration().chat().chatAccount().accountContact();
    auto path = m_avatars->path(avatarIds(contact));
    if (!path.isEmpty())
        photoIncoming = PathsProvider::webKitPath(path);
    else
        photoOutgoing = PathsProvider::webKitPath(styleHref + QStringLiteral("Outgoing/buddy_icon.png"));

    result.replace(QString("%incomingIconPath%"), Qt::escape(photoIncoming));
    result.replace(QString("%outgoingIconPath%"), Qt::escape(photoOutgoing));

    return result;
}

QString AdiumStyleRenderer::replaceKeywords(
    const QString &styleHref, const QString &source, const Message &message, const QString &nickColor)
{
    QString result = source;

    // Replace sender (contact nick)
    result.replace(QString("%sender%"), Qt::escape(message.messageSender().display(true)));
    // Replace %screenName% (contact ID)
    result.replace(QString("%senderScreenName%"), Qt::escape(message.messageSender().id()));
    // Replace service name (protocol name)
    if (message.messageChat().chatAccount().protocolHandler() &&
        message.messageChat().chatAccount().protocolHandler()->protocolFactory())
    {
        result.replace(
            QString("%service%"),
            Qt::escape(message.messageChat().chatAccount().protocolHandler()->protocolFactory()->displayName()));
        // Replace protocolIcon (sender statusIcon). TODO:
        result.replace(
            QString("%senderStatusIcon%"),
            Qt::escape(m_iconsManager->iconPath(
                message.messageChat().chatAccount().protocolHandler()->protocolFactory()->icon())));
    }
    else
    {
        result.replace(
            QString("%service%"), Qt::escape(Qt::escape(message.messageChat().chatAccount().accountIdentity().name())));
        result.remove("%senderStatusIcon%");
    }

    // Replace time
    QDateTime time = message.sendDate().isNull() ? message.receiveDate() : message.sendDate();
    result.replace(QString("%time%"), Qt::escape(printDateTime(m_chatConfigurationHolder->niceDateFormat(), time)));
    // Look for %time{X}%
    QRegExp timeRegExp("%time\\{([^}]*)\\}%");
    int pos = 0;
    while ((pos = timeRegExp.indexIn(result, pos)) != -1)
        result.replace(
            pos, timeRegExp.cap(0).length(),
            Qt::escape(AdiumTimeFormatter::convertTimeDate(m_systemInfo, timeRegExp.cap(1), time)));

    result.replace("%shortTime%", Qt::escape(printDateTime(m_chatConfigurationHolder->niceDateFormat(), time)));

    // Look for %textbackgroundcolor{X}%
    // TODO: highlight background color: use the X value.
    QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
    int textPos = 0;
    while ((textPos = textBackgroundRegExp.indexIn(result, textPos)) != -1)
        result.replace(textPos, textBackgroundRegExp.cap(0).length(), "inherit");

    // Replace userIconPath
    QString photoPath;
    if (message.type() == MessageTypeReceived)
    {
        result.replace(QString("%messageClasses%"), "message incoming");

        auto const &contact = message.messageSender();
        auto path = m_avatars->path(avatarIds(contact));
        if (!path.isEmpty())
            photoPath = PathsProvider::webKitPath(path);
        else
            photoPath = Qt::escape(PathsProvider::webKitPath(styleHref + QStringLiteral("Incoming/buddy_icon.png")));
    }
    else if (message.type() == MessageTypeSent)
    {
        result.replace(QString("%messageClasses%"), "message outgoing");
        auto const &contact = message.messageChat().chatAccount().accountContact();
        auto path = m_avatars->path(avatarIds(contact));
        if (!path.isEmpty())
            photoPath = PathsProvider::webKitPath(path);
        else
            photoPath = Qt::escape(PathsProvider::webKitPath(styleHref + QStringLiteral("Outgoing/buddy_icon.png")));
    }
    else
        result.remove(QString("%messageClasses%"));

    result.replace(QString("%userIconPath%"), Qt::escape(photoPath));

    // Message direction ("rtl"(Right-To-Left) or "ltr"(Left-to-right))
    result.replace(QString("%messageDirection%"), "ltr");

    // Replace contact's color
    QString lightColorName;
    QRegExp senderColorRegExp("%senderColor(?:\\{([^}]*)\\})?%");
    textPos = 0;
    while ((textPos = senderColorRegExp.indexIn(result, textPos)) != -1)
    {
        int light = 100;
        bool doLight = false;
        if (senderColorRegExp.captureCount() >= 1)
            light = senderColorRegExp.cap(1).toInt(&doLight);

        if (doLight && lightColorName.isNull())
            lightColorName = QColor(nickColor).light(light).name();

        result.replace(textPos, senderColorRegExp.cap(0).length(), Qt::escape(doLight ? lightColorName : nickColor));
    }

    auto messageText = m_messageHtmlRendererService ? m_messageHtmlRendererService.data()->renderMessage(message)
                                                    : message.content().string();

    if (!message.id().isEmpty())
        messageText.prepend(QString("<span id=\"message_%1\">").arg(Qt::escape(message.id())));
    else
        messageText.prepend("<span>");
    messageText.append("</span>");

    result.replace(QString("%messageId%"), Qt::escape(message.id()));
    result.replace(QString("%messageStatus%"), Qt::escape(QString::number(message.status())));

    result.replace(QString("%message%"), messageText);

    return result;
}

void AdiumStyleRenderer::displayMessageStatus(const QString &id, MessageStatus status)
{
    configuration().webFrame().evaluateJavaScript(
        QString("adium_messageStatusChanged(\"%1\", %2);").arg(Qt::escape(id)).arg(static_cast<int>(status)));
}

void AdiumStyleRenderer::displayChatState(ChatState state, const QString &message, const QString &name)
{
    configuration().webFrame().evaluateJavaScript(QString("adium_contactActivityChanged(%1, \"%2\", \"%3\");")
                                                      .arg(static_cast<int>(state))
                                                      .arg(Qt::escape(message))
                                                      .arg(Qt::escape(name)));
}

void AdiumStyleRenderer::displayChatImage(const ChatImage &chatImage, const QString &fileName)
{
    configuration().webFrame().evaluateJavaScript(QString("adium_chatImageAvailable(\"%1\", \"%2\");")
                                                      .arg(Qt::escape(chatImage.key()))
                                                      .arg(Qt::escape(fileName)));
}
