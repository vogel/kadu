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
#include "plugin/dependency-graph/plugin-dependency-graph.h"
#include "plugin/metadata/plugin-metadata.h"

#include <QtTest/QtTest>
#include <algorithm>
#include <map>

class PluginDependencyGraphBuilderTest : public QObject
{
    Q_OBJECT

private:
    using PluginTuple = std::tuple<QString, QString, QStringList>;

    std::map<QString, PluginMetadata> createPlugins(const QVector<PluginTuple> &plugins);
    PluginMetadata createPluginMetadata(const PluginTuple &plugin);
    void verifyDependencies(
        const PluginDependencyGraph &graph, const QString &pluginName, const QStringList &dependencies,
        const QStringList &dependents);

private slots:
    void simpleDependencyTest();
    void selfDependencyTest();
    void pluginOnlyAsDependencyTest();
    void cycleDependencyTest();
    void singleProvidesTest();
    void noConflictingProvidesTest();
    void conflictingProvidesTest();
    void conflictingWithCyclesProvidesTest();
};

::std::map<QString, PluginMetadata> PluginDependencyGraphBuilderTest::createPlugins(const QVector<PluginTuple> &plugins)
{
    auto result = ::std::map<QString, PluginMetadata>{};
    for (auto const &plugin : plugins)
        result.insert({std::get<0>(plugin), createPluginMetadata(plugin)});
    return result;
}

PluginMetadata PluginDependencyGraphBuilderTest::createPluginMetadata(const PluginTuple &plugin)
{
    auto result = PluginMetadata{};
    result.name = std::get<0>(plugin);
    result.provides = std::get<1>(plugin);
    result.dependencies = std::get<2>(plugin);

    return result;
}

void PluginDependencyGraphBuilderTest::verifyDependencies(
    const PluginDependencyGraph &graph, const QString &pluginName, const QStringList &dependencies,
    const QStringList &dependents)
{
    auto graphDependencies = graph.directDependencies(pluginName);
    auto graphDependents = graph.directDependents(pluginName);

    QCOMPARE(graphDependencies.toList().toSet(), dependencies.toSet());
    QCOMPARE(graphDependents.toList().toSet(), dependents.toSet());
    QCOMPARE(graphDependencies.size(), dependencies.size());
    QCOMPARE(graphDependents.size(), dependents.size());
}

void PluginDependencyGraphBuilderTest::simpleDependencyTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{}, QStringList{"p2", "p3", "p4"}),
                                           std::make_tuple(QString{"p2"}, QString{}, QStringList{"p3", "p4"}),
                                           std::make_tuple(QString{"p3"}, QString{}, QStringList{"p4"}),
                                           std::make_tuple(QString{"p4"}, QString{}, QStringList{})}));

    QCOMPARE(graph.size(), 4);

    verifyDependencies(graph, "p1", {"p2", "p3", "p4"}, {});
    verifyDependencies(graph, "p2", {"p3", "p4"}, {"p1"});
    verifyDependencies(graph, "p3", {"p4"}, {"p1", "p2"});
    verifyDependencies(graph, "p4", {}, {"p1", "p2", "p3"});
}

void PluginDependencyGraphBuilderTest::selfDependencyTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{}, QStringList{"p1"})}));

    QCOMPARE(graph.size(), 1);

    verifyDependencies(graph, "p1", {}, {});
}

void PluginDependencyGraphBuilderTest::pluginOnlyAsDependencyTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{}, QStringList{"p2"}),
                                           std::make_tuple(QString{"p2"}, QString{}, QStringList{"p3"}),
                                           std::make_tuple(QString{"p3"}, QString{}, QStringList{"p4"})}));

    QCOMPARE(graph.size(), 0);
}

void PluginDependencyGraphBuilderTest::cycleDependencyTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{}, QStringList{"p2"}),
                                           std::make_tuple(QString{"p2"}, QString{}, QStringList{"p1"})}));

    QCOMPARE(graph.size(), 0);
}

void PluginDependencyGraphBuilderTest::singleProvidesTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{}, QStringList{"p2"}),
                                           std::make_tuple(QString{"p2"}, QString{}, QStringList{"p3"}),
                                           std::make_tuple(QString{"p3"}, QString{"feature3"}, QStringList{})}));

    QCOMPARE(graph.size(), 3);

    verifyDependencies(graph, "p1", {"p2"}, {});
    verifyDependencies(graph, "p2", {"p3"}, {"p1"});
    verifyDependencies(graph, "p3", {}, {"p2"});
}

void PluginDependencyGraphBuilderTest::noConflictingProvidesTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{"feature1"}, QStringList{"p2"}),
                                           std::make_tuple(QString{"p2"}, QString{"feature2"}, QStringList{"p3"}),
                                           std::make_tuple(QString{"p3"}, QString{"feature3"}, QStringList{})}));

    QCOMPARE(graph.size(), 3);

    verifyDependencies(graph, "p1", {"p2"}, {});
    verifyDependencies(graph, "p2", {"p3"}, {"p1"});
    verifyDependencies(graph, "p3", {}, {"p2"});
}

void PluginDependencyGraphBuilderTest::conflictingProvidesTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{"feature12"}, QStringList{"p2"}),
                                           std::make_tuple(QString{"p2"}, QString{"feature12"}, QStringList{"p5"}),
                                           std::make_tuple(QString{"p3"}, QString{"feature3"}, QStringList{"p4"}),
                                           std::make_tuple(QString{"p4"}, QString{"feature4"}, QStringList{"p5"}),
                                           std::make_tuple(QString{"p5"}, QString{"feature5"}, QStringList{})}));

    QCOMPARE(graph.size(), 4);

    verifyDependencies(graph, "p2", {"p5"}, {});
    verifyDependencies(graph, "p3", {"p4"}, {});
    verifyDependencies(graph, "p4", {"p5"}, {"p3"});
    verifyDependencies(graph, "p5", {}, {"p2", "p4"});
}

void PluginDependencyGraphBuilderTest::conflictingWithCyclesProvidesTest()
{
    auto graph = PluginDependencyGraphBuilder{}.buildValidGraph(
        createPlugins(QVector<PluginTuple>{std::make_tuple(QString{"p1"}, QString{"feature12"}, QStringList{"p2"}),
                                           std::make_tuple(QString{"p2"}, QString{"feature12"}, QStringList{"p5"}),
                                           std::make_tuple(QString{"p3"}, QString{"feature3"}, QStringList{"p4"}),
                                           std::make_tuple(QString{"p4"}, QString{"feature47"}, QStringList{"p6"}),
                                           std::make_tuple(QString{"p5"}, QString{"feature5"}, QStringList{}),
                                           std::make_tuple(QString{"p5"}, QString{"feature6"}, QStringList{"p7"}),
                                           std::make_tuple(QString{"p7"}, QString{"feature47"}, QStringList{"p8"}),
                                           std::make_tuple(QString{"p8"}, QString{}, QStringList{"p3"})}));

    QCOMPARE(graph.size(), 2);

    verifyDependencies(graph, "p2", {"p5"}, {});
    verifyDependencies(graph, "p5", {}, {"p2"});
}

QTEST_APPLESS_MAIN(PluginDependencyGraphBuilderTest)
#include "plugin-dependency-graph-builder.test.moc"
