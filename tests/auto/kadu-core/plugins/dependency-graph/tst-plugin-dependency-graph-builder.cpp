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
#include "plugins/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugin-repository.h"

#include <algorithm>
#include <QtTest/QtTest>

class tst_PluginDependencyGraphBuilder : public QObject
{
	Q_OBJECT

private:
	template<typename T>
	bool contains(const std::vector<T> &v, T value);
	std::unique_ptr<PluginRepository> createPluginRepository(const QVector<QPair<QString, QStringList>> &plugins);
	std::unique_ptr<Plugin> createPlugin(const QPair<QString, QStringList> &plugin);

private slots:
	void simpleDependencyTest();
	void selfDependencyTest();

};

template<typename T>
bool tst_PluginDependencyGraphBuilder::contains(const std::vector<T> &v, T value)
{
	return std::find(v.begin(), v.end(), value) != v.end();
}

std::unique_ptr<PluginRepository> tst_PluginDependencyGraphBuilder::createPluginRepository(const QVector<QPair<QString, QStringList>> &plugins)
{
	auto result = make_unique<PluginRepository>();
	for (auto const &plugin : plugins)
		result.get()->addPlugin(plugin.first, createPlugin(plugin).release());
	return result;
}

std::unique_ptr<Plugin> tst_PluginDependencyGraphBuilder::createPlugin(const QPair<QString, QStringList> &plugin)
{
	auto info = PluginInfo
	{
		plugin.first,
		plugin.first,
		"category",
		"type",
		"description",
		"author",
		"version",
		plugin.second,
		QStringList(),
		QStringList(),
		QStringList(),
		false
	};

	return make_unique<Plugin>(info);
}

void tst_PluginDependencyGraphBuilder::simpleDependencyTest()
{
	auto plugins = QVector<QPair<QString, QStringList>>{};
	plugins.append(qMakePair(QString("p1"), QStringList() << "p2" << "p3" << "p4"));
	plugins.append(qMakePair(QString("p2"), QStringList() << "p3" << "p4"));
	plugins.append(qMakePair(QString("p3"), QStringList() << "p4"));

	auto pluginRepository = createPluginRepository(plugins);
	PluginDependencyGraphBuilder pluginDependencyGraphBuilder;
	pluginDependencyGraphBuilder.setPluginRepository(pluginRepository.get());

	auto graph = pluginDependencyGraphBuilder.buildGraph();

	QCOMPARE(graph.get()->nodes().size(), 4UL);

	auto p1 = graph.get()->node("p1");
	auto p2 = graph.get()->node("p2");
	auto p3 = graph.get()->node("p3");
	auto p4 = graph.get()->node("p4");
	auto p5 = graph.get()->node("p5");

	QVERIFY(p1);
	QVERIFY(p2);
	QVERIFY(p3);
	QVERIFY(p4);
	QVERIFY(!p5);

	QVERIFY(contains(p1->successors<PluginDependencyTag>(), p2));
	QVERIFY(contains(p1->successors<PluginDependencyTag>(), p3));
	QVERIFY(contains(p1->successors<PluginDependencyTag>(), p4));
	QCOMPARE(p1->successors<PluginDependencyTag>().size(), 3UL);
	QCOMPARE(p1->successors<PluginDependentTag>().size(), 0UL);

	QVERIFY(contains(p2->successors<PluginDependencyTag>(), p3));
	QVERIFY(contains(p2->successors<PluginDependencyTag>(), p4));
	QVERIFY(contains(p2->successors<PluginDependentTag>(), p1));
	QCOMPARE(p2->successors<PluginDependencyTag>().size(), 2UL);
	QCOMPARE(p2->successors<PluginDependentTag>().size(), 1UL);

	QVERIFY(contains(p3->successors<PluginDependencyTag>(), p4));
	QVERIFY(contains(p3->successors<PluginDependentTag>(), p1));
	QVERIFY(contains(p3->successors<PluginDependentTag>(), p2));
	QCOMPARE(p3->successors<PluginDependencyTag>().size(), 1UL);
	QCOMPARE(p3->successors<PluginDependentTag>().size(), 2UL);

	QVERIFY(contains(p4->successors<PluginDependentTag>(), p1));
	QVERIFY(contains(p4->successors<PluginDependentTag>(), p2));
	QVERIFY(contains(p4->successors<PluginDependentTag>(), p3));
	QCOMPARE(p4->successors<PluginDependencyTag>().size(), 0UL);
	QCOMPARE(p4->successors<PluginDependentTag>().size(), 3UL);
}

void tst_PluginDependencyGraphBuilder::selfDependencyTest()
{
	auto plugins = QVector<QPair<QString, QStringList>>{};
	plugins.append(qMakePair(QString("p1"), QStringList() << "p1"));

	auto pluginRepository = createPluginRepository(plugins);
	PluginDependencyGraphBuilder pluginDependencyGraphBuilder;
	pluginDependencyGraphBuilder.setPluginRepository(pluginRepository.get());

	auto graph = pluginDependencyGraphBuilder.buildGraph();

	QCOMPARE(graph.get()->nodes().size(), 1UL);

	auto p1 = graph.get()->node("p1");

	QVERIFY(p1);

	QCOMPARE(p1->successors<PluginDependencyTag>().size(), 0UL);
	QCOMPARE(p1->successors<PluginDependentTag>().size(), 0UL);
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraphBuilder)
#include "tst-plugin-dependency-graph-builder.moc"
