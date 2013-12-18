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
#include "plugins/dependency-graph/plugin-dependency-cycle-exception.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"

#include <algorithm>
#include <QtTest/QtTest>

class tst_PluginDependencyGraph : public QObject
{
	Q_OBJECT

private:
	PluginDependencyGraph linearGraph();
	PluginDependencyGraph treeGraph();
	PluginDependencyGraph cycleGraph();

	void expectCycle(std::function<void()> graphCall);

private slots:
	void linearDependencyCycleTest();
	void linearDirectDependencyTest();
	void linearDependencyTest();
	void treeCycleTest();
	void treeDirectDependencyTest();
	void treeDependencyTest();
	void cycleCycleTest();
	void cycleDirectDependencyTest();
	void cycleDependencyTest();

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

void tst_PluginDependencyGraph::expectCycle(std::function<void()> graphCall)
{

	try
	{
		graphCall();
		QFAIL("Exception not thrown");
	}
	catch (PluginDependencyCycleException &e)
	{
		// OK
	}
	catch (...)
	{
		QFAIL("Unexpected exception thrown");
	}
}

void tst_PluginDependencyGraph::linearDependencyCycleTest()
{
	QVERIFY(linearGraph().findPluginsInDependencyCycle().isEmpty());
}

void tst_PluginDependencyGraph::linearDirectDependencyTest()
{
	auto graph = linearGraph();

	auto p1DirectDependencies = graph.directDependencies("p1");
	auto p1DirectDependents = graph.directDependents("p1");
	auto p2DirectDependencies = graph.directDependencies("p2");
	auto p2DirectDependents = graph.directDependents("p2");
	auto p3DirectDependencies = graph.directDependencies("p3");
	auto p3DirectDependents = graph.directDependents("p3");
	auto p4DirectDependencies = graph.directDependencies("p4");
	auto p4DirectDependents = graph.directDependents("p4");

	QCOMPARE(p1DirectDependencies, QSet<QString>{} << "p2");
	QCOMPARE(p1DirectDependents, QSet<QString>{});

	QCOMPARE(p2DirectDependencies, QSet<QString>{} << "p3");
	QCOMPARE(p2DirectDependents, QSet<QString>{} << "p1");

	QCOMPARE(p3DirectDependencies, QSet<QString>{} << "p4");
	QCOMPARE(p3DirectDependents, QSet<QString>{} << "p2");

	QCOMPARE(p4DirectDependencies, QSet<QString>{});
	QCOMPARE(p4DirectDependents, QSet<QString>{} << "p3");
}

void tst_PluginDependencyGraph::linearDependencyTest()
{
	auto graph = linearGraph();

	auto p1Dependencies = graph.findDependencies("p1");
	auto p1Dependents = graph.findDependents("p1");
	auto p2Dependencies = graph.findDependencies("p2");
	auto p2Dependents = graph.findDependents("p2");
	auto p3Dependencies = graph.findDependencies("p3");
	auto p3Dependents = graph.findDependents("p3");
	auto p4Dependencies = graph.findDependencies("p4");
	auto p4Dependents = graph.findDependents("p4");

	QCOMPARE(p1Dependencies, QVector<QString>{} << "p4" << "p3" << "p2");
	QCOMPARE(p1Dependents, QVector<QString>{});

	QCOMPARE(p2Dependencies, QVector<QString>{} << "p4" << "p3");
	QCOMPARE(p2Dependents, QVector<QString>{} << "p1");

	QCOMPARE(p3Dependencies, QVector<QString>{} << "p4");
	QCOMPARE(p3Dependents, QVector<QString>{} << "p1" << "p2");

	QCOMPARE(p4Dependencies, QVector<QString>{});
	QCOMPARE(p4Dependents, QVector<QString>{} << "p1" << "p2" << "p3");
}

void tst_PluginDependencyGraph::treeCycleTest()
{
	QVERIFY(treeGraph().findPluginsInDependencyCycle().isEmpty());
}

void tst_PluginDependencyGraph::treeDirectDependencyTest()
{
	auto graph = treeGraph();

	auto p1DirectDependencies = graph.directDependencies("p1");
	auto p1DirectDependents = graph.directDependents("p1");
	auto p2DirectDependencies = graph.directDependencies("p2");
	auto p2DirectDependents = graph.directDependents("p2");
	auto p3DirectDependencies = graph.directDependencies("p3");
	auto p3DirectDependents = graph.directDependents("p3");
	auto p4DirectDependencies = graph.directDependencies("p4");
	auto p4DirectDependents = graph.directDependents("p4");
	auto p5DirectDependencies = graph.directDependencies("p5");
	auto p5DirectDependents = graph.directDependents("p5");
	auto p6DirectDependencies = graph.directDependencies("p6");
	auto p6DirectDependents = graph.directDependents("p6");

	QCOMPARE(p1DirectDependencies, QSet<QString>{} << "p2");
	QCOMPARE(p1DirectDependents, QSet<QString>{});

	QCOMPARE(p2DirectDependencies, QSet<QString>{} << "p3" << "p4");
	QCOMPARE(p2DirectDependents, QSet<QString>{} << "p1");

	QCOMPARE(p3DirectDependencies, QSet<QString>{} << "p5");
	QCOMPARE(p3DirectDependents, QSet<QString>{} << "p2");

	QCOMPARE(p4DirectDependencies, QSet<QString>{} << "p5");
	QCOMPARE(p4DirectDependents, QSet<QString>{} << "p2");

	QCOMPARE(p5DirectDependencies, QSet<QString>{} << "p6");
	QCOMPARE(p5DirectDependents, QSet<QString>{} << "p3" << "p4");

	QCOMPARE(p6DirectDependencies, QSet<QString>{});
	QCOMPARE(p6DirectDependents, QSet<QString>{} << "p5");
}

void tst_PluginDependencyGraph::treeDependencyTest()
{
	auto graph = treeGraph();

	auto p1Dependencies = graph.findDependencies("p1");
	auto p1Dependents = graph.findDependents("p1");

	auto p6Dependencies = graph.findDependencies("p6");
	auto p6Dependents = graph.findDependents("p6");

	QCOMPARE(p1Dependencies.size(), 5);
	auto p1DependencyIndex = p1Dependencies.indexOf("p1");
	auto p2DependencyIndex = p1Dependencies.indexOf("p2");
	auto p3DependencyIndex = p1Dependencies.indexOf("p3");
	auto p4DependencyIndex = p1Dependencies.indexOf("p4");
	auto p5DependencyIndex = p1Dependencies.indexOf("p5");
	auto p6DependencyIndex = p1Dependencies.indexOf("p6");
	QCOMPARE(-1, p1DependencyIndex);
	QVERIFY(p2DependencyIndex >= 0);
	QVERIFY(p3DependencyIndex >= 0);
	QVERIFY(p4DependencyIndex >= 0);
	QVERIFY(p5DependencyIndex >= 0);
	QVERIFY(p6DependencyIndex >= 0);
	QVERIFY(p6DependencyIndex < p5DependencyIndex);
	QVERIFY(p5DependencyIndex < p4DependencyIndex);
	QVERIFY(p5DependencyIndex < p3DependencyIndex);
	QVERIFY(p4DependencyIndex < p2DependencyIndex);
	QVERIFY(p3DependencyIndex < p2DependencyIndex);

	QCOMPARE(p1Dependents.size(), 0);
	QCOMPARE(p6Dependencies.size(), 0);

	QCOMPARE(p6Dependents.size(), 5);
	auto p1DependentIndex = p6Dependents.indexOf("p1");
	auto p2DependentIndex = p6Dependents.indexOf("p2");
	auto p3DependentIndex = p6Dependents.indexOf("p3");
	auto p4DependentIndex = p6Dependents.indexOf("p4");
	auto p5DependentIndex = p6Dependents.indexOf("p5");
	auto p6DependentIndex = p6Dependents.indexOf("p6");
	QVERIFY(p1DependentIndex >= 0);
	QVERIFY(p2DependentIndex >= 0);
	QVERIFY(p3DependentIndex >= 0);
	QVERIFY(p3DependentIndex >= 0);
	QVERIFY(p5DependentIndex >= 0);
	QCOMPARE(-1, p6DependentIndex);
	QVERIFY(p1DependentIndex < p2DependentIndex);
	QVERIFY(p2DependentIndex < p3DependentIndex);
	QVERIFY(p2DependentIndex < p4DependentIndex);
	QVERIFY(p3DependentIndex < p5DependentIndex);
	QVERIFY(p4DependentIndex < p5DependentIndex);
}

void tst_PluginDependencyGraph::cycleCycleTest()
{
	QCOMPARE(cycleGraph().findPluginsInDependencyCycle(), QSet<QString>{} << "p2" << "p3" << "p4" << "p5");
}

void tst_PluginDependencyGraph::cycleDirectDependencyTest()
{
	auto graph = cycleGraph();

	auto p1DirectDependencies = graph.directDependencies("p1");
	auto p1DirectDependents = graph.directDependents("p1");
	auto p2DirectDependencies = graph.directDependencies("p2");
	auto p2DirectDependents = graph.directDependents("p2");
	auto p3DirectDependencies = graph.directDependencies("p3");
	auto p3DirectDependents = graph.directDependents("p3");
	auto p4DirectDependencies = graph.directDependencies("p4");
	auto p4DirectDependents = graph.directDependents("p4");
	auto p5DirectDependencies = graph.directDependencies("p5");
	auto p5DirectDependents = graph.directDependents("p5");
	auto p6DirectDependencies = graph.directDependencies("p6");
	auto p6DirectDependents = graph.directDependents("p6");

	QCOMPARE(p1DirectDependencies, QSet<QString>{} << "p2");
	QCOMPARE(p1DirectDependents, QSet<QString>{});

	QCOMPARE(p2DirectDependencies, QSet<QString>{} << "p3" << "p4");
	QCOMPARE(p2DirectDependents, QSet<QString>{} << "p1" << "p5");

	QCOMPARE(p3DirectDependencies, QSet<QString>{} << "p5");
	QCOMPARE(p3DirectDependents, QSet<QString>{} << "p2");

	QCOMPARE(p4DirectDependencies, QSet<QString>{} << "p5");
	QCOMPARE(p4DirectDependents, QSet<QString>{} << "p2");

	QCOMPARE(p5DirectDependencies, QSet<QString>{} << "p6" << "p2");
	QCOMPARE(p5DirectDependents, QSet<QString>{} << "p3" << "p4");

	QCOMPARE(p6DirectDependencies, QSet<QString>{});
	QCOMPARE(p6DirectDependents, QSet<QString>{} << "p5");
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

	expectCycle([&]{ graph.findDependencies("p1"); });
	QCOMPARE(graph.findDependents("p1"), QVector<QString>{});
	expectCycle([&]{ graph.findDependencies("p2"); });
	expectCycle([&]{ graph.findDependents("p2"); });
	expectCycle([&]{ graph.findDependencies("p3"); });
	expectCycle([&]{ graph.findDependents("p3"); });
	expectCycle([&]{ graph.findDependencies("p4"); });
	expectCycle([&]{ graph.findDependents("p4"); });
	expectCycle([&]{ graph.findDependencies("p5"); });
	expectCycle([&]{ graph.findDependents("p5"); });
	QCOMPARE(graph.findDependencies("p6"), QVector<QString>{});
	expectCycle([&]{ graph.findDependents("p6"); });
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraph)
#include "tst-plugin-dependency-graph.moc"
