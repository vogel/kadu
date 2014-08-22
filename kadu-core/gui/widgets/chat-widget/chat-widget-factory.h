/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <memory>
#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "exports.h"

class Chat;
class ChatWidget;
class FormattedStringFactory;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetFactory
 * @short Factory for ChatWidget instances
 */
class KADUAPI ChatWidgetFactory : public QObject
{
	Q_OBJECT

public:
	explicit ChatWidgetFactory(QObject *parent = 0);
	virtual ~ChatWidgetFactory();

	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	std::unique_ptr<ChatWidget> createChatWidget(Chat chat);

private:
	QPointer<FormattedStringFactory> m_formattedStringFactory;

};

/**
 * @}
 */
