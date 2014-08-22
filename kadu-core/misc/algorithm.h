/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <algorithm>
#include <map>

/**
 * @addtogroup Algorithm
 * @{
 */

/**
 * @param c container to search
 * @param v value to look for
 * @return true if value v can be found in cointainer c
 */
template<typename C, typename T>
bool contains(const C &c, const T &v)
{
	return std::find(c.begin(), c.end(), v) != c.end();
}

/**
 * @param m map to search
 * @param k key to look for
 * @return true if key k can be found in map m
 */
template<typename K, typename V>
bool contains(const std::map<K, V> &m, const K &k)
{
	return m.find(k) != m.end();
}

/**
 * @param c container to search
 * @param v1 first value
 * @param v2 second value
 * @return true if value v1 precedes v2 inin cointainer c
 *
 * If v1 or v2 can not be found in container c, the result is undefined.
 */
template<typename C, typename T>
bool precedes(const C &c, const T &v1, const T &v2)
{
	auto it1 = std::find(c.begin(), c.end(), v1);
	auto it2 = std::find(c.begin(), c.end(), v2);

	return it1 < it2;
}

/**
 * @short Finds overlaping region of sequences.
 * @param first1 begin of first sequence
 * @param last1 end of first sequence
 * @param first2 begin of second sequence
 * @param last2 end of second sequence
 * @return Iterators covering overlaping region.
 *
 * Two sequences A and B overlaps when there exists such s for each k=0..s-1
 * A[end(A)-s+k] = B[k]. This functions returns pair such as its first element points
 * to A[end(A)-s] and second one points to B[k+1].
 *
 * In case there is no overlap {end(A), begin(B)} is returned
 */
template <typename ForwardIterator1, typename ForwardIterator2>
std::pair<ForwardIterator1, ForwardIterator2> find_overlapping_region(ForwardIterator1 first1, ForwardIterator1 last1,
		ForwardIterator2 first2, ForwardIterator2 last2)
{
	if (first1 == last1 || first2 == last2)
		return std::make_pair(last1, first2);

	auto result1 = std::find(first1, last1, *first2);
	if (result1 == last1)
		return std::make_pair(last1, first2);

	auto it1 = result1;
	auto it2 = first2;
	while (it1 != last1 && it2 != last2)
		if (*it1++ != *it2++)
			break;

	if (it1 == last1)
		return std::make_pair(result1, it2);
	else
		return std::make_pair(last1, first2);
}

/**
 * @}
 **/
