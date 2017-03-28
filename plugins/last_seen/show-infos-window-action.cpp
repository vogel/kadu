/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "show-infos-window-action.h"

#include "infos.h"

ShowInfosWindowAction::ShowInfosWindowAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setName(QStringLiteral("lastSeenAction"));
    setText(tr("&Show infos about buddies"));
    setType(ActionDescription::TypeMainMenu);
}

ShowInfosWindowAction::~ShowInfosWindowAction()
{
}

void ShowInfosWindowAction::setInfos(Infos *infos)
{
    m_infos = infos;
}

void ShowInfosWindowAction::actionTriggered(QAction *, bool)
{
    m_infos->onShowInfos();
}

#include "moc_show-infos-window-action.cpp"
