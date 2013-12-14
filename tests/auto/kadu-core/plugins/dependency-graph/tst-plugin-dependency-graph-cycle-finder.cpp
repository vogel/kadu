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
	void addDependency(std::unique_ptr<PluginDependencyGraphNode> &dependent,
					   std::unique_ptr<PluginDependencyGraphNode> &dependency);

private slots:
	void noCyclesTest();
	void oneCycleTest();
	void multipleCycleTest();

};

void tst_PluginDependencyGraphCycleFinder::addDependency(std::unique_ptr<PluginDependencyGraphNode> &dependent,
														 std::unique_ptr<PluginDependencyGraphNode> &dependency)
{
	dependent.get()->addSuccessor<PluginDependencyTag>(dependency.get());
	dependency.get()->addSuccessor<PluginDependentTag>(dependent.get());
}

void tst_PluginDependencyGraphCycleFinder::noCyclesTest()
{
	auto p1 = make_unique<PluginDependencyGraphNode>("p1");
	auto p2 = make_unique<PluginDependencyGraphNode>("p2");
	auto p3 = make_unique<PluginDependencyGraphNode>("p3");
	auto p4 = make_unique<PluginDependencyGraphNode>("p4");

	addDependency(p1, p2);
	addDependency(p1, p3);
	addDependency(p1, p4);
	addDependency(p2, p3);
	addDependency(p2, p4);
	addDependency(p3, p4);

	auto nodes = std::vector<decltype(p1)>{};
	nodes.emplace_back(std::move(p1));
	nodes.emplace_back(std::move(p2));
	nodes.emplace_back(std::move(p3));
	nodes.emplace_back(std::move(p4));

	PluginDependencyGraph pluginDependencyGraph{std::move(nodes)};
	PluginDependencyGraphCycleFinder pluginDependencyGraphCycleFinder{};

	auto nodesInCycle = pluginDependencyGraphCycleFinder.findNodesInCycle(&pluginDependencyGraph);
	QVERIFY(nodesInCycle.empty());
}

void tst_PluginDependencyGraphCycleFinder::oneCycleTest()
{
	auto p1 = make_unique<PluginDependencyGraphNode>("p1");
	auto p2 = make_unique<PluginDependencyGraphNode>("p2");
	auto p3 = make_unique<PluginDependencyGraphNode>("p3");
	auto p4 = make_unique<PluginDependencyGraphNode>("p4");

	addDependency(p1, p2);
	addDependency(p1, p3);
	addDependency(p1, p4);
	addDependency(p2, p3);
	addDependency(p2, p4);
	addDependency(p3, p4);
	addDependency(p4, p2);

	auto nodes = std::vector<decltype(p1)>{};
	nodes.emplace_back(std::move(p1));
	nodes.emplace_back(std::move(p2));
	nodes.emplace_back(std::move(p3));
	nodes.emplace_back(std::move(p4));

	PluginDependencyGraph pluginDependencyGraph{std::move(nodes)};
	PluginDependencyGraphCycleFinder pluginDependencyGraphCycleFinder{};

	auto nodesInCycle = pluginDependencyGraphCycleFinder.findNodesInCycle(&pluginDependencyGraph);
	QCOMPARE(nodesInCycle.size(), 3);
	QVERIFY(nodesInCycle.contains("p2"));
	QVERIFY(nodesInCycle.contains("p3"));
	QVERIFY(nodesInCycle.contains("p4"));
}

void tst_PluginDependencyGraphCycleFinder::multipleCycleTest()
{
	auto p1 = make_unique<PluginDependencyGraphNode>("p1");
	auto p2 = make_unique<PluginDependencyGraphNode>("p2");
	auto p3 = make_unique<PluginDependencyGraphNode>("p3");
	auto p4 = make_unique<PluginDependencyGraphNode>("p4");
	auto p5 = make_unique<PluginDependencyGraphNode>("p5");
	auto p6 = make_unique<PluginDependencyGraphNode>("p6");
	auto p7 = make_unique<PluginDependencyGraphNode>("p7");
	auto p8 = make_unique<PluginDependencyGraphNode>("p8");
	auto p9 = make_unique<PluginDependencyGraphNode>("p9");
	auto p10 = make_unique<PluginDependencyGraphNode>("p10");

	addDependency(p1, p2);
	addDependency(p2, p1);

	addDependency(p2, p3);
	addDependency(p3, p4);

	addDependency(p4, p5);
	addDependency(p5, p6);
	addDependency(p6, p7);
	addDependency(p7, p4);

	addDependency(p7, p8);

	addDependency(p4, p9);
	addDependency(p9, p10);
	addDependency(p10, p4);

	auto nodes = std::vector<decltype(p1)>{};
	nodes.emplace_back(std::move(p1));
	nodes.emplace_back(std::move(p2));
	nodes.emplace_back(std::move(p3));
	nodes.emplace_back(std::move(p4));
	nodes.emplace_back(std::move(p5));
	nodes.emplace_back(std::move(p6));
	nodes.emplace_back(std::move(p7));
	nodes.emplace_back(std::move(p8));
	nodes.emplace_back(std::move(p9));
	nodes.emplace_back(std::move(p10));

	PluginDependencyGraph pluginDependencyGraph{std::move(nodes)};
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
