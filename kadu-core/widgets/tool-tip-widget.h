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

#pragma once

#include "misc/memory.h"
#include "talkable/talkable.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtWidgets/QFrame>
#include <injeqt/injeqt.h>

class Parser;

class QLabel;

class ToolTipWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ToolTipWidget(const Talkable &talkable, QWidget *parent = nullptr);
    virtual ~ToolTipWidget();

private:
    QPointer<Parser> m_parser;

    owned_qptr<QLabel> m_tipLabel;
    Talkable m_talkable;

private slots:
    INJEQT_SET void setParser(Parser *parser);
    INJEQT_INIT void init();
};
