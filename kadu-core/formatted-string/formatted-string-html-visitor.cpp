/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-text-block.h"
#include "misc/misc.h"

#include "formatted-string-html-visitor.h"

FormattedStringHtmlVisitor::FormattedStringHtmlVisitor()
{
}

FormattedStringHtmlVisitor::~FormattedStringHtmlVisitor()
{
}

void FormattedStringHtmlVisitor::beginVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringHtmlVisitor::endVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringHtmlVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	auto imagePath = formattedStringImageBlock->imagePath();
	auto escapedImagePath = Qt::escape(imagePath);
	auto imageKey = formattedStringImageBlock->image().key();
	auto escapedImageKey = imageKey;

	append(QFileInfo(imagePath).isAbsolute()
			? QString("<img class=\"scalable\" src=\"file://%1\" name=\"%2\" />").arg(imagePath).arg(escapedImageKey)
			: imagePath.startsWith("kaduimg:///")
			? QString("<img class=\"scalable\" src=\"%1\" name=\"%2\" />").arg(imagePath).arg(escapedImageKey)
			: QString("<img class=\"scalable\" src=\"kaduimg:///%1\" name=\"%2\" />").arg(imagePath).arg(escapedImageKey));
}

void FormattedStringHtmlVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	QString content(replacedNewLine(Qt::escape(formattedStringTextBlock->content()), QLatin1String("<br/>")));

	if (!formattedStringTextBlock->bold() && !formattedStringTextBlock->italic() && !formattedStringTextBlock->underline() && !formattedStringTextBlock->color().isValid())
	{
		append(content);
		return;
	}

	QString span = "<span style=\"";
	if (formattedStringTextBlock->bold())
		span += "font-weight:600;";
	if (formattedStringTextBlock->italic())
		span += "font-style:italic;";
	if (formattedStringTextBlock->underline())
		span += "text-decoration:underline;";

//  TODO: Ignore colors settings for now. Many clients send black as default color.
//	This breaks all dark chat themes. We have to find better solution for post 0.9.0 versions
//	if (Color.isValid())
//		span += QString("color:%1;").arg(Color.name());

	span += "\">";

	append(span + content + "</span>");
}

void FormattedStringHtmlVisitor::append(QString content)
{
	m_result.append(std::move(content));
}

QString FormattedStringHtmlVisitor::result() const
{
	return m_result;
}
