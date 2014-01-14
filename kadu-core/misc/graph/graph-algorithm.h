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

#include "misc/algorithm.h"

#include <deque>
#include <map>
#include <set>
#include <utility>

template<typename GraphType, typename SuccessorTypeTag>
class GraphCycleFinder
{
	using NodePointer = typename GraphType::NodePointer;

public:
	explicit GraphCycleFinder(const GraphType &graph) : m_index{0}
	{
		for (auto &node : graph.nodes())
			if (!isVisited(node.second.get()))
				strongConnect(node.second.get());
	}

	std::set<NodePointer> result() const
	{
		return m_result;
	}

private:
	std::map<NodePointer, std::pair<int, int>> m_indexes;
	std::deque<NodePointer> m_currnentPath;
	std::set<NodePointer> m_result;
	int m_index;

	bool isVisited(NodePointer node) const
	{
		return m_indexes.count(node) > 0;
	}

	bool isOnCurrentPath(NodePointer node) const
	{
		return contains(m_currnentPath, node);
	}

	bool isRootNode(NodePointer node) const
	{
		return m_indexes.at(node).first == m_indexes.at(node).second;
	}

	std::vector<NodePointer> getCurrentCycle(NodePointer node)
	{
		auto result = std::vector<NodePointer>{};
		while (!m_currnentPath.empty())
		{
			auto stackNode = m_currnentPath.back();
			result.push_back(stackNode);
			m_currnentPath.pop_back();

			if (node == stackNode)
				return result;
		}

		return result;
	}

	void strongConnect(NodePointer node)
	{
		m_indexes.insert({node, std::make_pair(m_index, m_index)});
		m_index++;
		m_currnentPath.push_back(node);

		for (auto successor : node->template successors<SuccessorTypeTag>())
		{
			if (!isVisited(successor))
			{
				strongConnect(successor);
				m_indexes.at(node).second = std::min(m_indexes.at(node).second, m_indexes.at(successor).second);
			}
			else if (isOnCurrentPath(successor))
				m_indexes.at(node).second = std::min(m_indexes.at(node).second, m_indexes.at(successor).first);
		}

		if (isRootNode(node))
		{
			auto cycle = getCurrentCycle(node);
			if (cycle.size() > 1)
				std::copy(cycle.begin(), cycle.end(), std::inserter(m_result, m_result.end()));
		}
	}

};

template<typename SuccessorTypeTag, typename GraphType>
std::set<typename GraphType::PayloadType> graph_find_cycles(const GraphType &graph)
{
	auto result = std::set<typename GraphType::PayloadType>{};
	auto cycleFinder = GraphCycleFinder<GraphType, SuccessorTypeTag>{graph};
	auto cycleNodes = cycleFinder.result();

	std::transform(cycleNodes.begin(), cycleNodes.end(), std::inserter(result, result.begin()), [](typename GraphType::NodePointer node){
		return node->payload();
	});

	return result;
}

class GraphCycleException : public std::exception
{
	virtual const char * what() const noexcept { return "Found cycle in graph"; }
};

template<typename GraphType, typename SuccessorTypeTag>
class GraphSuccessorSorter
{
	using PayloadType = typename GraphType::PayloadType;
	using NodePointer = typename GraphType::NodePointer;

public:
	explicit GraphSuccessorSorter(const GraphType &graph, const PayloadType &payload) noexcept(false)
	{
		auto node = graph.node(payload);
		if (node)
			visitNode(node);
	}

	std::vector<NodePointer> result() const
	{
		return m_result;
	}

private:
	enum class VisitState
	{
		NotVisited,
		BeingVisited,
		Visited
	};

	std::vector<NodePointer> m_result;
	std::map<NodePointer, VisitState> m_visitStates;

	void visitNode(NodePointer node) noexcept(false)
	{
		m_visitStates.insert({node, VisitState::BeingVisited});
		for (auto successor : node->template successors<SuccessorTypeTag>())
			sortSuccessors(successor);
		m_visitStates.at(node) = VisitState::Visited;
	}

	void sortSuccessors(NodePointer node) noexcept(false)
	{
		auto visitState = m_visitStates.find(node);
		if (visitState == m_visitStates.end())
		{
			visitNode(node);
			m_result.push_back(node);
		}
		else if ((*visitState).second == VisitState::BeingVisited)
			throw GraphCycleException();
	}

};

template<typename SuccessorTypeTag, typename GraphType, typename PayloadType = typename GraphType::PayloadType>
std::vector<typename GraphType::NodePointer> graph_sort_successors(const GraphType &graph, const PayloadType &payload) noexcept(false)
{
	auto successorSorter = GraphSuccessorSorter<GraphType, SuccessorTypeTag>{graph, payload};
	return successorSorter.result();
}
