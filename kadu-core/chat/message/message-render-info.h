/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_RENDER_INFO
#define MESSAGE_RENDER_INFO

#include <QtCore/QDateTime>
#include <QtCore/QString>

#include "chat/message/message.h"

#include "contacts/contact.h"
#include "contacts/contact-list.h"

#include "protocols/protocol.h"

#include "emoticons.h"

#include "exports.h"

class KADUAPI MessageRenderInfo : public QObject
{
	Q_OBJECT

	Message MyMessage;
	int SeparatorSize;
	bool ShowServerTime;

public:
	QString unformattedMessage;
	QString backgroundColor;
	QString fontColor;
	QString nickColor;

	static void registerParserTags();
	static void unregisterParserTags();

	MessageRenderInfo(const Message &msg);

	Message message() const { return MyMessage; }

	void replaceLoadingImages(const QString &imageId, const QString &imagePath);

	void setSeparatorSize(int separatorSize) { SeparatorSize = separatorSize; }
	int separatorSize() const { return SeparatorSize; }

	void setShowServerTime(bool noServerTime, int noServerTimeDiff);
	bool showServerTime() const { return ShowServerTime; }

	void setColorsAndBackground(const QString &backgroundColor, const QString &nickColor, const QString &fontColor);

signals:
	void statusChanged(Message::Status);

};

QString formatMessage(const QString &text, const QString &backgroundColor);

#endif // MESSAGE_RENDER_INFO
