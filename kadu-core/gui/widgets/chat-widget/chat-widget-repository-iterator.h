/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <map>
#include <memory>

class Chat;
class ChatWidget;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetRepositoryIterator
 * @short Iterator for ChatWidget* instances in ChatWidgetRepository.
 */
class ChatWidgetRepositoryIterator
{
	using Iterator = std::map<Chat, std::unique_ptr<ChatWidget>>::iterator;

public:
	inline explicit ChatWidgetRepositoryIterator(Iterator iterator) : m_iterator{iterator} {}

	inline bool operator != (const ChatWidgetRepositoryIterator &compareTo) const
	{
		return m_iterator != compareTo.m_iterator;
	}

	inline ChatWidgetRepositoryIterator & operator++()
	{
		++m_iterator;
		return *this;
	}

	inline ChatWidget * operator*() const
	{
		return m_iterator->second.get();
	}

private:
	Iterator m_iterator;

};

/**
 * @}
 */
