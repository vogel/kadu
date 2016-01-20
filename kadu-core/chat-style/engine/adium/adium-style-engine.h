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

#include "chat-style/engine/chat-style-engine.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class InjectedFactory;
class PathsProvider;

class AdiumStyleEngine : public QObject, public ChatStyleEngine
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AdiumStyleEngine(QObject *parent = nullptr);
	virtual ~AdiumStyleEngine();

	virtual std::unique_ptr<ChatStyleRendererFactory> createRendererFactory(const ChatStyle &chatStyle);

	virtual bool supportVariants() { return true; }
	virtual QString isStyleValid(QString styleName);
	virtual QString defaultVariant(const QString &styleName);

	virtual QStringList styleVariants(QString styleName);
	virtual bool styleUsesTransparencyByDefault(QString styleName);

private:
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);

};
