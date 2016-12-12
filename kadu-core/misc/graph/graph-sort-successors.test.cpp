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

#include "misc/algorithm.h"
#include "misc/exception.h"
#include "misc/graph/graph-algorithm.h"
#include "misc/graph/graph.h"

#include <QtTest/QtTest>
#include <algorithm>

struct Tag1 {};
struct Tag2 {};

using TestGraph = Graph<std::string, Tag1, Tag2>;

class GraphSortSuccessorsTest : public QObject
{
	Q_OBJECT

private slots:
	void deterministicSortTest();
	void nondeterministicSortTest();
	void cycleSortTest();
	void invalidNodeTest();

};

void GraphSortSuccessorsTest::deterministicSortTest()
{
	auto graph = TestGraph{};

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p1", "p3");
	graph.addEdge<Tag1>("p1", "p4");
	graph.addEdge<Tag1>("p2", "p3");
	graph.addEdge<Tag1>("p2", "p4");
	graph.addEdge<Tag1>("p3", "p4");

	auto p2 = graph.node("p2");
	auto p3 = graph.node("p3");
	auto p4 = graph.node("p4");

	QCOMPARE(graph_sort_successors<Tag1>(graph, "p1"), (std::vector<TestGraph::NodePointer>{p4, p3, p2}));
	QCOMPARE(graph_sort_successors<Tag1>(graph, "p2"), (std::vector<TestGraph::NodePointer>{p4, p3}));
	QCOMPARE(graph_sort_successors<Tag1>(graph, "p3"), (std::vector<TestGraph::NodePointer>{p4}));
	QCOMPARE(graph_sort_successors<Tag1>(graph, "p4"), (std::vector<TestGraph::NodePointer>{}));
}

void GraphSortSuccessorsTest::nondeterministicSortTest()
{
	auto graph = TestGraph{};

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p2", "p3");
	graph.addEdge<Tag1>("p2", "p4");
	graph.addEdge<Tag1>("p3", "p5");
	graph.addEdge<Tag1>("p4", "p5");

	auto p2 = graph.node("p2");
	auto p3 = graph.node("p3");
	auto p4 = graph.node("p4");
	auto p5 = graph.node("p5");

	auto p1Successors = graph_sort_successors<Tag1>(graph, "p1");
	QCOMPARE(p1Successors.size(), size_t{4});
	QVERIFY(contains(p1Successors, p2));
	QVERIFY(contains(p1Successors, p3));
	QVERIFY(contains(p1Successors, p4));
	QVERIFY(contains(p1Successors, p5));
	QVERIFY(precedes(p1Successors, p5, p4));
	QVERIFY(precedes(p1Successors, p5, p3));
	QVERIFY(precedes(p1Successors, p5, p2));
	QVERIFY(precedes(p1Successors, p4, p2));
	QVERIFY(precedes(p1Successors, p3, p2));
}

void GraphSortSuccessorsTest::cycleSortTest()
{
	auto graph = TestGraph{};

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p2", "p3");
	graph.addEdge<Tag1>("p3", "p1");

	expect<GraphCycleException>([&]{ graph_sort_successors<Tag1>(graph, "p1"); });
}

void GraphSortSuccessorsTest::invalidNodeTest()
{
	auto graph = TestGraph{};

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p1", "p3");

	auto p4Successors = graph_sort_successors<Tag1>(graph, "p4");
	QCOMPARE(p4Successors.size(), size_t{0});
}

QTEST_APPLESS_MAIN(GraphSortSuccessorsTest)
#include "graph-sort-successors.test.moc"
