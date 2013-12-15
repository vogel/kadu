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
#include "misc/graph/graph.h"
#include "misc/graph/graph-algorithm.h"

#include <algorithm>
#include <QtTest/QtTest>

struct Tag1 {};
struct Tag2 {};

using TestGraph = Graph<std::string, Tag1, Tag2>;


class tst_GraphSortSuccessors : public QObject
{
	Q_OBJECT

private slots:
	void deterministicSortTest();
	void nondeterministicSortTest();
	void cycleSortTest();

};

void tst_GraphSortSuccessors::deterministicSortTest()
{
	auto graph = TestGraph{};

	graph.addNode("p1");
	graph.addNode("p2");
	graph.addNode("p3");
	graph.addNode("p4");

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p1", "p3");
	graph.addEdge<Tag1>("p1", "p4");
	graph.addEdge<Tag1>("p2", "p3");
	graph.addEdge<Tag1>("p2", "p4");
	graph.addEdge<Tag1>("p3", "p4");

	auto p1Successors = graph_sort_successors<Tag1>(graph, "p1");
	QCOMPARE(p1Successors.size(), 3UL);
	QCOMPARE(p1Successors.at(0)->payload(), std::string("p4"));
	QCOMPARE(p1Successors.at(1)->payload(), std::string("p3"));
	QCOMPARE(p1Successors.at(2)->payload(), std::string("p2"));

	auto p2Successors = graph_sort_successors<Tag1>(graph, "p2");
	QCOMPARE(p2Successors.size(), 2UL);
	QCOMPARE(p2Successors.at(0)->payload(), std::string("p4"));
	QCOMPARE(p2Successors.at(1)->payload(), std::string("p3"));

	auto p3Successors = graph_sort_successors<Tag1>(graph, "p3");
	QCOMPARE(p3Successors.size(), 1UL);
	QCOMPARE(p3Successors.at(0)->payload(), std::string("p4"));

	auto p4Successors = graph_sort_successors<Tag1>(graph, "p4");
	QCOMPARE(p4Successors.size(), 0UL);
}

void tst_GraphSortSuccessors::nondeterministicSortTest()
{
	auto graph = TestGraph{};

	graph.addNode("p1");
	graph.addNode("p2");
	graph.addNode("p3");
	graph.addNode("p4");

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p1", "p3");
	graph.addEdge<Tag1>("p2", "p4");
	graph.addEdge<Tag1>("p3", "p4");

	auto p1Successors = graph_sort_successors<Tag1>(graph, "p1");
	QCOMPARE(p1Successors.size(), 3UL);
	QCOMPARE(p1Successors.at(0)->payload(), std::string("p4"));

	bool sortVariant1 = p1Successors.at(1)->payload() == "p3" && p1Successors.at(2)->payload() == "p2";
	bool sortVariant2 = p1Successors.at(1)->payload() == "p2" && p1Successors.at(2)->payload() == "p3";
	QVERIFY(sortVariant1 || sortVariant2);
}

void tst_GraphSortSuccessors::cycleSortTest()
{
	auto graph = TestGraph{};

	graph.addNode("p1");
	graph.addNode("p2");
	graph.addNode("p3");

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p2", "p3");
	graph.addEdge<Tag1>("p3", "p1");

	try
	{
		auto p1Successors = graph_sort_successors<Tag1>(graph, "p1");
		QFAIL("Exception not thrown");
	}
	catch (GraphCycleException &e)
	{
		// OK
	}
	catch (...)
	{
		QFAIL("Unexpected exception thrown");
	}
}

QTEST_APPLESS_MAIN(tst_GraphSortSuccessors)
#include "tst-graph-sort-successors.moc"
