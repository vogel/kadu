/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

enum class QMqttConnectFlag : uint8_t {
	Clr = 1 << 1,
	Wil = 1 << 2,
	Ret = 1 << 5,
	Pass = 1 << 6,
	User = 1 << 7,
	QoS0 = 0 << 3,
	QoS1 = 1 << 3,
	QoS2 = 2 << 3
};

enum class QMqttMessageFlag : uint8_t {
	Ret = 1 << 0,
	Dup = 1 << 3,
	QoS0 = 0 << 1,
	QoS1 = 1 << 1,
	QoS2 = 2 << 1
};
