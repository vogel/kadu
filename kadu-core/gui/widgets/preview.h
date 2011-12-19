/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef PREVIEW_H
#define PREVIEW_H

#include <QtGui/QFrame>

#include "configuration/configuration-aware-object.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"

#define PREVIEW_DEFAULT_HEIGHT 250

class KaduWebView;
class MessageRenderInfo;

class Preview : public QFrame, public ConfigurationAwareObject
{
	Q_OBJECT

	KaduWebView *WebView;

	QList<MessageRenderInfo *> Messages;

protected:
	virtual void configurationUpdated();

public:
	explicit Preview(QWidget *parent = 0);
	virtual ~Preview();

	void addMessage(MessageRenderInfo *messageRenderInfo);
	const QList<MessageRenderInfo *> & messages() const;

	KaduWebView * webView() const;

public slots:
	void syntaxChanged(const QString &content);

signals:
	void needSyntaxFixup(QString &syntax);
	void needFixup(QString &syntax);

};

#endif // PREVIEW_H
