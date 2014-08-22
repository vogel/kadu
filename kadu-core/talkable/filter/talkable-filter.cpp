/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "talkable-filter.h"

#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"

TalkableFilter::TalkableFilter(QObject *parent) :
		QObject(parent)
{
}

TalkableFilter::~TalkableFilter()
{
}

TalkableFilter::FilterResult TalkableFilter::filterChat(const Chat &chat)
{
	Q_UNUSED(chat);

	return Undecided;
}

TalkableFilter::FilterResult TalkableFilter::filterBuddy(const Buddy &buddy)
{
	Q_UNUSED(buddy);

	return Undecided;
}

TalkableFilter::FilterResult TalkableFilter::filterContact(const Contact &contact)
{
	Q_UNUSED(contact);

	return Undecided;
}

#include "moc_talkable-filter.cpp"
