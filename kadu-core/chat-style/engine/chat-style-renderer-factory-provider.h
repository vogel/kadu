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

#include <QtCore/QObject>
#include <memory>

class ChatStyleRendererFactory;

/**
 * @addtogroup ChatStyle
 * @{
 */

/**
 * @class ChatStyleRendererFactoryProvider
 * @short Provider that contains current instance of ChatStyleRendererFactory.
 */
class ChatStyleRendererFactoryProvider : public QObject
{
	Q_OBJECT

public:
	explicit ChatStyleRendererFactoryProvider(QObject *parent = nullptr);
	virtual ~ChatStyleRendererFactoryProvider();

	/**
	 * @return Current instance of ChatStyleRendererFactory.
	 */
	virtual std::shared_ptr<ChatStyleRendererFactory> chatStyleRendererFactory() const = 0;

signals:
	/**
	 * @short Signal emited when instance of ChatStyleRendererFactory is replaced by other one.
	 * @param chatStyleRenderer new instance of ChatStyleRendererFactory.
	 */
	void chatStyleRendererFactoryChanged(std::shared_ptr<ChatStyleRendererFactory> chatStyleRenderer);

};

/**
 * @}
 */
