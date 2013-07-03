/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-top-bar-widget-factory-repository.h"

ChatTopBarWidgetFactoryRepository::ChatTopBarWidgetFactoryRepository(QObject* parent) :
		QObject(parent)
{
}

ChatTopBarWidgetFactoryRepository::~ChatTopBarWidgetFactoryRepository()
{
}

void ChatTopBarWidgetFactoryRepository::registerFactory(ChatTopBarWidgetFactory *factory)
{
	if (Factories.contains(factory))
		return;

	Factories.append(factory);
	emit factoryRegistered(factory);
}

void ChatTopBarWidgetFactoryRepository::unregisterFactory(ChatTopBarWidgetFactory *factory)
{
	if (!Factories.contains(factory))
		return;

	Factories.removeAll(factory);
	emit factoryUnregistered(factory);
}

QList<ChatTopBarWidgetFactory *> ChatTopBarWidgetFactoryRepository::factories() const
{
	return Factories;
}

#include "moc_chat-top-bar-widget-factory-repository.cpp"
