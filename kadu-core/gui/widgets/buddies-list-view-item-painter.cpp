/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTreeView>

#include "gui/widgets/buddies-list-view-delegate.h"
#include "model/roles.h"

#include "buddies-list-view-item-painter.h"

BuddiesListViewItemPainter::BuddiesListViewItemPainter(const BuddiesListViewDelegate *delegate, const QStyleOptionViewItemV4 &option, const QModelIndex &index) :
		Delegate(delegate), Option(option), Index(index)
{
	Widget = dynamic_cast<const QTreeView *>(option.widget);
}

QSize BuddiesListViewItemPainter::sizeHint()
{
	if (!Widget)
		return QSize(0, 0);

	return QSize(Widget->width(), buddyHeight());
}

QTextDocument * BuddiesListViewItemPainter::descriptionDocument(const QString &text, int width, QColor color) const
{
	QString description = Qt::escape(text);
	description.replace("\n", Delegate->showMultiLineDescription() ? "<br/>" : " " );

	QTextDocument *doc = new QTextDocument();

	doc->setDefaultFont(Delegate->descriptionFont());
	if (Delegate->descriptionColor().isValid())
		doc->setDefaultStyleSheet(QString("* { color: %1; }").arg(color.name()));

	doc->setHtml(QString("<span>%1</span>").arg(description));

	QTextOption opt = doc->defaultTextOption();
	opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	doc->setDefaultTextOption(opt);

	QTextFrameFormat frameFormat = doc->rootFrame()->frameFormat();
	frameFormat.setMargin(0);
	doc->rootFrame()->setFrameFormat(frameFormat);

	doc->setTextWidth(width);
	return doc;
}

int BuddiesListViewItemPainter::iconsWidth(int margin) const
{
	QPixmap pixmap = qvariant_cast<QPixmap>(Index.data(Qt::DecorationRole));

	int result = 0;
	if (!pixmap.isNull())
		result += pixmap.width() + margin;
	if (Delegate->useMessagePixmap(Index))
		result += Delegate->messagePixmap().width() + margin;

	return result;
}

int BuddiesListViewItemPainter::textAvailableWidth(const QTreeView *widget) const
{
	int avatarSize = Delegate->showAvatars() ? Delegate->defaultAvatarSize().width() + 4 : 0;

	int indentation = Index.parent().isValid()
		? widget->indentation()
		: 0;

	QStyle *style = widget ? widget->style() : QApplication::style();
	const int hFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget);

	int textLeft = hFrameMargin + iconsWidth(hFrameMargin);
	int neededSpace = indentation + textLeft + hFrameMargin + avatarSize;
	return widget->columnWidth(0) - neededSpace;
}

QPixmap BuddiesListViewItemPainter::buddyAvatar() const
{
	QVariant avatar = Index.data(AvatarRole);
	if (!avatar.canConvert<QPixmap>())
		return QPixmap();

	return avatar.value<QPixmap>();
}

QPixmap BuddiesListViewItemPainter::buddyIcon() const
{
	return qvariant_cast<QPixmap>(Index.data(Qt::DecorationRole));
}

int BuddiesListViewItemPainter::buddyIconHeight() const
{
	return buddyIcon().height();
}

int BuddiesListViewItemPainter::buddyAvatarHeight() const
{
	if (!Delegate->showAvatars())
		return 0;

	if (buddyAvatar().isNull())
		return 0;

	return Delegate->defaultAvatarSize().height();
}

int BuddiesListViewItemPainter::buddyNameHeight() const
{
	QFontMetrics fontMetrics(Delegate->font());
	return fontMetrics.lineSpacing();
}

int BuddiesListViewItemPainter::buddyDescriptionHeight(int availableWidth) const
{
	if (!Delegate->showDescription())
		return 0;

	QString description = Index.data(DescriptionRole).toString();
	if (description.isEmpty())
		return 0;

	QTextDocument *dd = descriptionDocument(description, availableWidth, Delegate->descriptionColor());
	int descriptionHeight = (int)dd->size().height();
	delete dd;

	return descriptionHeight;
}

int BuddiesListViewItemPainter::buddyHeight() const
{
	QStyle *style = Widget ? Widget->style() : QApplication::style();
	const int vFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, Widget);

	int iconHeight = buddyIconHeight();
	int displayHeight = buddyNameHeight();
	int descriptionHeight = buddyDescriptionHeight(textAvailableWidth(Widget));
	int avatarHeight = buddyAvatarHeight();

	int textHeight = displayHeight;
	if (descriptionHeight > 0)
	{
		textHeight += 3; // margin
		textHeight += descriptionHeight;
	}

	return qMax(qMax(iconHeight, displayHeight + descriptionHeight), avatarHeight) + 2 * vFrameMargin;
}
