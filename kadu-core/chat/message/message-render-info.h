/*
 * %kadu copyright begin%
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef MESSAGE_RENDER_INFO
#define MESSAGE_RENDER_INFO

#include <QtCore/QDateTime>
#include <QtCore/QString>

#include "chat/message/message.h"

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"

#include "protocols/protocol.h"

#include "exports.h"

class KADUAPI MessageRenderInfo : public QObject
{
	Q_OBJECT

	Message MyMessage;
	QString HtmlMessageContent;

	QString BackgroundColor;
	QString FontColor;
	QString NickColor;

	int SeparatorSize;
	bool ShowServerTime;

public:
	static void registerParserTags();
	static void unregisterParserTags();

	MessageRenderInfo(const Message &msg);
	virtual ~MessageRenderInfo();

	void replaceLoadingImages(const QString &imageId, const QString &imageFileName);

	const Message & message() const { return MyMessage; }
	Message & message() { return MyMessage; }

	const QString & htmlMessageContent() const { return HtmlMessageContent; }

	MessageRenderInfo & setSeparatorSize(int separatorSize);
	int separatorSize() const { return SeparatorSize; }

	MessageRenderInfo & setShowServerTime(bool noServerTime, int noServerTimeDiff);
	bool showServerTime() const { return ShowServerTime; }

	MessageRenderInfo & setBackgroundColor(const QString &backgroundColor);
	const QString & backgroundColor() const { return BackgroundColor; }

	MessageRenderInfo & setFontColor(const QString &fontColor);
	const QString & fontColor() const { return FontColor; }

	MessageRenderInfo & setNickColor(const QString &nickColor);
	const QString & nickColor() const { return NickColor; }

};

QString formatMessage(const QString &text);

#endif // MESSAGE_RENDER_INFO
