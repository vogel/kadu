/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-text-block.h"
#include "misc/memory.h"
#include "misc/misc.h"

#include "formatted-string-clone-visitor.h"

FormattedStringCloneVisitor::FormattedStringCloneVisitor()
{
}

FormattedStringCloneVisitor::~FormattedStringCloneVisitor()
{
}

void FormattedStringCloneVisitor::cloned(std::unique_ptr<FormattedString> &&clonedFormattedString)
{
	ItemsStack.push(std::move(clonedFormattedString));
}

void FormattedStringCloneVisitor::beginVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);

	ItemsStack.push(nullptr); // mark composite begin
}

void FormattedStringCloneVisitor::endVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);

	std::vector<std::unique_ptr<FormattedString>> items;
	while (!ItemsStack.empty())
	{
		auto item = std::move(ItemsStack.top());
		ItemsStack.pop();

		if (item)
			items.push_back(std::move(item));
		else
			break;
	}
	std::reverse(std::begin(items), std::end(items));
	ItemsStack.push(make_unique<CompositeFormattedString>(std::move(items)));
}

void FormattedStringCloneVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	cloned(formattedStringImageBlock->image().isNull()
			? make_unique<FormattedStringImageBlock>(formattedStringImageBlock->imagePath())
			: make_unique<FormattedStringImageBlock>(formattedStringImageBlock->image()));
}

void FormattedStringCloneVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	cloned(make_unique<FormattedStringTextBlock>(
		formattedStringTextBlock->content(),
		formattedStringTextBlock->bold(),
		formattedStringTextBlock->italic(),
		formattedStringTextBlock->underline(),
		formattedStringTextBlock->color()
	));
}

std::unique_ptr<FormattedString> FormattedStringCloneVisitor::result()
{
	Q_ASSERT(ItemsStack.size() == 1);

	auto result = std::move(ItemsStack.top());
	ItemsStack.pop();
	return std::move(result);
}
