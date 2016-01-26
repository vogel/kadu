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
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>
#include <injeqt/injeqt.h>

#include "exports.h"

class QDomDocument;

class ClipboardHtmlTransformerService;
class Configuration;
class DomProcessorService;
class MailUrlDomVisitorProvider;
class MailUrlHandler;
class StandardUrlDomVisitorProvider;
class StandardUrlHandler;
class UrlClipboardHtmlTransformer;
class UrlHandler;
class UrlOpener;

class KADUAPI UrlHandlerManager : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit UrlHandlerManager(QObject *parent = nullptr);
	virtual ~UrlHandlerManager();

	void registerUrlHandler(UrlHandler *handler);
	void unregisterUrlHandler(UrlHandler *handler);

	void openUrl(const QByteArray &url, bool disableMenu = false);

	// TODO:
	//for mail validation:
	const QRegExp & mailRegExp();

	//for link validation
	const QRegExp & urlRegExp();

private:
	QPointer<ClipboardHtmlTransformerService> m_clipboardHtmlTransformerService;
	QPointer<Configuration> m_configuration;
	QPointer<DomProcessorService> m_domProcessorService;
	QPointer<UrlOpener> m_urlOpener;

	QList<UrlHandler *> RegisteredHandlers;

	StandardUrlDomVisitorProvider *StandardUrlVisitorProvider;
	MailUrlDomVisitorProvider *MailUrlVisitorProvider;

	StandardUrlHandler *standardUrlHandler;
	MailUrlHandler *mailUrlHandler;

	QScopedPointer<UrlClipboardHtmlTransformer> ClipboardTransformer;

private slots:
	INJEQT_SET void setClipboardHtmlTransformerService(ClipboardHtmlTransformerService *clipboardHtmlTransformerService);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SET void setUrlOpener(UrlOpener *urlOpener);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

};
