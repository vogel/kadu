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

#include "configuration/configuration-holder.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <injeqt/injeqt.h>

enum class ChatWidgetTitleComposingStatePosition;

class Configuration;

class KADUAPI ChatConfigurationHolder : public ConfigurationHolder
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ChatConfigurationHolder(QObject *parent = nullptr);
    virtual ~ChatConfigurationHolder();

    void configurationUpdated();

    bool autoSend() const
    {
        return AutoSend;
    }
    bool niceDateFormat() const
    {
        return NiceDateFormat;
    }
    bool chatTextCustomColors() const
    {
        return ChatTextCustomColors;
    }
    const QColor &chatTextBgColor() const
    {
        return ChatTextBgColor;
    }
    const QColor &chatTextFontColor() const
    {
        return ChatTextFontColor;
    }
    bool forceCustomChatFont() const
    {
        return ForceCustomChatFont;
    }
    const QFont &chatFont() const
    {
        return ChatFont;
    }
    const QString &myBackgroundColor() const
    {
        return MyBackgroundColor;
    }
    const QString &myFontColor() const
    {
        return MyFontColor;
    }
    const QString &myNickColor() const
    {
        return MyNickColor;
    }
    const QString &usrBackgroundColor() const
    {
        return UsrBackgroundColor;
    }
    const QString &usrFontColor() const
    {
        return UsrFontColor;
    }
    const QString &usrNickColor() const
    {
        return UsrNickColor;
    }
    bool chatBgFilled() const
    {
        return ChatBgFilled;
    }
    const QColor &chatBgColor() const
    {
        return ChatBgColor;
    }
    bool useTransparency() const
    {
        return UseTransparency;
    }

    bool contactStateChats() const
    {
        return ContactStateChats;
    }
    bool contactStateWindowTitle() const
    {
        return ContactStateWindowTitle;
    }
    int contactStateWindowTitlePosition() const
    {
        return ContactStateWindowTitlePosition;
    }
    ChatWidgetTitleComposingStatePosition composingStatePosition() const;

signals:
    void chatConfigurationUpdated();

private:
    QPointer<Configuration> m_configuration;

    bool AutoSend;
    bool NiceDateFormat;
    bool ChatTextCustomColors;
    QColor ChatTextBgColor;
    QColor ChatTextFontColor;
    bool ForceCustomChatFont;
    QFont ChatFont;
    QString MyBackgroundColor;
    QString MyFontColor;
    QString MyNickColor;
    QString UsrBackgroundColor;
    QString UsrFontColor;
    QString UsrNickColor;
    bool ChatBgFilled;
    QColor ChatBgColor;
    bool UseTransparency;

    bool ContactStateChats;
    bool ContactStateWindowTitle;
    int ContactStateWindowTitlePosition;

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_INIT void init();
};
