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


class tst_GraphFindCycles : public QObject
{
	Q_OBJECT

private slots:
	void noCyclesTest();
	void oneCycleTest();
	void multipleCycleTest();
	void multipleCycleMultipleTagsTest();
	void bigCycleTest();

};

void tst_GraphFindCycles::noCyclesTest()
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

	auto nodesInCycle = graph_find_cycles<Tag1>(graph);
	QVERIFY(nodesInCycle.empty());
}

void tst_GraphFindCycles::oneCycleTest()
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
	graph.addEdge<Tag1>("p4", "p2");

	auto nodesInCycle = graph_find_cycles<Tag1>(graph);
	QCOMPARE(nodesInCycle.size(), 3UL);
	QVERIFY(contains(nodesInCycle, "p2"));
	QVERIFY(contains(nodesInCycle, "p3"));
	QVERIFY(contains(nodesInCycle, "p4"));
}

void tst_GraphFindCycles::multipleCycleTest()
{
	auto graph = TestGraph{};

	graph.addNode("p1");
	graph.addNode("p2");
	graph.addNode("p3");
	graph.addNode("p4");
	graph.addNode("p5");
	graph.addNode("p6");
	graph.addNode("p7");
	graph.addNode("p8");
	graph.addNode("p9");
	graph.addNode("p10");

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p2", "p1");

	graph.addEdge<Tag1>("p2", "p3");
	graph.addEdge<Tag1>("p3", "p4");

	graph.addEdge<Tag1>("p4", "p5");
	graph.addEdge<Tag1>("p5", "p6");
	graph.addEdge<Tag1>("p6", "p7");
	graph.addEdge<Tag1>("p7", "p4");

	graph.addEdge<Tag1>("p7", "p8");

	graph.addEdge<Tag1>("p4", "p9");
	graph.addEdge<Tag1>("p9", "p10");
	graph.addEdge<Tag1>("p10", "p4");

	auto nodesInCycle = graph_find_cycles<Tag1>(graph);
	QCOMPARE(nodesInCycle.size(), 8UL);
	QVERIFY(contains(nodesInCycle, "p1"));
	QVERIFY(contains(nodesInCycle, "p2"));
	QVERIFY(contains(nodesInCycle, "p4"));
	QVERIFY(contains(nodesInCycle, "p5"));
	QVERIFY(contains(nodesInCycle, "p6"));
	QVERIFY(contains(nodesInCycle, "p7"));
	QVERIFY(contains(nodesInCycle, "p9"));
	QVERIFY(contains(nodesInCycle, "p10"));
}

void tst_GraphFindCycles::multipleCycleMultipleTagsTest()
{
	auto graph = TestGraph{};

	graph.addNode("p1");
	graph.addNode("p2");
	graph.addNode("p3");
	graph.addNode("p4");
	graph.addNode("p5");
	graph.addNode("p6");
	graph.addNode("p7");
	graph.addNode("p8");
	graph.addNode("p9");
	graph.addNode("p10");

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p2", "p1");

	graph.addEdge<Tag1>("p3", "p4");
	graph.addEdge<Tag1>("p4", "p5");
	graph.addEdge<Tag1>("p5", "p3");

	graph.addEdge<Tag2>("p1", "p3");
	graph.addEdge<Tag2>("p3", "p5");
	graph.addEdge<Tag2>("p5", "p7");
	graph.addEdge<Tag2>("p7", "p1");

	graph.addEdge<Tag2>("p4", "p2");
	graph.addEdge<Tag2>("p2", "p4");

	auto nodesInCycleTag1 = graph_find_cycles<Tag1>(graph);
	QCOMPARE(nodesInCycleTag1.size(), 5UL);
	QVERIFY(contains(nodesInCycleTag1, "p1"));
	QVERIFY(contains(nodesInCycleTag1, "p2"));
	QVERIFY(contains(nodesInCycleTag1, "p3"));
	QVERIFY(contains(nodesInCycleTag1, "p4"));
	QVERIFY(contains(nodesInCycleTag1, "p5"));

	auto nodesInCycleTag2 = graph_find_cycles<Tag2>(graph);
	QCOMPARE(nodesInCycleTag2.size(), 6UL);
	QVERIFY(contains(nodesInCycleTag2, "p1"));
	QVERIFY(contains(nodesInCycleTag2, "p2"));
	QVERIFY(contains(nodesInCycleTag2, "p3"));
	QVERIFY(contains(nodesInCycleTag2, "p4"));
	QVERIFY(contains(nodesInCycleTag2, "p5"));
	QVERIFY(contains(nodesInCycleTag2, "p7"));
}

void tst_GraphFindCycles::bigCycleTest()
{
	auto graph = TestGraph{};

	graph.addNode("p1");
	graph.addNode("p2");
	graph.addNode("p3");
	graph.addNode("p4");
	graph.addNode("p5");
	graph.addNode("p6");

	graph.addEdge<Tag1>("p1", "p2");
	graph.addEdge<Tag1>("p2", "p3");
	graph.addEdge<Tag1>("p2", "p4");
	graph.addEdge<Tag1>("p3", "p5");
	graph.addEdge<Tag1>("p4", "p5");
	graph.addEdge<Tag1>("p5", "p2");
	graph.addEdge<Tag1>("p5", "p6");

	auto nodesInCycle = graph_find_cycles<Tag1>(graph);
	QCOMPARE(nodesInCycle.size(), 4UL);
	QVERIFY(contains(nodesInCycle, "p2"));
	QVERIFY(contains(nodesInCycle, "p3"));
	QVERIFY(contains(nodesInCycle, "p4"));
	QVERIFY(contains(nodesInCycle, "p5"));
}

QTEST_APPLESS_MAIN(tst_GraphFindCycles)
#include "tst-graph-find-cycles.moc"
