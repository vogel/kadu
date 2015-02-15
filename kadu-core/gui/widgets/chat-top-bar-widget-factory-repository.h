/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_TOP_BAR_WIDGET_FACTORY_REPOSITORY_H
#define CHAT_TOP_BAR_WIDGET_FACTORY_REPOSITORY_H

#include <QtCore/QObject>
#include "exports.h"

class ChatTopBarWidgetFactory;

class KADUAPI ChatTopBarWidgetFactoryRepository : public QObject
{
	Q_OBJECT

	QList<ChatTopBarWidgetFactory *> Factories;

public:
	explicit ChatTopBarWidgetFactoryRepository(QObject *parent = 0);
	virtual ~ChatTopBarWidgetFactoryRepository();

	void registerFactory(ChatTopBarWidgetFactory *factory);
	void unregisterFactory(ChatTopBarWidgetFactory *factory);

	QList<ChatTopBarWidgetFactory *> factories() const;

signals:
	void factoryRegistered(ChatTopBarWidgetFactory *factory);
	void factoryUnregistered(ChatTopBarWidgetFactory *factory);

};

#endif // CHAT_TOP_BAR_WIDGET_FACTORY_REPOSITORY_H
