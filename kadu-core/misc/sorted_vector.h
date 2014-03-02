/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <algorithm>
#include <functional>
#include <vector>

template<typename T, bool (*LessThanComparator)(const T &, const T &), bool (*EqualityComparator)(const T &, const T &)>
class KADUAPI sorted_vector
{

public:
	using This = sorted_vector<T, LessThanComparator, EqualityComparator>;
	using Storage = std::vector<T>;
	using size_type = typename Storage::size_type;

	sorted_vector() {}
	sorted_vector(Storage content) :
			m_content{std::move(content)}
	{
		std::stable_sort(std::begin(m_content), std::end(m_content), LessThanComparator);
	}

	void add(T item)
	{
		if (m_content.empty())
		{
			m_content.emplace_back(std::move(item));
			return;
		}

		auto upperBound = std::upper_bound(std::begin(m_content), std::end(m_content), item, LessThanComparator);
		auto previous = *(upperBound - 1);
		if (!EqualityComparator(previous, item))
			m_content.emplace(upperBound, std::move(item));
	}

	void add(const This &sortedVector)
	{
		auto result = Storage{};

		std::merge(std::begin(m_content), std::end(m_content),
			std::begin(sortedVector.m_content), std::end(sortedVector.m_content),
			std::back_inserter(result), LessThanComparator);
		result.erase(std::unique(std::begin(result), std::end(result), EqualityComparator), std::end(result));

		m_content = std::move(result);
	}

	const Storage & content() const
	{
		return m_content;
	}

	bool empty() const
	{
		return m_content.empty();
	}

	typename Storage::size_type size() const
	{
		return m_content.size();
	}

	void clear()
	{
		m_content.clear();
	}

private:
	Storage m_content;

};

template<typename T, bool (*LessThanComparator)(const T &, const T &), bool (*EqualityComparator)(const T &, const T &)>
KADUAPI typename sorted_vector<T, LessThanComparator, EqualityComparator>::Storage::const_iterator begin(const sorted_vector<T, LessThanComparator, EqualityComparator> &sortedVector)
{
	return std::begin(sortedVector.content());
}

template<typename T, bool (*LessThanComparator)(const T &, const T &), bool (*EqualityComparator)(const T &, const T &)>
KADUAPI typename sorted_vector<T, LessThanComparator, EqualityComparator>::Storage::const_iterator end(const sorted_vector<T, LessThanComparator, EqualityComparator> &sortedVector)
{
	return std::end(sortedVector.content());
}
