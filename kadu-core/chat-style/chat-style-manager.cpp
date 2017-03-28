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

#include "chat-style-manager.h"

#include "chat-style/chat-style-configuration-ui-handler.h"
#include "chat-style/engine/adium/adium-style-engine.h"
#include "chat-style/engine/configured-chat-style-renderer-factory-provider.h"
#include "chat-style/engine/kadu/kadu-style-engine.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "misc/algorithm.h"
#include "misc/memory.h"
#include "misc/paths-provider.h"
#include "widgets/chat-style-preview.h"
#include "widgets/configuration/config-group-box.h"
#include "widgets/configuration/configuration-widget.h"
#include "windows/main-configuration-window.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtGui/QPalette>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

ChatStyleManager::ChatStyleManager(QObject *parent)
        : QObject{parent}, CurrentEngine{}, CfgNoHeaderRepeat{}, CfgHeaderSeparatorHeight{}, CfgNoHeaderInterval{},
          ParagraphSeparator{}, Prune{}, NoServerTime{}, NoServerTimeDiff{}
{
}

ChatStyleManager::~ChatStyleManager()
{
    unregisterChatStyleEngine("Kadu");
    unregisterChatStyleEngine("Adium");
}

void ChatStyleManager::setAdiumStyleEngine(AdiumStyleEngine *adiumStyleEngine)
{
    m_adiumStyleEngine = adiumStyleEngine;
}

void ChatStyleManager::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
    m_chatConfigurationHolder = chatConfigurationHolder;
}

void ChatStyleManager::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ChatStyleManager::setConfiguredChatStyleRendererFactoryProvider(
    ConfiguredChatStyleRendererFactoryProvider *configuredChatStyleRendererFactoryProvider)
{
    m_configuredChatStyleRendererFactoryProvider = configuredChatStyleRendererFactoryProvider;
}

void ChatStyleManager::setKaduStyleEngine(KaduStyleEngine *kaduStyleEngine)
{
    m_kaduStyleEngine = kaduStyleEngine;
}

void ChatStyleManager::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void ChatStyleManager::init()
{
    registerChatStyleEngine("Kadu", m_kaduStyleEngine);
    registerChatStyleEngine("Adium", m_adiumStyleEngine);

    loadStyles();
    configurationUpdated();
}

void ChatStyleManager::registerChatStyleEngine(const QString &name, ChatStyleEngine *engine)
{
    m_engines.insert(std::make_pair(name, engine));
}

void ChatStyleManager::unregisterChatStyleEngine(const QString &name)
{
    m_engines.erase(name);
}

void ChatStyleManager::configurationUpdated()
{
    if (m_configuration->deprecatedApi()->readBoolEntry("Chat", "ChatPrune", true))
        Prune = m_configuration->deprecatedApi()->readNumEntry("Chat", "ChatPruneLen");
    else
    {
        m_configuration->deprecatedApi()->writeEntry("Chat", "ChatPrune", true);
        m_configuration->deprecatedApi()->writeEntry("Chat", "ChatPruneLen", 0);
        Prune = 0;
    }

    ParagraphSeparator = m_configuration->deprecatedApi()->readNumEntry("Look", "ParagraphSeparator");

    QFont font = m_configuration->deprecatedApi()->readFontEntry("Look", "ChatFont");

    QString fontFamily = font.family();
    QString fontSize;
    if (font.pointSize() > 0)
        fontSize = QString::number(font.pointSize()) + "pt";
    else
        fontSize = QString::number(font.pixelSize()) + "px";
    QString fontStyle = font.italic() ? "italic" : "normal";
    QString fontWeight = font.bold() ? "bold" : "normal";
    QString textDecoration = font.underline() ? "underline" : "none";
    QString backgroundColor = "transparent";
    if (m_chatConfigurationHolder->chatBgFilled())
        backgroundColor = m_chatConfigurationHolder->chatBgColor().name();

    MainStyle = QString(
                    "html {"
                    "	font: %1 %2 %3 %4;"
                    "	text-decoration: %5;"
                    "	word-wrap: break-word;"
                    "}"
                    "a {"
                    "	text-decoration: underline;"
                    "}"
                    "body {"
                    "	margin: %6;"
                    "	padding: 0;"
                    "	background-color: %7;"
                    "}"
                    "p {"
                    "	margin: 0;"
                    "	padding: 3px;"
                    "}")
                    .arg(
                        fontStyle, fontWeight, fontSize, fontFamily, textDecoration,
                        QString::number(ParagraphSeparator), backgroundColor);

    CfgNoHeaderRepeat = m_configuration->deprecatedApi()->readBoolEntry("Look", "NoHeaderRepeat", true);

    // headers removal stuff
    if (CfgNoHeaderRepeat)
    {
        CfgHeaderSeparatorHeight = m_configuration->deprecatedApi()->readNumEntry("Look", "HeaderSeparatorHeight");
        CfgNoHeaderInterval = m_configuration->deprecatedApi()->readNumEntry("Look", "NoHeaderInterval");
    }
    else
    {
        CfgHeaderSeparatorHeight = 0;
        CfgNoHeaderInterval = 0;
    }

    NoServerTime = m_configuration->deprecatedApi()->readBoolEntry("Look", "NoServerTime");
    NoServerTimeDiff = m_configuration->deprecatedApi()->readNumEntry("Look", "NoServerTimeDiff");

    auto newChatStyle = ChatStyle{m_configuration->deprecatedApi()->readEntry("Look", "Style"),
                                  m_configuration->deprecatedApi()->readEntry("Look", "ChatStyleVariant")};

    // if Style was changed, load new Style
    if (!CurrentEngine || newChatStyle != m_currentChatStyle)
    {
        auto newStyleName = fixedStyleName(newChatStyle.name());
        CurrentEngine = AvailableStyles.value(newStyleName).engine;
        auto newVariantName = fixedVariantName(newStyleName, newChatStyle.variant());
        m_currentChatStyle = {newStyleName, newVariantName};

        if (m_configuredChatStyleRendererFactoryProvider)
            m_configuredChatStyleRendererFactoryProvider->setChatStyleRendererFactory(
                CurrentEngine->createRendererFactory(m_currentChatStyle));
    }

    emit chatStyleConfigurationUpdated();
}

QString ChatStyleManager::fixedStyleName(QString styleName)
{
    if (!AvailableStyles.contains(styleName))
    {
        styleName = "Satin";
        if (!AvailableStyles.contains(styleName))
        {
            styleName = "kadu";
            if (!AvailableStyles.contains(styleName))
                styleName = *AvailableStyles.keys().constBegin();
        }
    }

    return styleName;
}

QString ChatStyleManager::fixedVariantName(const QString &styleName, QString variantName)
{
    if (!CurrentEngine->styleVariants(styleName).contains(variantName))
        return CurrentEngine->defaultVariant(styleName);

    return variantName;
}

// any better ideas?
void ChatStyleManager::loadStyles()
{
    QDir dir;
    QString path, StyleName;
    QFileInfo fi;
    QStringList files;

    path = m_pathsProvider->profilePath() + QStringLiteral("syntax/chat/");
    dir.setPath(path);

    files = dir.entryList();

    AvailableStyles.clear();   // allow reloading of styles

    foreach (const QString &file, files)
    {
        fi.setFile(path + file);
        if (fi.isReadable() && !AvailableStyles.contains(file))
        {
            for (auto &&engine : m_engines)
            {
                StyleName = engine.second->isStyleValid(path + file);
                if (!StyleName.isNull())
                {
                    AvailableStyles[StyleName].engine = engine.second;
                    AvailableStyles[StyleName].global = false;
                    break;
                }
            }
        }
    }

    path = m_pathsProvider->dataPath() + QStringLiteral("syntax/chat/");
    dir.setPath(path);

    files = dir.entryList();

    foreach (const QString &file, files)
    {
        fi.setFile(path + file);
        if (fi.isReadable() && !AvailableStyles.contains(file))
        {
            for (auto &&engine : m_engines)
            {
                StyleName = engine.second->isStyleValid(path + file);
                if (!StyleName.isNull())
                {
                    AvailableStyles[StyleName].engine = engine.second;
                    AvailableStyles[StyleName].global = true;
                    break;
                }
            }
        }
    }
}

bool ChatStyleManager::isChatStyleValid(const QString &name) const
{
    return AvailableStyles.contains(name) && AvailableStyles.value(name).engine;
}

StyleInfo ChatStyleManager::chatStyleInfo(const QString &name) const
{
    if (AvailableStyles.contains(name))
        return AvailableStyles.value(name);
    else
        return StyleInfo();
}

#include "moc_chat-style-manager.cpp"
