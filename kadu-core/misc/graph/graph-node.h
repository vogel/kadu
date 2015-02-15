/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <vector>

template<typename P, typename... SuccessorTypeTags>
class GraphNode
{
	template<int I, class T, class Head, class... Tail>
	struct TypeIndex
	{
		typedef typename TypeIndex<I + 1, T, Tail...>::type type;
		static constexpr int i = I;
	};

	template<int I, class T, class... Tail>
	struct TypeIndex<I, T, T, Tail...>
	{
		typedef TypeIndex type;
		static constexpr int i = I;
	};

public:
	using Type = GraphNode<P, SuccessorTypeTags...>;
	using Pointer = Type*;

	explicit GraphNode(P payload) :
			m_payload{payload}
	{
		m_successors.resize(sizeof...(SuccessorTypeTags));
	}

	template<typename SuccessorTypeTag>
	void addSuccessor(Pointer successor)
	{
		m_successors.at(TypeIndex<0, SuccessorTypeTag, SuccessorTypeTags...>::type::i).push_back(successor);
	}

	template<typename SuccessorTypeTag>
	std::vector<Pointer> successors() const
	{
		return m_successors.at(TypeIndex<0, SuccessorTypeTag, SuccessorTypeTags...>::type::i);
	}

	P payload() const { return m_payload; }

private:
	std::vector<std::vector<Pointer>> m_successors;
	P m_payload;

};
