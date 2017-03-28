/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"
#include "html/normalized-html-string.h"
#include "icons/kadu-icon.h"
#include "parser/parser-data.h"

#include <QtCore/QVariantMap>

class Parser;

struct KADUAPI Notification final : public ParserData
{
public:
    static void registerParserTags(Parser *parser);
    static void unregisterParserTags(Parser *parser);

    QString type;
    QString title;
    NormalizedHtmlString text;
    NormalizedHtmlString details;
    KaduIcon icon;

    QList<QString> callbacks;
    QString acceptCallback;
    QString discardCallback;

    QVariantMap data;
};

KADUAPI bool operator==(const Notification &x, const Notification &y);
