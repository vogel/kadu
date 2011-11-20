/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHAT_CONFIGURATION_H
#define CHAT_CONFIGURATION_H

#include <QtGui/QColor>
#include <QtGui/QFont>

#include "configuration-holder.h"

class ChatConfigurationHolder : public ConfigurationHolder
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatConfigurationHolder)

	static ChatConfigurationHolder *Instance;
	explicit ChatConfigurationHolder();

	void init();

	bool AutoSend;
	bool NiceDateFormat;
	int EmoticonsStyle;
	bool ChatTextCustomColors;
	QColor ChatTextBgColor;
	QColor ChatTextFontColor;
	bool ForceCustomChatFont;
	QFont ChatFont;
	QString ChatContents;
	QString ConferenceContents;
	QString ConferencePrefix;
	QString MyBackgroundColor;
	QString MyFontColor;
	QString MyNickColor;
	QString UsrBackgroundColor;
	QString UsrFontColor;
	QString UsrNickColor;
	bool ChatBgFilled;
	QColor ChatBgColor;
	bool UseTransparency;

	bool ContactStateChats;
	bool ContactStateWindowTitle;
	int ContactStateWindowTitlePosition;
	QString ContactStateWindowTitleComposingSyntax;

public:
	static ChatConfigurationHolder * instance();
	void configurationUpdated();

	bool autoSend() const { return AutoSend; }
	bool niceDateFormat() const { return NiceDateFormat; }
	int emoticonsStyle() const { return EmoticonsStyle; }
	bool chatTextCustomColors() const { return ChatTextCustomColors; }
	const QColor &chatTextBgColor() const { return ChatTextBgColor; }
	const QColor &chatTextFontColor() const { return ChatTextFontColor; }
	bool forceCustomChatFont() const { return ForceCustomChatFont; }
	const QFont &chatFont() const { return ChatFont; }
	const QString &chatContents() const { return ChatContents; }
	const QString &conferenceContents() const { return ConferenceContents; }
	const QString &conferencePrefix() const { return ConferencePrefix; }
	const QString &myBackgroundColor() const { return MyBackgroundColor; }
	const QString &myFontColor() const { return MyFontColor; }
	const QString &myNickColor() const { return MyNickColor; }
	const QString &usrBackgroundColor() const { return UsrBackgroundColor; }
	const QString &usrFontColor() const { return UsrFontColor; }
	const QString &usrNickColor() const { return UsrNickColor; }
	bool chatBgFilled() const { return ChatBgFilled; }
	const QColor &chatBgColor() const { return ChatBgColor; }
	bool useTransparency() const { return UseTransparency; }

	bool contactStateChats() const { return ContactStateChats; }
	bool contactStateWindowTitle() const { return ContactStateWindowTitle; }
	int contactStateWindowTitlePosition() const { return ContactStateWindowTitlePosition; }
	const QString &contactStateWindowTitleComposingSyntax() const { return ContactStateWindowTitleComposingSyntax; }

signals:
	void chatConfigurationUpdated();

};

#endif // CHAT_CONFIGURATION_H
