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

/**
 * @addtogroup Misc
 * @{
 */

/**
 * @class sorted_unique_vector
 * @short Vector that stored only unique values thata are always sorted.
 * @param T type of data
 * @param LessThanComparator comparator used for sorting
 * @param EqualityComparator comparator used for uniqueness testing
 */
template<typename T, bool (*LessThanComparator)(const T &, const T &), bool (*EqualityComparator)(const T &, const T &)>
class KADUAPI sorted_unique_vector
{

public:
	using This = sorted_unique_vector<T, LessThanComparator, EqualityComparator>;
	using Storage = std::vector<T>;
	using size_type = typename Storage::size_type;

	/**
	 * @short Create empty sorted_unique_vector.
	 */
	sorted_unique_vector() {}

	/**
	 * @short Create sorted_unique_vector from given vector.
	 * @param storage vector to get data from
	 *
	 * Copies content of storage, sorts it and removes duplicates.
	 */
	sorted_unique_vector(Storage storage) :
			m_content{std::move(storage)}
	{
		std::stable_sort(std::begin(m_content), std::end(m_content), LessThanComparator);
		ensureUnique(m_content);
	}

	/**
	 * @short Add new item to sorted vector.
	 * @param item new item
	 *
	 * Item is added at proper place so vector remains sorted. Item will not be
	 * added if another one that compares equal (using EqualityComparator)
	 * already exists.
	 */
	void add(T item)
	{
		if (m_content.empty())
		{
			m_content.emplace_back(std::move(item));
			return;
		}

		auto upperBound = std::upper_bound(std::begin(m_content), std::end(m_content), item, LessThanComparator);
		if (upperBound == std::begin(m_content) || !EqualityComparator(*(upperBound - 1), item))
			m_content.emplace(upperBound, std::move(item));
	}

	/**
	 * @short Merge with another sorted vector.
	 * @param sortedVector vector to merge with
	 *
	 * All items from sortedVector are added at proper places and duplicates are removed.
	 */
	void merge(const This &sortedVector)
	{
		auto result = Storage{};

		std::merge(std::begin(m_content), std::end(m_content),
			std::begin(sortedVector.m_content), std::end(sortedVector.m_content),
			std::back_inserter(result), LessThanComparator);
		ensureUnique(result);

		m_content = std::move(result);
	}

	/**
	 * @return Data stored in sorted vector.
	 */
	const Storage & content() const
	{
		return m_content;
	}

	/**
	 * @return true if no data is stored
	 */
	bool empty() const
	{
		return m_content.empty();
	}

	/**
	 * @return number of stored items
	 */
	typename Storage::size_type size() const
	{
		return m_content.size();
	}

	/**
	 * @short Removes all items from sorted vector.
	 */
	void clear()
	{
		m_content.clear();
	}

private:
	Storage m_content;

	void ensureUnique(Storage &storage)
	{
		storage.erase(std::unique(std::begin(storage), std::end(storage), EqualityComparator), std::end(storage));
	}

};

/**
 * @return begin iterator to content of sorted_unique_vector.
 */
template<typename T, bool (*LessThanComparator)(const T &, const T &), bool (*EqualityComparator)(const T &, const T &)>
typename sorted_unique_vector<T, LessThanComparator, EqualityComparator>::Storage::const_iterator begin(const sorted_unique_vector<T, LessThanComparator, EqualityComparator> &sortedVector)
{
	return std::begin(sortedVector.content());
}

/**
 * @return end iterator to content of sorted_unique_vector.
 */
template<typename T, bool (*LessThanComparator)(const T &, const T &), bool (*EqualityComparator)(const T &, const T &)>
typename sorted_unique_vector<T, LessThanComparator, EqualityComparator>::Storage::const_iterator end(const sorted_unique_vector<T, LessThanComparator, EqualityComparator> &sortedVector)
{
	return std::end(sortedVector.content());
}

/**
 * @}
 */
