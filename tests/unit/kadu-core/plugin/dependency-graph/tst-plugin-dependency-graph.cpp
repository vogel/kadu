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
#include "plugin/dependency-graph/plugin-dependency-cycle-exception.h"
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "utils/exception.h"

#include <algorithm>
#include <QtTest/QtTest>

class tst_PluginDependencyGraph : public QObject
{
	Q_OBJECT

private:
	PluginDependencyGraph linearGraph();
	PluginDependencyGraph treeGraph();
	PluginDependencyGraph cycleGraph();

private slots:
	void invalidDirectDependencyTest();
	void invalidDependencyTest();
	void linearDependencyCycleTest();
	void linearDirectDependencyTest();
	void linearDependencyTest();
	void treeCycleTest();
	void treeDirectDependencyTest();
	void treeDependencyTest();
	void cycleCycleTest();
	void cycleDirectDependencyTest();
	void cycleDependencyTest();
	void pluginsTest();

};

PluginDependencyGraph tst_PluginDependencyGraph::linearGraph()
{
	auto graph = PluginDependencyGraph{};

	graph.addPlugin("p1");
	graph.addPlugin("p2");
	graph.addPlugin("p3");
	graph.addPlugin("p4");

	graph.addDependency("p1", "p2");
	graph.addDependency("p2", "p3");
	graph.addDependency("p3", "p4");

	return std::move(graph);
}

PluginDependencyGraph tst_PluginDependencyGraph::treeGraph()
{
	auto graph = PluginDependencyGraph{};

	graph.addPlugin("p1");
	graph.addPlugin("p2");
	graph.addPlugin("p3");
	graph.addPlugin("p4");
	graph.addPlugin("p5");
	graph.addPlugin("p6");

	graph.addDependency("p1", "p2");
	graph.addDependency("p2", "p3");
	graph.addDependency("p2", "p4");
	graph.addDependency("p3", "p5");
	graph.addDependency("p4", "p5");
	graph.addDependency("p5", "p6");

	return std::move(graph);
}

PluginDependencyGraph tst_PluginDependencyGraph::cycleGraph()
{
	auto graph = PluginDependencyGraph{};

	graph.addPlugin("p1");
	graph.addPlugin("p2");
	graph.addPlugin("p3");
	graph.addPlugin("p4");
	graph.addPlugin("p5");
	graph.addPlugin("p6");

	graph.addDependency("p1", "p2");
	graph.addDependency("p2", "p3");
	graph.addDependency("p2", "p4");
	graph.addDependency("p3", "p5");
	graph.addDependency("p4", "p5");
	graph.addDependency("p5", "p2");
	graph.addDependency("p5", "p6");

	return std::move(graph);
}

void tst_PluginDependencyGraph::invalidDirectDependencyTest()
{
	auto graph = linearGraph();

	QCOMPARE(graph.directDependencies("p8"), {});
	QCOMPARE(graph.directDependents("p8"), {});
}

void tst_PluginDependencyGraph::invalidDependencyTest()
{
	auto graph = linearGraph();

	QCOMPARE(graph.findDependencies("p8"), {});
	QCOMPARE(graph.findDependents("p8"), {});
}

void tst_PluginDependencyGraph::linearDependencyCycleTest()
{
	QVERIFY(linearGraph().findPluginsInDependencyCycle().isEmpty());
}

void tst_PluginDependencyGraph::linearDirectDependencyTest()
{
	auto graph = linearGraph();

	QCOMPARE(graph.directDependencies("p1"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependents("p1"), QSet<QString>{});
	QCOMPARE(graph.directDependencies("p2"), QSet<QString>{} << "p3");
	QCOMPARE(graph.directDependents("p2"), QSet<QString>{} << "p1");
	QCOMPARE(graph.directDependencies("p3"), QSet<QString>{} << "p4");
	QCOMPARE(graph.directDependents("p3"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependencies("p4"), QSet<QString>{});
	QCOMPARE(graph.directDependents("p4"), QSet<QString>{} << "p3");
}

void tst_PluginDependencyGraph::linearDependencyTest()
{
	auto graph = linearGraph();

	QCOMPARE(graph.findDependencies("p1"), QVector<QString>{} << "p4" << "p3" << "p2");
	QCOMPARE(graph.findDependents("p1"), QVector<QString>{});
	QCOMPARE(graph.findDependencies("p2"), QVector<QString>{} << "p4" << "p3");
	QCOMPARE(graph.findDependents("p2"), QVector<QString>{} << "p1");
	QCOMPARE(graph.findDependencies("p3"), QVector<QString>{} << "p4");
	QCOMPARE(graph.findDependents("p3"), QVector<QString>{} << "p1" << "p2");
	QCOMPARE(graph.findDependencies("p4"), QVector<QString>{});
	QCOMPARE(graph.findDependents("p4"), QVector<QString>{} << "p1" << "p2" << "p3");
}

void tst_PluginDependencyGraph::treeCycleTest()
{
	QVERIFY(treeGraph().findPluginsInDependencyCycle().isEmpty());
}

void tst_PluginDependencyGraph::treeDirectDependencyTest()
{
	auto graph = treeGraph();

	QCOMPARE(graph.directDependencies("p1"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependents("p1"), QSet<QString>{});
	QCOMPARE(graph.directDependencies("p2"), QSet<QString>{} << "p3" << "p4");
	QCOMPARE(graph.directDependents("p2"), QSet<QString>{} << "p1");
	QCOMPARE(graph.directDependencies("p3"), QSet<QString>{} << "p5");
	QCOMPARE(graph.directDependents("p3"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependencies("p4"), QSet<QString>{} << "p5");
	QCOMPARE(graph.directDependents("p4"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependencies("p5"), QSet<QString>{} << "p6");
	QCOMPARE(graph.directDependents("p5"), QSet<QString>{} << "p3" << "p4");
	QCOMPARE(graph.directDependencies("p6"), QSet<QString>{});
	QCOMPARE(graph.directDependents("p6"), QSet<QString>{} << "p5");
}

void tst_PluginDependencyGraph::treeDependencyTest()
{
	auto graph = treeGraph();

	auto p1Dependencies = graph.findDependencies("p1");
	auto p1Dependents = graph.findDependents("p1");

	auto p6Dependencies = graph.findDependencies("p6");
	auto p6Dependents = graph.findDependents("p6");

	QCOMPARE(p1Dependencies.size(), 5);
	QVERIFY(!contains(p1Dependencies, "p1"));
	QVERIFY(contains(p1Dependencies, "p2"));
	QVERIFY(contains(p1Dependencies, "p3"));
	QVERIFY(contains(p1Dependencies, "p4"));
	QVERIFY(contains(p1Dependencies, "p5"));
	QVERIFY(contains(p1Dependencies, "p6"));
	QVERIFY(precedes(p1Dependencies, "p6", "p5"));
	QVERIFY(precedes(p1Dependencies, "p5", "p4"));
	QVERIFY(precedes(p1Dependencies, "p5", "p3"));
	QVERIFY(precedes(p1Dependencies, "p4", "p2"));
	QVERIFY(precedes(p1Dependencies, "p3", "p2"));

	QCOMPARE(p1Dependents.size(), 0);
	QCOMPARE(p6Dependencies.size(), 0);

	QCOMPARE(p6Dependents.size(), 5);
	QVERIFY(contains(p6Dependents, "p1"));
	QVERIFY(contains(p6Dependents, "p2"));
	QVERIFY(contains(p6Dependents, "p3"));
	QVERIFY(contains(p6Dependents, "p4"));
	QVERIFY(contains(p6Dependents, "p5"));
	QVERIFY(!contains(p6Dependents, "p6"));
	QVERIFY(precedes(p6Dependents, "p1", "p2"));
	QVERIFY(precedes(p6Dependents, "p2", "p3"));
	QVERIFY(precedes(p6Dependents, "p2", "p4"));
	QVERIFY(precedes(p6Dependents, "p3", "p5"));
	QVERIFY(precedes(p6Dependents, "p4", "p5"));
}

void tst_PluginDependencyGraph::cycleCycleTest()
{
	QCOMPARE(cycleGraph().findPluginsInDependencyCycle(), QSet<QString>{} << "p2" << "p3" << "p4" << "p5");
}

void tst_PluginDependencyGraph::cycleDirectDependencyTest()
{
	auto graph = cycleGraph();

	QCOMPARE(graph.directDependencies("p1"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependents("p1"), QSet<QString>{});
	QCOMPARE(graph.directDependencies("p2"), QSet<QString>{} << "p3" << "p4");
	QCOMPARE(graph.directDependents("p2"), QSet<QString>{} << "p1" << "p5");
	QCOMPARE(graph.directDependencies("p3"), QSet<QString>{} << "p5");
	QCOMPARE(graph.directDependents("p3"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependencies("p4"), QSet<QString>{} << "p5");
	QCOMPARE(graph.directDependents("p4"), QSet<QString>{} << "p2");
	QCOMPARE(graph.directDependencies("p5"), QSet<QString>{} << "p6" << "p2");
	QCOMPARE(graph.directDependents("p5"), QSet<QString>{} << "p3" << "p4");
	QCOMPARE(graph.directDependencies("p6"), QSet<QString>{});
	QCOMPARE(graph.directDependents("p6"), QSet<QString>{} << "p5");
}

void tst_PluginDependencyGraph::cycleDependencyTest()
{
	auto graph = cycleGraph();

	graph.addDependency("p1", "p2");
	graph.addDependency("p2", "p3");
	graph.addDependency("p2", "p4");
	graph.addDependency("p3", "p5");
	graph.addDependency("p4", "p5");
	graph.addDependency("p5", "p2");
	graph.addDependency("p5", "p6");

	expect<PluginDependencyCycleException>([&]{ graph.findDependencies("p1"); });
	QCOMPARE(graph.findDependents("p1"), QVector<QString>{});
	expect<PluginDependencyCycleException>([&]{ graph.findDependencies("p2"); });
	expect<PluginDependencyCycleException>([&]{ graph.findDependents("p2"); });
	expect<PluginDependencyCycleException>([&]{ graph.findDependencies("p3"); });
	expect<PluginDependencyCycleException>([&]{ graph.findDependents("p3"); });
	expect<PluginDependencyCycleException>([&]{ graph.findDependencies("p4"); });
	expect<PluginDependencyCycleException>([&]{ graph.findDependents("p4"); });
	expect<PluginDependencyCycleException>([&]{ graph.findDependencies("p5"); });
	expect<PluginDependencyCycleException>([&]{ graph.findDependents("p5"); });
	QCOMPARE(graph.findDependencies("p6"), QVector<QString>{});
	expect<PluginDependencyCycleException>([&]{ graph.findDependents("p6"); });
}

void tst_PluginDependencyGraph::pluginsTest()
{
	auto graph = PluginDependencyGraph{};

	QCOMPARE(graph.size(), 0);
	QCOMPARE(graph.plugins(), {});

	graph.addPlugin("p1");

	QCOMPARE(graph.size(), 1);
	QCOMPARE(graph.plugins(), {"p1"});

	graph.addPlugin("p1");

	QCOMPARE(graph.size(), 1);
	QCOMPARE(graph.plugins(), {"p1"});

	graph.addPlugin("p2");

	QCOMPARE(graph.size(), 2);
	QCOMPARE(graph.plugins(), (std::set<QString>{"p1", "p2"}));

	graph.addDependency("p1", "p2");

	QCOMPARE(graph.size(), 2);
	QCOMPARE(graph.plugins(), (std::set<QString>{"p1", "p2"}));

	graph.addDependency("p3", "p4");

	QCOMPARE(graph.size(), 4);
	QCOMPARE(graph.plugins(), (std::set<QString>{"p1", "p2", "p3", "p4"}));

	graph.addDependency("p1", "p5");

	QCOMPARE(graph.size(), 5);
	QCOMPARE(graph.plugins(), (std::set<QString>{"p1", "p2", "p3", "p4", "p5"}));
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraph)
#include "tst-plugin-dependency-graph.moc"
