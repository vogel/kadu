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

#include "misc/misc-memory.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/dependency-graph/plugin-dependency-graph-cycle-finder.h"
#include "plugins/dependency-graph/plugin-dependency-graph-node.h"

#include <algorithm>
#include <QtTest/QtTest>

class tst_PluginDependencyGraphCycleFinder : public QObject
{
	Q_OBJECT

private slots:
	void noCyclesTest();
	void oneCycleTest();
	// void multipleCycleTest();

};

void tst_PluginDependencyGraphCycleFinder::noCyclesTest()
{
	auto p1 = make_unique<PluginDependencyGraphNode>("p1");
	auto p2 = make_unique<PluginDependencyGraphNode>("p2");
	auto p3 = make_unique<PluginDependencyGraphNode>("p3");
	auto p4 = make_unique<PluginDependencyGraphNode>("p4");

	p1.get()->addDependency(p2.get());
	p2.get()->addDependent(p1.get());
	p1.get()->addDependency(p3.get());
	p3.get()->addDependent(p1.get());
	p1.get()->addDependency(p4.get());
	p4.get()->addDependent(p1.get());
	p2.get()->addDependency(p3.get());
	p3.get()->addDependent(p2.get());
	p2.get()->addDependency(p4.get());
	p4.get()->addDependent(p2.get());
	p3.get()->addDependency(p4.get());
	p4.get()->addDependent(p3.get());

	auto nodes = std::vector<decltype(p1)>{};
	nodes.push_back(std::move(p1));
	nodes.push_back(std::move(p2));
	nodes.push_back(std::move(p3));
	nodes.push_back(std::move(p4));

	PluginDependencyGraph pluginDependencyGraph{std::move(nodes)};
	PluginDependencyGraphCycleFinder pluginDependencyGraphCycleFinder{};

	auto nodesInCycle = pluginDependencyGraphCycleFinder.findNodesInCycle(&pluginDependencyGraph);
	QCOMPARE(nodesInCycle.size(), 0UL);
}

void tst_PluginDependencyGraphCycleFinder::oneCycleTest()
{
	auto p1 = make_unique<PluginDependencyGraphNode>("p1");
	auto p2 = make_unique<PluginDependencyGraphNode>("p2");
	auto p3 = make_unique<PluginDependencyGraphNode>("p3");
	auto p4 = make_unique<PluginDependencyGraphNode>("p4");

	p1.get()->addDependency(p2.get());
	p2.get()->addDependent(p1.get());
	p1.get()->addDependency(p3.get());
	p3.get()->addDependent(p1.get());
	p1.get()->addDependency(p4.get());
	p4.get()->addDependent(p1.get());
	p2.get()->addDependency(p3.get());
	p3.get()->addDependent(p2.get());
	p2.get()->addDependency(p4.get());
	p4.get()->addDependent(p2.get());
	p3.get()->addDependency(p4.get());
	p4.get()->addDependent(p3.get());
	p4.get()->addDependency(p2.get());
	p2.get()->addDependent(p4.get());

	auto nodes = std::vector<decltype(p1)>{};
	nodes.push_back(std::move(p1));
	nodes.push_back(std::move(p2));
	nodes.push_back(std::move(p3));
	nodes.push_back(std::move(p4));

	PluginDependencyGraph pluginDependencyGraph{std::move(nodes)};
	PluginDependencyGraphCycleFinder pluginDependencyGraphCycleFinder{};

	auto nodesInCycle = pluginDependencyGraphCycleFinder.findNodesInCycle(&pluginDependencyGraph);
	QCOMPARE(nodesInCycle.size(), 3UL);
	QCOMPARE(nodesInCycle.count(pluginDependencyGraph.node("p2")), 1UL);
	QCOMPARE(nodesInCycle.count(pluginDependencyGraph.node("p3")), 1UL);
	QCOMPARE(nodesInCycle.count(pluginDependencyGraph.node("p4")), 1UL);
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraphCycleFinder)
#include "tst-plugin-dependency-graph-cycle-finder.moc"
