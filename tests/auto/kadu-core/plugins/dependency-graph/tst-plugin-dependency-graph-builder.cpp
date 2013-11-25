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
#include "misc/string-utils.h"
#include "plugins/dependency-graph/plugin-dependency-graph.h"
#include "plugins/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugins/dependency-graph/plugin-dependency-graph-node.h"
#include "plugins/plugin.h"
#include "plugins/plugin-info.h"
#include "plugins/plugin-repository.h"

#include <algorithm>
#include <QtTest/QtTest>

class tst_PluginDependencyGraphBuilder : public QObject
{
	Q_OBJECT

private:
	std::unique_ptr<PluginRepository> createPluginRepository(const QVector<QPair<QString, QStringList>> &plugins);
	std::unique_ptr<Plugin> createPlugin(const QPair<QString, QStringList> &plugin);

private slots:
	void simpleDependencyTest();

};

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

	QVERIFY(p1->dependencies().contains(p2));
	QVERIFY(p1->dependencies().contains(p3));
	QVERIFY(p1->dependencies().contains(p4));

	QVERIFY(p2->dependencies().contains(p3));
	QVERIFY(p2->dependencies().contains(p4));
	QVERIFY(p2->dependents().contains(p1));

	QVERIFY(p3->dependencies().contains(p4));
	QVERIFY(p3->dependents().contains(p2));

	QVERIFY(p4->dependents().contains(p3));
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraphBuilder)
#include "tst-plugin-dependency-graph-builder.moc"
