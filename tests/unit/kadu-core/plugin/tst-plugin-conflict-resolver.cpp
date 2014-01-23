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
#include "plugin/plugin-conflict-resolver.h"
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

class tst_PluginConflictResolver : public QObject
{
	Q_OBJECT

private:
	using PluginTuple = std::tuple<QString, QString, QStringList>;

	std::unique_ptr<PluginConflictResolver> createPluginConflictResolver(const QVector<PluginTuple> &plugins);
	PluginDependencyHandler * createPluginDependencyHandler(const QVector<PluginTuple> &plugins, QObject *parent);
	SimpleMetadataProvider * createMetatadataProvider(const QVector<PluginTuple> &plugins, QObject *parent);
	PluginMetadata createPluginMetadata(const PluginTuple &plugin);

private slots:
	void noConflicts();
	void simpleConflict();
	void dependencyConflict();

};

std::unique_ptr<PluginConflictResolver> tst_PluginConflictResolver::createPluginConflictResolver(const QVector<PluginTuple> &plugins)
{
	auto result = make_unique<PluginConflictResolver>();
	result.get()->setPluginDependencyHandler(createPluginDependencyHandler(plugins, result.get()));
	return result;
}

PluginDependencyHandler * tst_PluginConflictResolver::createPluginDependencyHandler(const QVector<PluginTuple> &plugins, QObject *parent)
{
	auto result = new PluginDependencyHandler{parent};
	result->setPluginDependencyGraphBuilder(new PluginDependencyGraphBuilder{result});
	result->setPluginMetadataProvider(createMetatadataProvider(plugins, result));
	result->initialize();
	return result;
}

SimpleMetadataProvider * tst_PluginConflictResolver::createMetatadataProvider(const QVector<PluginTuple> &plugins, QObject *parent)
{
	auto result = std::map<QString, PluginMetadata>{};
	for (auto const &plugin : plugins)
		result.insert({std::get<0>(plugin), createPluginMetadata(plugin)});
	return new SimpleMetadataProvider{result, parent};
}

PluginMetadata tst_PluginConflictResolver::createPluginMetadata(const PluginTuple &plugin)
{
	auto builder = PluginMetadataBuilder{};
	return builder
			.setName(std::get<0>(plugin))
			.setProvides(std::get<1>(plugin))
			.setDependencies(std::get<2>(plugin))
			.create();
}

void tst_PluginConflictResolver::noConflicts()
{
	auto resolver = createPluginConflictResolver(QVector<PluginTuple>
	{
		std::make_tuple(QString{"p1"}, QString{}, QStringList{"p2", "p3", "p4"}),
		std::make_tuple(QString{"p2"}, QString{}, QStringList{"p3", "p4"}),
		std::make_tuple(QString{"p3"}, QString{}, QStringList{"p4"}),
		std::make_tuple(QString{"p4"}, QString{}, QStringList{})
	});

	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p1"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2", "p3"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2", "p4"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3", "p4"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2", "p3", "p4"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p4"), {});
}

void tst_PluginConflictResolver::simpleConflict()
{
	auto resolver = createPluginConflictResolver(QVector<PluginTuple>
	{
		std::make_tuple(QString{"p1"}, QString{"feature13"}, QStringList{}),
		std::make_tuple(QString{"p2"}, QString{"feature24"}, QStringList{}),
		std::make_tuple(QString{"p3"}, QString{"feature13"}, QStringList{}),
		std::make_tuple(QString{"p4"}, QString{"feature24"}, QStringList{})
	});

	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p1"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p1"), {"p3"});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p1"}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p2"), {"p4"});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p1"}, "p3"), {"p1"});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p4"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p1"}, "p4"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p4"), {"p2"});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p4"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p4"), {});
}

void tst_PluginConflictResolver::dependencyConflict()
{
	auto resolver = createPluginConflictResolver(QVector<PluginTuple>
	{
		std::make_tuple(QString{"p0"}, QString{}, QStringList{"p1"}),
		std::make_tuple(QString{"p1"}, QString{"feature13"}, QStringList{"p2"}),
		std::make_tuple(QString{"p2"}, QString{"feature24"}, QStringList{}),
		std::make_tuple(QString{"p3"}, QString{"feature13"}, QStringList{}),
		std::make_tuple(QString{"p4"}, QString{"feature24"}, QStringList{})
	});

	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p0", "p1", "p2"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p1", "p2"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p1"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p1"), {"p3"});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p1"), {"p4"});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p0", "p1", "p2"}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p1", "p2"}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p2"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p2"), {"p4"});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p0", "p1", "p2"}, "p3"), std::set<QString>({"p0", "p1"}));
	QCOMPARE(resolver.get()->conflictingPlugins({"p1", "p2"}, "p3"), {"p1"});
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p3"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({""}, "p4"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p0", "p1", "p2"}, "p4"), std::set<QString>({"p0", "p1", "p2"}));
	QCOMPARE(resolver.get()->conflictingPlugins({"p1", "p2"}, "p4"), std::set<QString>({"p1", "p2"}));
	QCOMPARE(resolver.get()->conflictingPlugins({"p2"}, "p4"), {"p2"});
	QCOMPARE(resolver.get()->conflictingPlugins({"p3"}, "p4"), {});
	QCOMPARE(resolver.get()->conflictingPlugins({"p4"}, "p4"), {});
}

QTEST_APPLESS_MAIN(tst_PluginConflictResolver)
#include "tst-plugin-conflict-resolver.moc"
