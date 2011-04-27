/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exec_notify.h"

#include "exec-notify-plugin.h"

ExecNotifyPlugin::~ExecNotifyPlugin()
{
}

int ExecNotifyPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	ExecNotifyInstance = new ExecNotify(this);

	return 0;
}

void ExecNotifyPlugin::done()
{
}

Q_EXPORT_PLUGIN2(exec_notify, ExecNotifyPlugin)
