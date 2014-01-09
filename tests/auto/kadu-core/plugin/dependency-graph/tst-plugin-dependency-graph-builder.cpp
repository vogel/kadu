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
#include "misc/memory.h"
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugin/plugin-metadata.h"
#include "plugin/plugin-metadata-builder.h"
#include "plugin/plugin-metadata-repository.h"

#include <algorithm>
#include <QtTest/QtTest>

class tst_PluginDependencyGraphBuilder : public QObject
{
	Q_OBJECT

private:
	std::unique_ptr<PluginMetadataRepository> createPluginMetadataRepository(const QVector<QPair<QString, QStringList>> &plugins);
	PluginMetadata createPluginMetadata(const QPair<QString, QStringList> &plugin);

private slots:
	void simpleDependencyTest();
	void selfDependencyTest();

};

std::unique_ptr<PluginMetadataRepository> tst_PluginDependencyGraphBuilder::createPluginMetadataRepository(const QVector<QPair<QString, QStringList>> &plugins)
{
	auto result = make_unique<PluginMetadataRepository>();
	for (auto const &plugin : plugins)
		result.get()->addPluginMetadata(plugin.first, createPluginMetadata(plugin));
	return result;
}

PluginMetadata tst_PluginDependencyGraphBuilder::createPluginMetadata(const QPair<QString, QStringList> &plugin)
{
	auto builder = PluginMetadataBuilder{};
	return builder
			.setName(plugin.first)
			.setDependencies(plugin.second)
			.create();
}

void tst_PluginDependencyGraphBuilder::simpleDependencyTest()
{
	auto plugins = QVector<QPair<QString, QStringList>>{};
	plugins.append(qMakePair(QString("p1"), QStringList() << "p2" << "p3" << "p4"));
	plugins.append(qMakePair(QString("p2"), QStringList() << "p3" << "p4"));
	plugins.append(qMakePair(QString("p3"), QStringList() << "p4"));

	auto graph = PluginDependencyGraphBuilder{}.buildGraph(*createPluginMetadataRepository(plugins).get());

	QCOMPARE(graph.get()->size(), 4);

	auto p1Dependencies = graph.get()->directDependencies("p1");
	auto p1Dependents = graph.get()->directDependents("p1");
	auto p2Dependencies = graph.get()->directDependencies("p2");
	auto p2Dependents = graph.get()->directDependents("p2");
	auto p3Dependencies = graph.get()->directDependencies("p3");
	auto p3Dependents = graph.get()->directDependents("p3");
	auto p4Dependencies = graph.get()->directDependencies("p4");
	auto p4Dependents = graph.get()->directDependents("p4");

	QVERIFY(contains(p1Dependencies, "p2"));
	QVERIFY(contains(p1Dependencies, "p3"));
	QVERIFY(contains(p1Dependencies, "p4"));
	QCOMPARE(p1Dependencies.size(), 3);
	QCOMPARE(p1Dependents.size(), 0);

	QVERIFY(contains(p2Dependencies, "p3"));
	QVERIFY(contains(p2Dependencies, "p4"));
	QVERIFY(contains(p2Dependents, "p1"));
	QCOMPARE(p2Dependencies.size(), 2);
	QCOMPARE(p2Dependents.size(), 1);

	QVERIFY(contains(p3Dependencies, "p4"));
	QVERIFY(contains(p3Dependents, "p1"));
	QVERIFY(contains(p3Dependents, "p2"));
	QCOMPARE(p3Dependencies.size(), 1);
	QCOMPARE(p3Dependents.size(), 2);

	QVERIFY(contains(p4Dependents, "p1"));
	QVERIFY(contains(p4Dependents, "p2"));
	QVERIFY(contains(p4Dependents, "p3"));
	QCOMPARE(p4Dependencies.size(), 0);
	QCOMPARE(p4Dependents.size(), 3);
}

void tst_PluginDependencyGraphBuilder::selfDependencyTest()
{
	auto plugins = QVector<QPair<QString, QStringList>>{};
	plugins.append(qMakePair(QString("p1"), QStringList() << "p1"));

	auto graph = PluginDependencyGraphBuilder{}.buildGraph(*createPluginMetadataRepository(plugins).get());

	QCOMPARE(graph.get()->size(), 1);

	auto p1Dependencies = graph.get()->directDependencies("p1");
	auto p1Dependents = graph.get()->directDependents("p1");
	QCOMPARE(p1Dependencies.size(), 0);
	QCOMPARE(p1Dependents.size(), 0);
}

QTEST_APPLESS_MAIN(tst_PluginDependencyGraphBuilder)
#include "tst-plugin-dependency-graph-builder.moc"
