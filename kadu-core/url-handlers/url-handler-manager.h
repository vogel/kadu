/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include "exports.h"

class QDomDocument;

class MailUrlDomVisitorProvider;
class MailUrlHandler;
class StandardUrlDomVisitorProvider;
class StandardUrlHandler;
class UrlClipboardHtmlTransformer;
class UrlHandler;

class KADUAPI UrlHandlerManager : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit UrlHandlerManager(QObject *parent = nullptr);
	virtual ~UrlHandlerManager();

	void registerUrlHandler(const QString &name, UrlHandler *handler);
	void unregisterUrlHandler(const QString &name);

	void openUrl(const QByteArray &url, bool disableMenu = false);

	// TODO:
	//for mail validation:
	const QRegExp & mailRegExp();

	//for link validation
	const QRegExp & urlRegExp();

private:
	QMap<QString, UrlHandler *> RegisteredHandlers;
	QList<UrlHandler *> RegisteredHandlersByPriority;

	StandardUrlDomVisitorProvider *StandardUrlVisitorProvider;
	MailUrlDomVisitorProvider *MailUrlVisitorProvider;

	StandardUrlHandler *standardUrlHandler;
	MailUrlHandler *mailUrlHandler;

	QScopedPointer<UrlClipboardHtmlTransformer> ClipboardTransformer;

	void registerUrlClipboardTransformer();
	void unregisterUrlClipboardTransformer();

};
