/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "execution-arguments-parser.h"

#include "execution-arguments/execution-arguments.h"

#include <cstdio>

ExecutionArguments ExecutionArgumentsParser::parse(const QStringList &arguments) const
{
    auto queryVersion = false;
    auto queryUsage = false;
    auto debugMask = QString{};
    auto profileDirectory = QString{};
    auto openIds = QStringList{};
    auto openUuid = QString{};

    for (auto it = arguments.constBegin(); it != arguments.constEnd(); ++it)
    {
        if (*it == QStringLiteral("--version"))
            queryVersion = true;
        else if (*it == QStringLiteral("--help"))
            queryUsage = true;
        else if (*it == QStringLiteral("--debug") && (it + 1) != arguments.constEnd())
        {
            bool ok;
            (++it)->toInt(&ok);
            if (ok)
                debugMask = *it;
        }
        else if (*it == QStringLiteral("--config-dir") && (it + 1) != arguments.constEnd())
            profileDirectory = *(++it);
        else if (QRegExp("^[a-zA-Z]+:(/){0,3}.+").exactMatch(*it))
            openIds.append(*it);
        else if (*it == QStringLiteral("--open-uuid") && (it + 1) != arguments.constEnd())
            openUuid = *(++it);
        else
            fprintf(stderr, "Ignoring unknown parameter '%s'\n", it->toUtf8().constData());
    }

    return ExecutionArguments{
        queryVersion, queryUsage, std::move(profileDirectory), std::move(openIds), std::move(openUuid)};
}
