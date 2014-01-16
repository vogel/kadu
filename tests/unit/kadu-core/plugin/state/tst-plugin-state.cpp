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

#include <QtTest/QtTest>

class tst_PluginState : public QObject
{
	Q_OBJECT

private slots:
	void newTest();
	void enabledTest();
	void disabledTest();

};

void tst_PluginState::newTest()
{
	QCOMPARE(pluginStateToString(PluginState::New), {});
	QCOMPARE(stringToPluginState(""), PluginState::New);
	QCOMPARE(stringToPluginState("New"), PluginState::New);
	QCOMPARE(stringToPluginState("Invalid"), PluginState::New);
}

void tst_PluginState::enabledTest()
{
	QCOMPARE(pluginStateToString(PluginState::Enabled), QString{"Loaded"});
	QCOMPARE(stringToPluginState("Loaded"), PluginState::Enabled);
}

void tst_PluginState::disabledTest()
{
	QCOMPARE(pluginStateToString(PluginState::Disabled), QString{"NotLoaded"});
	QCOMPARE(stringToPluginState("NotLoaded"), PluginState::Disabled);
}

QTEST_APPLESS_MAIN(tst_PluginState)
#include "tst-plugin-state.moc"
