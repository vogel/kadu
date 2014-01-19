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
#include "plugin/dependency-graph/plugin-dependency-graph-builder.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/metadata/plugin-metadata.h"
#include "plugin/metadata/plugin-metadata-builder.h"
#include "plugin/metadata/plugin-metadata-provider.h"

#include <algorithm>
#include <map>
#include <QtTest/QtTest>

namespace {
class SimpleMetadataProvider : public PluginMetadataProvider
{
	Q_OBJECT

public:
	SimpleMetadataProvider(std::map<QString, PluginMetadata> metadataSet, QObject *parent = nullptr) :
			PluginMetadataProvider{parent}, m_metadataSet{std::move(metadataSet)} {}
	virtual ~SimpleMetadataProvider() {}
	virtual std::map<QString, PluginMetadata> provide() noexcept override { return m_metadataSet; }

private:
	std::map<QString, PluginMetadata> m_metadataSet;

};
}

class tst_PluginDependencyHandler : public QObject
{
	Q_OBJECT

private:
	std::unique_ptr<PluginDependencyHandler> createPluginDependencyHandler(const QVector<QPair<QString, QStringList>> &plugins);
	SimpleMetadataProvider * createMetatadataProvider(const QVector<QPair<QString, QStringList>> &plugins, QObject *parent);
	PluginMetadata createPluginMetadata(const QPair<QString, QStringList> &plugin);
	void verifyDependencies(const PluginDependencyGraph &graph, const QString &pluginName, const QStringList &dependencies, const QStringList &dependents);

private slots:
	void simpleDependencyTest();
	void selfDependencyTest();
	void pluginOnlyAsDependencyTest();
	void cycleDependencyTest();

};

std::unique_ptr<PluginDependencyHandler> tst_PluginDependencyHandler::createPluginDependencyHandler(const QVector<QPair<QString, QStringList>> &plugins)
{
	auto result = make_unique<PluginDependencyHandler>();
	result.get()->setPluginDependencyGraphBuilder(new PluginDependencyGraphBuilder{result.get()});
	result.get()->setPluginMetadataProvider(createMetatadataProvider(plugins, result.get()));
	result.get()->initialize();
	return std::move(result);
}

SimpleMetadataProvider * tst_PluginDependencyHandler::createMetatadataProvider(const QVector<QPair<QString, QStringList>> &plugins, QObject *parent)
{
	auto result = std::map<QString, PluginMetadata>{};
	for (auto const &plugin : plugins)
		result.insert({plugin.first, createPluginMetadata(plugin)});
	return new SimpleMetadataProvider{result, parent};
}

PluginMetadata tst_PluginDependencyHandler::createPluginMetadata(const QPair<QString, QStringList> &plugin)
{
	auto builder = PluginMetadataBuilder{};
	return builder
			.setName(plugin.first)
			.setDependencies(plugin.second)
			.create();
}

void tst_PluginDependencyHandler::verifyDependencies(const PluginDependencyGraph &graph, const QString &pluginName, const QStringList &dependencies, const QStringList &dependents)
{
	auto graphDependencies = graph.directDependencies(pluginName);
	auto graphDependents = graph.directDependents(pluginName);

	QCOMPARE(graphDependencies.toList().toSet(), dependencies.toSet());
	QCOMPARE(graphDependents.toList().toSet(), dependents.toSet());
	QCOMPARE(graphDependencies.size(), dependencies.size());
	QCOMPARE(graphDependents.size(), dependents.size());
}

void tst_PluginDependencyHandler::simpleDependencyTest()
{
	auto handler = createPluginDependencyHandler(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p2", "p3", "p4"}),
		qMakePair(QString{"p2"}, QStringList{"p3", "p4"}),
		qMakePair(QString{"p3"}, QStringList{"p4"}),
		qMakePair(QString{"p4"}, QStringList{})
	});

	QCOMPARE(handler.get()->pluginNames(), (std::set<QString>{"p1", "p2", "p3", "p4"}));
	QVERIFY(handler.get()->hasPluginMetadata("p1"));
	QVERIFY(handler.get()->hasPluginMetadata("p2"));
	QVERIFY(handler.get()->hasPluginMetadata("p3"));
	QVERIFY(handler.get()->hasPluginMetadata("p4"));
	QVERIFY(!handler.get()->hasPluginMetadata("p5"));
	QCOMPARE(handler.get()->pluginMetadata("p1").name(), QString{"p1"});
	QCOMPARE(handler.get()->pluginMetadata("p2").name(), QString{"p2"});
	QCOMPARE(handler.get()->pluginMetadata("p3").name(), QString{"p3"});
	QCOMPARE(handler.get()->pluginMetadata("p4").name(), QString{"p4"});
	QCOMPARE(handler.get()->withDependencies("p1"), QVector<QString>{} << "p4" << "p3" << "p2" << "p1");
	QCOMPARE(handler.get()->withDependents("p1"), QVector<QString>{} << "p1");
	QCOMPARE(handler.get()->withDependencies("p2"), QVector<QString>{} << "p4" << "p3" << "p2");
	QCOMPARE(handler.get()->withDependents("p2"), QVector<QString>{} << "p1" << "p2");
	QCOMPARE(handler.get()->withDependencies("p3"), QVector<QString>{} << "p4" << "p3");
	QCOMPARE(handler.get()->withDependents("p3"), QVector<QString>{} << "p1" << "p2" << "p3");
	QCOMPARE(handler.get()->withDependencies("p4"), QVector<QString>{} << "p4");
	QCOMPARE(handler.get()->withDependents("p4"), QVector<QString>{} << "p1" << "p2" << "p3" << "p4");
	QCOMPARE(handler.get()->withDependencies("p5"), QVector<QString>{});
	QCOMPARE(handler.get()->withDependents("p5"), QVector<QString>{});
}

void tst_PluginDependencyHandler::selfDependencyTest()
{
	auto handler = createPluginDependencyHandler(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p1"})
	});

	QCOMPARE(handler.get()->pluginNames(), std::set<QString>{"p1"});
	QVERIFY(handler.get()->hasPluginMetadata("p1"));
	QVERIFY(!handler.get()->hasPluginMetadata("p2"));
	QCOMPARE(handler.get()->pluginMetadata("p1").name(), QString{"p1"});
	QCOMPARE(handler.get()->withDependencies("p1"), QVector<QString>{} << "p1");
	QCOMPARE(handler.get()->withDependents("p1"), QVector<QString>{} << "p1");
}

void tst_PluginDependencyHandler::pluginOnlyAsDependencyTest()
{
	auto handler = createPluginDependencyHandler(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p2"}),
		qMakePair(QString{"p2"}, QStringList{"p3"}),
		qMakePair(QString{"p3"}, QStringList{"p4"})
	});

	QCOMPARE(handler.get()->pluginNames(), std::set<QString>{});
	QVERIFY(!handler.get()->hasPluginMetadata("p1"));
	QVERIFY(!handler.get()->hasPluginMetadata("p2"));
	QVERIFY(!handler.get()->hasPluginMetadata("p3"));
	QVERIFY(!handler.get()->hasPluginMetadata("p4"));
}

void tst_PluginDependencyHandler::cycleDependencyTest()
{
	auto handler = createPluginDependencyHandler(QVector<QPair<QString, QStringList>>
	{
		qMakePair(QString{"p1"}, QStringList{"p2"}),
		qMakePair(QString{"p2"}, QStringList{"p1"})
	});

	QCOMPARE(handler.get()->pluginNames(), std::set<QString>{});
	QVERIFY(!handler.get()->hasPluginMetadata("p1"));
	QVERIFY(!handler.get()->hasPluginMetadata("p2"));
}

QTEST_APPLESS_MAIN(tst_PluginDependencyHandler)
#include "tst-plugin-dependency-handler.moc"
