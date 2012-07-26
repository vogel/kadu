/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtGui/QTextDocument>

#include "formatted-string/formatted-string-html-visitor.h"
#include "formatted-string/formatted-string-image-block.h"
#include "misc/misc.h"

#include "formatted-string-part.h"

FormattedStringHtmlVisitor::FormattedStringHtmlVisitor()
{
}

FormattedStringHtmlVisitor::~FormattedStringHtmlVisitor()
{
}

void FormattedStringHtmlVisitor::visit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringHtmlVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	QString imagePath = formattedStringImageBlock->imagePath();
	ChatImageKey imageKey = formattedStringImageBlock->imageKey();
	Result.append(QFileInfo(imagePath).isAbsolute()
			? QString("<img src=\"file://%1\" id=\"%2\" />").arg(imagePath).arg(imageKey.toString())
			: QString("<img src=\"kaduimg:///%1\" id=\"%2\" />").arg(imagePath).arg(imageKey.toString()));
}

void FormattedStringHtmlVisitor::visit(const FormattedStringPart * const formattedStringPart)
{
	QString content(replacedNewLine(Qt::escape(formattedStringPart->content()), QLatin1String("<br/>")));
	content.replace(QChar::LineSeparator, QLatin1String("<br/>"));

	if (!formattedStringPart->bold() && !formattedStringPart->italic() && !formattedStringPart->underline() && !formattedStringPart->color().isValid())
	{
		Result.append(content);
		return;
	}

	QString span = "<span style=\"";
	if (formattedStringPart->bold())
		span += "font-weight:600;";
	if (formattedStringPart->italic())
		span += "font-style:italic;";
	if (formattedStringPart->underline())
		span += "text-decoration:underline;";

//  TODO: Ignore colors settings for now. Many clients send black as default color.
//	This breaks all dark chat themes. We have to find better solution for post 0.9.0 versions
//	if (Color.isValid())
//		span += QString("color:%1;").arg(Color.name());

	span += "\">";

	Result.append(span + content + "</span>");
}

QString FormattedStringHtmlVisitor::result() const
{
	return Result;
}
