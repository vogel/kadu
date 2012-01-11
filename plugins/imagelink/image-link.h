/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010 Michał Obrembski (byku@byku.com.pl)
 * Copyright 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IMAGE_LINK_H
#define IMAGE_LINK_H

#include <QtCore/QObject>
#include <QtCore/QRegExp>

#include "accounts/accounts-aware-object.h"
#include "chat/chat.h"
#include "contacts/contact.h"

#include "configuration/image-link-configuration.h"

class ChatWidget;

class ImageLink : public QObject, AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ImageLink)

	static ImageLink *Instance;

	ImageLinkConfiguration Configuration;
	QRegExp ImageRegExp;
	QRegExp YouTubeRegExp;

	ImageLink();
	virtual ~ImageLink();

	QString getImageCode(const QString &image);
	QString getVideoCode(const QString &video);

	void insertCodeIntoChatWindow(Chat chat, Contact sender, const QString& code);

private slots:
	void filterIncomingMessage(Chat chat, Contact sender, QString &msg, bool &ignore);

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	static void createInstance();
	static void destroyInstance();
	static ImageLink * instance() { return Instance; }

};

#endif // IMAGE_LINK_H
