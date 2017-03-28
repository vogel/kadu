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

#include "windows-jump-list.h"

#include "chat/chat.h"

#include <QtCore/QDebug>
#include <QtWinExtras/QtWinExtras>

WindowsJumpList::WindowsJumpList(QObject *parent) : JumpList{parent}
{
    m_jumpList = make_owned<QWinJumpList>(this);
}

WindowsJumpList::~WindowsJumpList()
{
    clear();
}

void WindowsJumpList::clear()
{
    m_jumpList->tasks()->clear();
}

void WindowsJumpList::addChat(Chat chat)
{
    auto title = chat.display().isEmpty() ? chat.name() : chat.display();
    m_jumpList->tasks()->addLink(
        title, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()),
        QStringList{"--open-uuid", chat.uuid().toString()});
}

void WindowsJumpList::addSeparator()
{
    m_jumpList->tasks()->addSeparator();
}

void WindowsJumpList::setVisible(bool visible)
{
    m_jumpList->tasks()->setVisible(visible);
}

#include "windows-jump-list.moc"
