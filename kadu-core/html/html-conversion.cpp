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

#include "html-conversion.h"

#include "dom/dom-processor.h"
#include "html/html-string.h"
#include "html/normalized-html-string.h"
#include "misc/remove-script-dom-visitor.h"

#include <QtCore/QRegularExpression>
#include <QtGui/QTextDocument>

namespace
{
QString htmlToPlain(const QString &html)
{
    QTextDocument textDocument;
    textDocument.setHtml(html);
    return textDocument.toPlainText();
}
}

QString htmlToPlain(const HtmlString &html)
{
    return htmlToPlain(html.string());
}

QString htmlToPlain(const NormalizedHtmlString &html)
{
    return htmlToPlain(html.string());
}

HtmlString plainToHtml(const QString &plain)
{
    return HtmlString{plain.toHtmlEscaped().replace('\n', "<br />")};
}

NormalizedHtmlString normalizeHtml(const HtmlString &html)
{
    static auto imageRegExp =
        QRegularExpression{R"rx(<img name="([a-z0-9]*)">)rx", QRegularExpression::CaseInsensitiveOption};
    auto fixedHtml = QString{html.string()}.replace(imageRegExp, R"(<img src="\1"/>)").replace("<br>", "<br/>");

    try
    {
        return NormalizedHtmlString{processDom(fixedHtml, RemoveScriptDomVisitor{})};
    }
    catch (invalid_xml &)
    {
        // secure it by applying another html escape
        // at least it won't break anything in webview
        return NormalizedHtmlString{plainToHtml(fixedHtml).string()};
    }
}
