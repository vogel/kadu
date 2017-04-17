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

#include "mediaplayer-menu-action.h"
#include "mediaplayer-menu-action.moc"

#include "mediaplayer.h"

#include "widgets/chat-edit-box.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

MediaplayerMenuAction::MediaplayerMenuAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"external_modules/mediaplayer"});
    setName(QStringLiteral("mediaplayer_button"));
    setText(tr("MediaPlayer"));
    setType(ActionDescription::TypeChat);
}

MediaplayerMenuAction::~MediaplayerMenuAction()
{
}

void MediaplayerMenuAction::setMediaPlayer(MediaPlayer *mediaPlayer)
{
    m_mediaPlayer = mediaPlayer;
}

void MediaplayerMenuAction::actionTriggered(QAction *sender, bool)
{
    auto chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
    if (!chatEditBox)
        return;

    auto chatWidget = chatEditBox->chatWidget();
    if (chatWidget)
    {
        auto widgets = sender->associatedWidgets();
        if (widgets.isEmpty())
            return;

        auto widget = widgets[widgets.size() - 1];
        m_mediaPlayer->menu()->popup(widget->mapToGlobal(QPoint(0, widget->height())));
    }
}
