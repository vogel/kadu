/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2006, 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#include "parser/parser-data.h"

#include "buddies/buddy-list.h"
#include "buddies/buddy.h"
#include "message/message.h"
#include "protocols/protocol.h"
#include "exports.h"

#include <QtCore/QString>

class KADUAPI MessageRenderInfo : public ParserData
{

public:
	static void registerParserTags();
	static void unregisterParserTags();

	MessageRenderInfo(Message message, QString backgroundColor, QString fontColor, QString nickColor,
			bool includeHeader, int separatorSize, bool showServerTime);
	virtual ~MessageRenderInfo();

	Message message() const;
	QString backgroundColor() const;
	QString fontColor() const;
	QString nickColor() const;
	bool includeHeader() const;
	int separatorSize() const;
	bool showServerTime() const;

private:
	Message m_message;
	QString m_backgroundColor;
	QString m_fontColor;
	QString m_nickColor;
	bool m_includeHeader;
	int m_separatorSize;
	bool m_showServerTime;

};
