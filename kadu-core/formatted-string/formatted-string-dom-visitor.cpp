/*
 * %kadu copyright begin%
 * Copyright 2012, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string-dom-visitor.h"

FormattedStringDomVisitor::FormattedStringDomVisitor()
{
	Result.appendChild(Result.createElement("message"));
}

FormattedStringDomVisitor::~FormattedStringDomVisitor()
{
}

void FormattedStringDomVisitor::beginVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringDomVisitor::endVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);
}

void FormattedStringDomVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	QString imagePath = formattedStringImageBlock->imagePath();
	ChatImage image = formattedStringImageBlock->image();

	QDomElement img = Result.createElement("img");
	img.setAttribute("name", image.key());
	img.setAttribute("class", "scalable");

	if (QFileInfo(imagePath).isAbsolute())
		img.setAttribute("src", QString("file://%1").arg(imagePath));
	else if (imagePath.startsWith("kaduimg:///"))
		img.setAttribute("src", imagePath);
	else
		img.setAttribute("src", QString("kaduimg:///%1").arg(imagePath));

	Result.documentElement().appendChild(img);
}

QString FormattedStringDomVisitor::style(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	if (!formattedStringTextBlock->bold() && !formattedStringTextBlock->italic() && !formattedStringTextBlock->underline() && !formattedStringTextBlock->color().isValid())
		return QString();

	QString style;
	if (formattedStringTextBlock->bold())
		style += "font-weight:600;";
	if (formattedStringTextBlock->italic())
		style += "font-style:italic;";
	if (formattedStringTextBlock->underline())
		style += "text-decoration:underline;";

//  TODO: Ignore colors settings for now. Many clients send black as default color.
//	This breaks all dark chat themes. We have to find better solution for post 0.9.0 versions
//	if (Color.isValid())
//		style += QString("color:%1;").arg(Color.name());

	return style;
}

void FormattedStringDomVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	QString content(replacedNewLine(formattedStringTextBlock->content(), QLatin1String("\n")));
	QStringList lines = content.split("\n");

	QDomElement span = Result.createElement("span");
	span.setAttribute("style", style(formattedStringTextBlock));

	int size = lines.size();
	for (int i = 0; i < size; i++)
	{
		if (i > 0)
			span.appendChild(Result.createElement("br"));
		span.appendChild(Result.createTextNode(lines.at(i)));
	}

	Result.documentElement().appendChild(span);
}

QDomDocument FormattedStringDomVisitor::result() const
{
	return Result;
}
