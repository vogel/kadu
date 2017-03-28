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

#include "tool-tip-widget.h"

#include "parser/parser.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

ToolTipWidget::ToolTipWidget(const Talkable &talkable, QWidget *parent)
        : QFrame{parent, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint |
                             Qt::WindowStaysOnTopHint | Qt::MSWindowsOwnDC},
          m_talkable{talkable}
{
    setObjectName(QStringLiteral("tool_tip"));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setStyleSheet(
        QStringLiteral("#tool_tip { border: 1px solid %1; }").arg(palette().window().color().darker().name()));

    auto layout = make_owned<QHBoxLayout>(this);
    layout->setMargin(10);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    m_tipLabel = make_owned<QLabel>(this);
    m_tipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_tipLabel->setContentsMargins(10, 10, 10, 10);
    m_tipLabel->setTextFormat(Qt::RichText);
    m_tipLabel->setWordWrap(true);

    layout->addWidget(m_tipLabel);
}

ToolTipWidget::~ToolTipWidget()
{
}

void ToolTipWidget::setParser(Parser *parser)
{
    m_parser = parser;
}

void ToolTipWidget::init()
{
    auto syntax = QStringLiteral(
        QT_TRANSLATE_NOOP(
            "HintManager",
            "<table>"
            "<tr>"
            "<td align=\"left\" valign=\"top\">"
            "<img style=\"max-width:64px; max-height:64px;\" "
            "src=\"{#{avatarPath} #{avatarPath}}{~#{avatarPath} @{kadu_icons/kadu:64x64}}\""
            ">"
            "</td>"
            "<td width=\"100%\">"
            "<div>[<b>%a</b>][&nbsp;<b>(%g)</b>]</div>"
            "[<div><img height=\"16\" width=\"16\" src=\"#{statusIconPath}\">&nbsp;&nbsp;%u</div>]"
            "[<div><img height=\"16\" width=\"16\" src=\"@{phone:16x16}\">&nbsp;&nbsp;%m</div>]"
            "[<div><img height=\"16\" width=\"16\" src=\"@{mail-message-new:16x16}\">&nbsp;&nbsp;%e</div>]"
            "</td>"
            "</tr>"
            "</table>"
            "[<hr><b>%s</b>][<b>:</b><br><small>%d</small>]"));

    auto text = m_parser->parse(syntax, m_talkable, ParserEscape::HtmlEscape);
    while (text.endsWith(QStringLiteral("<br/>")))
        text.resize(text.length() - QString("<br/>").length());
    while (text.startsWith(QStringLiteral("<br/>")))
        text = text.right(text.length() - QString("<br/>").length());

#ifdef Q_OS_UNIX
    text = text.remove("file://");
#endif

    m_tipLabel->setText(text);

    setFixedSize(m_tipLabel->sizeHint() + QSize{2, 2});
}
