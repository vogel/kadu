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

#include "chat-style/chat-style.h"
#include "configuration/configuration-aware-object.h"

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtGui/QColor>
#include <injeqt/injeqt.h>
#include <map>
#include <memory>

class AdiumStyleEngine;
class ChatConfigurationHolder;
class ChatStyleEngine;
class Configuration;
class ConfiguredChatStyleRendererFactoryProvider;
class KaduStyleEngine;
class PathsProvider;

class QCheckBox;
class QComboBox;

// TODO: review
struct StyleInfo
{
    bool global;
    ChatStyleEngine *engine;

    StyleInfo() : global(false), engine(0)
    {
    }
};

class KADUAPI ChatStyleManager : public QObject, ConfigurationAwareObject
{
    Q_OBJECT

    QPointer<AdiumStyleEngine> m_adiumStyleEngine;
    QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
    QPointer<Configuration> m_configuration;
    QPointer<ConfiguredChatStyleRendererFactoryProvider> m_configuredChatStyleRendererFactoryProvider;
    QPointer<KaduStyleEngine> m_kaduStyleEngine;
    QPointer<PathsProvider> m_pathsProvider;

    std::map<QString, ChatStyleEngine *> m_engines;
    ChatStyle m_currentChatStyle;
    QMap<QString, StyleInfo> AvailableStyles;

    ChatStyleEngine *CurrentEngine;

    bool CfgNoHeaderRepeat;       /*!< Remove repeated message headers. */
    int CfgHeaderSeparatorHeight; /*!< Header separator height. */
    int CfgNoHeaderInterval;      /*!< Time Interval, in which headers will not be repeated*/
    int ParagraphSeparator;       /*!< Message separator height. */

    int Prune; /*!< Maximal number of visible messages. */

    bool NoServerTime;    /*!< Remove server time */
    int NoServerTimeDiff; /*!< Maximal time difference between server time and local time, for which server time will be
                             removed */

    QString MainStyle;

    QString fixedStyleName(QString styleName);
    QString fixedVariantName(const QString &styleName, QString variantName);

private slots:
    INJEQT_SET void setAdiumStyleEngine(AdiumStyleEngine *adiumStyleEngine);
    INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setConfiguredChatStyleRendererFactoryProvider(
        ConfiguredChatStyleRendererFactoryProvider *configuredChatStyleRendererFactoryProvider);
    INJEQT_SET void setKaduStyleEngine(KaduStyleEngine *kaduStyleEngine);
    INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
    INJEQT_INIT void init();

protected:
    virtual void configurationUpdated();

public:
    Q_INVOKABLE explicit ChatStyleManager(QObject *parent = nullptr);
    virtual ~ChatStyleManager();

    void registerChatStyleEngine(const QString &name, ChatStyleEngine *engine);
    void unregisterChatStyleEngine(const QString &name);

    ChatStyle currentChatStyle() const
    {
        return m_currentChatStyle;
    }
    ChatStyleEngine *currentEngine() const
    {
        return CurrentEngine;
    }

    QMap<QString, StyleInfo> availableStyles() const
    {
        return AvailableStyles;
    }
    bool hasChatStyle(const QString &name) const
    {
        return AvailableStyles.contains(name);
    }
    bool isChatStyleValid(const QString &name) const;
    StyleInfo chatStyleInfo(const QString &name) const;

    void loadStyles();

    bool cfgNoHeaderRepeat()
    {
        return CfgNoHeaderRepeat;
    }
    int cfgHeaderSeparatorHeight()
    {
        return CfgHeaderSeparatorHeight;
    }
    int cfgNoHeaderInterval()
    {
        return CfgNoHeaderInterval;
    }
    int paragraphSeparator()
    {
        return ParagraphSeparator;
    }

    bool noServerTime()
    {
        return NoServerTime;
    }
    int noServerTimeDiff()
    {
        return NoServerTimeDiff;
    }

    int prune()
    {
        return Prune;
    }

    const QString &mainStyle()
    {
        return MainStyle;
    }

signals:
    void chatStyleConfigurationUpdated();
};
