/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

	bool AutoSend;
	bool NiceDateFormat;
	int EmoticonsStyle;
	QColor ChatTextBgColor;
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

public:
	static ChatConfigurationHolder * instance();
	void configurationUpdated();

	bool autoSend() const { return AutoSend; }
	bool niceDateFormat() const { return NiceDateFormat; }
	int emoticonsStyle() const { return EmoticonsStyle; }
	const QColor &chatTextBgColor() const { return ChatTextBgColor; }
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

};

#endif // CHAT_CONFIGURATION_H
