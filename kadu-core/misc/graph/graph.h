/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/algorithm.h"
#include "misc/graph/graph-node.h"
#include "misc/memory.h"
#include "exports.h"

#include <map>
#include <memory>

template<typename P, typename... SuccessorTypeTags>
class Graph
{

public:
	using Type = Graph<P, SuccessorTypeTags...>;
	using Pointer = Type*;
	using PayloadType = P;
	using NodeType = GraphNode<P, SuccessorTypeTags...>;
	using NodePointer = NodeType*;
	using Storage = std::map<PayloadType, std::unique_ptr<NodeType>>;

	Graph() = default;
	Graph(const Graph &) = delete;
	Graph(Graph &&) = default;
	Graph & operator = (const Graph &) = delete;
	Graph & operator = (Graph &&) = default;

	const Storage & nodes() const
	{
		return m_nodes;
	}

	NodePointer addNode(PayloadType payload)
	{
		if (contains(m_nodes, payload))
			return m_nodes.at(payload).get();

		auto node = make_unique<NodeType>(payload);
		auto result = node.get();
		m_nodes.insert(std::make_pair(payload, std::move(node)));
		return result;
	}

	template<typename SuccessorTypeTag>
	void addEdge(const PayloadType &from, const PayloadType &to)
	{
		auto nodeFrom = addNode(from);
		auto nodeTo = addNode(to);

		nodeFrom->template addSuccessor<SuccessorTypeTag>(nodeTo);
	}

	NodePointer node(const PayloadType &payload) const
	{
		return contains(m_nodes, payload)
				? m_nodes.at(payload).get()
				: nullptr;
	}

private:
	Storage m_nodes;

};
