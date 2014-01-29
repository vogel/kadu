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
#include "misc/misc.h"

#include "formatted-string-clone-visitor.h"

FormattedStringCloneVisitor::FormattedStringCloneVisitor()
{
}

FormattedStringCloneVisitor::~FormattedStringCloneVisitor()
{
	// in case something is left
	qDeleteAll(ItemsStack);
}

void FormattedStringCloneVisitor::cloned(FormattedString *clonedFormattedString)
{
	ItemsStack.push(clonedFormattedString);
}

void FormattedStringCloneVisitor::beginVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);

	ItemsStack.push(0); // mark composite begin
}

void FormattedStringCloneVisitor::endVisit(const CompositeFormattedString * const compositeFormattedString)
{
	Q_UNUSED(compositeFormattedString);

	std::vector<std::unique_ptr<FormattedString>> items;
	while (!ItemsStack.isEmpty())
	{
		FormattedString *item = ItemsStack.pop();
		if (item)
			items.push_back(std::unique_ptr<FormattedString>(item));
		else
			break;
	}
	std::reverse(std::begin(items), std::end(items));

	CompositeFormattedString *cloned = new CompositeFormattedString(std::move(items));

	ItemsStack.push(cloned);
}

void FormattedStringCloneVisitor::visit(const FormattedStringImageBlock * const formattedStringImageBlock)
{
	cloned(formattedStringImageBlock->imageKey().isNull()
			? new FormattedStringImageBlock(formattedStringImageBlock->imagePath())
			: new FormattedStringImageBlock(formattedStringImageBlock->imageKey()));
}

void FormattedStringCloneVisitor::visit(const FormattedStringTextBlock * const formattedStringTextBlock)
{
	cloned(new FormattedStringTextBlock(
		formattedStringTextBlock->content(),
		formattedStringTextBlock->bold(),
		formattedStringTextBlock->italic(),
		formattedStringTextBlock->underline(),
		formattedStringTextBlock->color()
	));
}

FormattedString * FormattedStringCloneVisitor::result()
{
	Q_ASSERT(ItemsStack.size() == 1);

	return ItemsStack.pop();
}
