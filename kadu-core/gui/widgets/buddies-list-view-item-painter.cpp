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
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTreeView>

#include "buddies/buddy.h"
#include "chat/message/pending-messages-manager.h"
#include "contacts/contact.h"
#include "gui/widgets/buddies-list-view-delegate-configuration.h"
#include "model/roles.h"

#include "buddies-list-view-item-painter.h"

#define MARGIN 3

BuddiesListViewItemPainter::BuddiesListViewItemPainter(const BuddiesListViewDelegateConfiguration &configuration, const QStyleOptionViewItemV4 &option, const QModelIndex &index) :
		Configuration(configuration), Option(option), Index(index),
		FontMetrics(/* bold ? Configuration.boldFont() : */ Configuration.font()),
		DescriptionFontMetrics(Configuration.descriptionFont())
{
	Widget = dynamic_cast<const QTreeView *>(option.widget);

	QStyle *style = Widget->style();

	HFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, Widget);
	VFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, Widget);
}

bool BuddiesListViewItemPainter::showAccountName()
{
	if (Index.parent().isValid())
		return true;

	return Option.state & QStyle::State_MouseOver && Configuration.showAccountName();
}

bool BuddiesListViewItemPainter::showDescription()
{
	if (!Configuration.showDescription())
		return false;

	QString description = Index.data(DescriptionRole).toString();
	return !description.isEmpty();
}

void BuddiesListViewItemPainter::computeIconRect()
{
	IconRect = QRect(0, 0, 0, 0);

	QPixmap icon = buddyIcon();
	if (icon.isNull())
		return;

	QPoint topLeft = ItemRect.topLeft();
	IconRect = icon.rect();

	if (!Configuration.alignTop())
		topLeft.setY(topLeft.y() + (ItemRect.height() - icon.height()) / 2);

	IconRect.moveTo(topLeft);
}

void BuddiesListViewItemPainter::computeMessageIconRect()
{
	MessageIconRect = QRect(0, 0, 0, 0);
	if (!useMessagePixmap())
		return;

	QPixmap icon = Configuration.messagePixmap();

	QPoint topLeft = ItemRect.topLeft();
	MessageIconRect = icon.rect();

	if (!Configuration.alignTop())
		topLeft.setY(topLeft.y() + (ItemRect.height() - icon.height()) / 2);

	if (!IconRect.isEmpty())
		topLeft.setX(IconRect.x() + MARGIN);

	MessageIconRect.moveTo(topLeft);
}

void BuddiesListViewItemPainter::computeAvatarRect()
{
	AvatarRect = QRect(0, 0, 0, 0);
	if (!Configuration.showAvatars())
		return;

	int avatarSize = Configuration.defaultAvatarSize().width() + 4;
	AvatarRect.setWidth(avatarSize);

	if (!buddyAvatar().isNull())
		AvatarRect.setHeight(avatarSize);
	else
		AvatarRect.setHeight(1); // just a placeholder

	AvatarRect.moveRight(ItemRect.right());
	AvatarRect.moveTop(ItemRect.top());
}

void BuddiesListViewItemPainter::computeAccountNameRect()
{
	AccountNameRect = QRect(0, 0, 0, 0);
	if (!showAccountName())
		return;

	Account account = qvariant_cast<Account>(Index.data(AccountRole));
	QString accountDisplay;
	if (account)
		accountDisplay = account.accountIdentity().name();

	int accountDisplayWidth = DescriptionFontMetrics.width(accountDisplay);

	AccountNameRect.setWidth(accountDisplayWidth);
	AccountNameRect.setHeight(DescriptionFontMetrics.height());

	AccountNameRect.moveRight(ItemRect.right() - AvatarRect.width() - MARGIN);
	AccountNameRect.moveTop(ItemRect.top());
}

void BuddiesListViewItemPainter::computeNameRect()
{
	NameRect = QRect(0, 0, 0, 0);

	int left = qMax(IconRect.right(), MessageIconRect.right());
	if (0 != left)
		left += MARGIN;

	int right;
	if (!AccountNameRect.isEmpty())
		right = AccountNameRect.left() - MARGIN;
	else
		right = AvatarRect.left() - MARGIN;

	NameRect.moveTop(ItemRect.top());
	NameRect.setLeft(left);
	NameRect.setRight(right);
	NameRect.setHeight(FontMetrics.height());
}

void BuddiesListViewItemPainter::computeDescriptionRect()
{
	DescriptionRect = QRect(0, 0, 0, 0);

	if (!showDescription())
		return;

	DescriptionRect.setTop(NameRect.bottom() + MARGIN);
	DescriptionRect.setLeft(NameRect.left());
	DescriptionRect.setRight(AvatarRect.left() - MARGIN);

	QColor textcolor = Option.palette.color(QPalette::Normal, Option.state & QStyle::State_Selected
			? QPalette::HighlightedText
			: QPalette::Text);

	QTextDocument *dd = descriptionDocument(Index.data(DescriptionRole).toString(), DescriptionRect.width(),
			Option.state & QStyle::State_Selected
					? textcolor
					: Configuration.descriptionColor());


	DescriptionRect.setHeight((int)dd->size().height());

	// printf("Item rect: %d %d\n", ItemRect.width(), ItemRect.height());
	// printf("Desc: %d %d\n", DescriptionRect.width(), DescriptionRect.height());

	delete dd;
}

void BuddiesListViewItemPainter::computeLayout()
{
	computeIconRect();
	computeMessageIconRect();
	computeAvatarRect();
	computeAccountNameRect();
	computeNameRect();
	computeDescriptionRect();
}

QTextDocument * BuddiesListViewItemPainter::descriptionDocument(const QString &text, int width, QColor color) const
{
	QString description = Qt::escape(text);
	description.replace("\n", Configuration.showMultiLineDescription() ? "<br/>" : " " );

	QTextDocument *doc = new QTextDocument();

	doc->setDefaultFont(Configuration.descriptionFont());
	if (Configuration.descriptionColor().isValid())
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

bool BuddiesListViewItemPainter::useMessagePixmap() const
{
	if (Index.parent().isValid()) // contact
	{
		Contact contact = qvariant_cast<Contact>(Index.data(ContactRole));
		return contact && PendingMessagesManager::instance()->hasPendingMessagesForContact(contact);
	}
	else
	{
		Buddy buddy = qvariant_cast<Buddy>(Index.data(BuddyRole));
		return buddy && PendingMessagesManager::instance()->hasPendingMessagesForBuddy(buddy);
	}
}

int BuddiesListViewItemPainter::iconsWidth(int margin) const
{
	QPixmap pixmap = qvariant_cast<QPixmap>(Index.data(Qt::DecorationRole));

	int result = 0;
	if (!pixmap.isNull())
		result += pixmap.width() + margin;
	if (useMessagePixmap())
		result += Configuration.messagePixmap().width() + margin;

	return result;
}

int BuddiesListViewItemPainter::textAvailableWidth(const QTreeView *widget) const
{
	int avatarSize = Configuration.showAvatars() ? Configuration.defaultAvatarSize().width() + 4 : 0;

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

void BuddiesListViewItemPainter::paintDebugRect(QPainter *painter, QRect rect, QColor color) const
{
	painter->save();
	painter->setPen(color);
	painter->drawRect(rect);
	painter->restore();
}

int BuddiesListViewItemPainter::getItemIndentation()
{
	int level = 0;

	if (Widget->rootIsDecorated())
		level++;

	if (Index.parent().isValid())
		level++;

	return level * Widget->indentation();
}

int BuddiesListViewItemPainter::height()
{
	if (!Widget)
		return 0;

	ItemRect = QRect(0, 0, 0, 20);

	QHeaderView *header = Widget->header();
	if (header)
		ItemRect.setWidth(header->sectionSize(0) - getItemIndentation());

	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	QStyle *style = Widget ? Widget->style() : QApplication::style();
	const int vFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, Widget);

	QRect wholeRect = IconRect;
	wholeRect |= MessageIconRect;
	wholeRect |= AvatarRect;
	wholeRect |= AccountNameRect;
	wholeRect |= NameRect;
	wholeRect |= DescriptionRect;

	return wholeRect.height() + 2 * vFrameMargin;
}

void BuddiesListViewItemPainter::paint(QPainter *painter)
{
	ItemRect = Option.rect;
	ItemRect.adjust(HFrameMargin, VFrameMargin, -HFrameMargin, -VFrameMargin);

	computeLayout();

	paintDebugRect(painter, ItemRect, QColor(255, 0, 0));
	paintDebugRect(painter, IconRect, QColor(0, 255, 0));
	paintDebugRect(painter, MessageIconRect, QColor(255, 255, 0));
	paintDebugRect(painter, AvatarRect, QColor(0, 0, 255));
	paintDebugRect(painter, AccountNameRect, QColor(255, 0, 255));
	paintDebugRect(painter, NameRect, QColor(0, 255, 255));
	paintDebugRect(painter, DescriptionRect, QColor(0, 0, 0));
}
