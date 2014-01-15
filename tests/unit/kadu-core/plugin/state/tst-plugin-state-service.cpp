/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin/state/plugin-state.h"
#include "plugin/state/plugin-state-service.h"

#include <QtTest/QtTest>

class tst_PluginStateService : public QObject
{
	Q_OBJECT

private slots:
	void noDataServiceTest();
	void setPluginStateTest();
	void setPluginStatesTest();
	void dynamicPluginStateTest();

};

void tst_PluginStateService::noDataServiceTest()
{
	PluginStateService service{};

	QCOMPARE(service.pluginStates().size(), 0);
	QCOMPARE(service.pluginState("p1"), PluginState::New);
	QCOMPARE(service.pluginState("p2"), PluginState::New);
	QCOMPARE(service.enabledPlugins(), {});
}

void tst_PluginStateService::setPluginStateTest()
{
	PluginStateService service{};

	service.setPluginState("p1", PluginState::New);
	service.setPluginState("p2", PluginState::Enabled);
	service.setPluginState("p3", PluginState::Disabled);

	auto pluginStates = service.pluginStates();
	QCOMPARE(pluginStates.size(), 2);
	QCOMPARE(pluginStates.value("p1"), PluginState::New);
	QCOMPARE(pluginStates.value("p2"), PluginState::Enabled);
	QCOMPARE(pluginStates.value("p3"), PluginState::Disabled);
	QCOMPARE(service.pluginState("p1"), PluginState::New);
	QCOMPARE(service.pluginState("p2"), PluginState::Enabled);
	QCOMPARE(service.pluginState("p3"), PluginState::Disabled);
	QCOMPARE(service.enabledPlugins(), {"p2"});
}

void tst_PluginStateService::setPluginStatesTest()
{
	PluginStateService service{};

	auto setStates = QMap<QString, PluginState>{};
	setStates.insert("p1", PluginState::New);
	setStates.insert("p2", PluginState::Enabled);
	setStates.insert("p3", PluginState::Disabled);

	service.setPluginStates(setStates);

	auto pluginStates = service.pluginStates();
	QCOMPARE(pluginStates.size(), 2);
	QCOMPARE(pluginStates.value("p1"), PluginState::New);
	QCOMPARE(pluginStates.value("p2"), PluginState::Enabled);
	QCOMPARE(pluginStates.value("p3"), PluginState::Disabled);
	QCOMPARE(service.pluginState("p1"), PluginState::New);
	QCOMPARE(service.pluginState("p2"), PluginState::Enabled);
	QCOMPARE(service.pluginState("p3"), PluginState::Disabled);
	QCOMPARE(service.enabledPlugins(), {"p2"});
}

void tst_PluginStateService::dynamicPluginStateTest()
{
	PluginStateService service{};

	service.setPluginState("p1", PluginState::New);

	QCOMPARE(service.pluginStates().size(), 0);
	QCOMPARE(service.pluginStates().value("p1"), PluginState::New);
	QCOMPARE(service.pluginState("p1"), PluginState::New);
	QCOMPARE(service.enabledPlugins(), {});

	service.setPluginState("p1", PluginState::Enabled);

	QCOMPARE(service.pluginStates().size(), 1);
	QCOMPARE(service.pluginStates().value("p1"), PluginState::Enabled);
	QCOMPARE(service.pluginState("p1"), PluginState::Enabled);
	QCOMPARE(service.enabledPlugins(), {"p1"});

	service.setPluginState("p1", PluginState::Disabled);

	QCOMPARE(service.pluginStates().size(), 1);
	QCOMPARE(service.pluginStates().value("p1"), PluginState::Disabled);
	QCOMPARE(service.pluginState("p1"), PluginState::Disabled);
	QCOMPARE(service.enabledPlugins(), {});
}

QTEST_APPLESS_MAIN(tst_PluginStateService)
#include "tst-plugin-state-service.moc"
