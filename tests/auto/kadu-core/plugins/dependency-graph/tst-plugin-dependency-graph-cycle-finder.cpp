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

#include "misc/memory.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/dependency-graph/plugin-dependency-graph-cycle-finder.h"

#include <algorithm>
#include <QtTest/QtTest>

class tst_PluginDependencyGraphCycleFinder : public QObject
{
	Q_OBJECT

private:
	void addDependency(PluginDependencyGraph &graph, const QString &dependent, const QString &dependency);

private slots:
	void noCyclesTest();
	void oneCycleTest();
	void multipleCycleTest();

};

void tst_PluginDependencyGraphCycleFinder::addDependency(PluginDependencyGraph &graph, const QString &dependent, const QString &dependency)
{
	graph.addEdge<PluginDependencyTag>(dependent, dependency);
	graph.addEdge<PluginDependentTag>(dependency, dependent);
}

void tst_PluginDependencyGraphCycleFinder::noCyclesTest()
{
	auto pluginDependencyGraph = PluginDependencyGraph{};

	pluginDependencyGraph.addNode("p1");
	pluginDependencyGraph.addNode("p2");
	pluginDependencyGraph.addNode("p3");
	pluginDependencyGraph.addNode("p4");

	addDependency(pluginDependencyGraph, "p1", "p2");
	addDependency(pluginDependencyGraph, "p1", "p3");
	addDependency(pluginDependencyGraph, "p1", "p4");
	addDependency(pluginDependencyGraph, "p2", "p3");
	addDependency(pluginDependencyGraph, "p2", "p4");
	addDependency(pluginDependencyGraph, "p3", "p4");

	PluginDependencyGraphCycleFinder pluginDependencyGraphCycleFinder{};

	auto nodesInCycle = pluginDependencyGraphCycleFinder.findNodesInCycle(&pluginDependencyGraph);
	QVERIFY(nodesInCycle.empty());
}

void tst_PluginDependencyGraphCycleFinder::oneCycleTest()
{
	auto pluginDependencyGraph = PluginDependencyGraph{};

	pluginDependencyGraph.addNode("p1");
	pluginDependencyGraph.addNode("p2");
	pluginDependencyGraph.addNode("p3");
	pluginDependencyGraph.addNode("p4");

	addDependency(pluginDependencyGraph, "p1", "p2");
	addDependency(pluginDependencyGraph, "p1", "p3");
	addDependency(pluginDependencyGraph, "p1", "p4");
	addDependency(pluginDependencyGraph, "p2", "p3");
	addDependency(pluginDependencyGraph, "p2", "p4");
	addDependency(pluginDependencyGraph, "p3", "p4");
	addDependency(pluginDependencyGraph, "p4", "p2");

	PluginDependencyGraphCycleFinder pluginDependencyGraphCycleFinder{};

	auto nodesInCycle = pluginDependencyGraphCycleFinder.findNodesInCycle(&pluginDependencyGraph);
	QCOMPARE(nodesInCycle.size(), 3);
	QVERIFY(nodesInCycle.contains("p2"));
	QVERIFY(nodesInCycle.contains("p3"));
	QVERIFY(nodesInCycle.contains("p4"));
}

void tst_PluginDependencyGraphCycleFinder::multipleCycleTest()
{
	auto pluginDependencyGraph = PluginDependencyGraph{};

	pluginDependencyGraph.addNode("p1");
	pluginDependencyGraph.addNode("p2");
	pluginDependencyGraph.addNode("p3");
	pluginDependencyGraph.addNode("p4");
	pluginDependencyGraph.addNode("p5");
	pluginDependencyGraph.addNode("p6");
	pluginDependencyGraph.addNode("p7");
	pluginDependencyGraph.addNode("p8");
	pluginDependencyGraph.addNode("p9");
	pluginDependencyGraph.addNode("p10");

	addDependency(pluginDependencyGraph, "p1", "p2");
	addDependency(pluginDependencyGraph, "p2", "p1");

	addDependency(pluginDependencyGraph, "p2", "p3");
	addDependency(pluginDependencyGraph, "p3", "p4");

	addDependency(pluginDependencyGraph, "p4", "p5");
	addDependency(pluginDependencyGraph, "p5", "p6");
	addDependency(pluginDependencyGraph, "p6", "p7");
	addDependency(pluginDependencyGraph, "p7", "p4");

	addDependency(pluginDependencyGraph, "p7", "p8");

	addDependency(pluginDependencyGraph, "p4", "p9");
	addDependency(pluginDependencyGraph, "p9", "p10");
	addDependency(pluginDependencyGraph, "p10", "p4");

	PluginDependencyGraphCycleFinder pluginDependencyGraphCycleFinder{};

	auto nodesInCycle = pluginDependencyGraphCycleFinder.findNodesInCycle(&pluginDependencyGraph);
	QCOMPARE(nodesInCycle.size(), 8);
	QVERIFY(nodesInCycle.contains("p1"));
	QVERIFY(nodesInCycle.contains("p2"));
	QVERIFY(nodesInCycle.contains("p4"));
	QVERIFY(nodesInCycle.contains("p5"));
	QVERIFY(nodesInCycle.contains("p6"));
	QVERIFY(nodesInCycle.contains("p7"));
	QVERIFY(nodesInCycle.contains("p9"));
	QVERIFY(nodesInCycle.contains("p10"));
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraphCycleFinder)
#include "tst-plugin-dependency-graph-cycle-finder.moc"
