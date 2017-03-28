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

#include "console-jump-list.h"

#include "chat/chat.h"

#include <QtCore/QDebug>

ConsoleJumpList::ConsoleJumpList(QObject *parent) : JumpList{parent}
{
}

ConsoleJumpList::~ConsoleJumpList()
{
    clear();
}

void ConsoleJumpList::clear()
{
    qDebug() << "ConsoleJumpList::clear()";
}

void ConsoleJumpList::addChat(Chat chat)
{
    qDebug() << "ConsoleJumpList::addChat(" << chat.uuid().toString() << ")";
}

void ConsoleJumpList::addSeparator()
{
    qDebug() << "ConsoleJumpList::addSeparator()";
}

void ConsoleJumpList::setVisible(bool visible)
{
    qDebug() << "ConsoleJumpList::setVisible(" << visible << ")";
}

#include "console-jump-list.moc"
