/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtCore/QAbstractItemModel>
#include <QtGui/QApplication>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QIcon>
#include <QtGui/QLayout>
#include <QtGui/QListView>
#include <QtGui/QPainter>
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
#include <QtGui/QPixmapCache>
#endif
#include <QtGui/QStyleOption>
#include <QtGui/QTextDocument>
#include <QtGui/QTextFrame>
#include <QtGui/QTextFrameFormat>
#include <QtGui/QTextOption>
#include <QtGui/QTreeView>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/model/buddies-model.h"
#include "chat/message/pending-messages-manager.h"
#include "contacts/contact.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/buddies-list-view-item-painter.h"
#include "model/roles.h"
#include "debug.h"
#include "icons-manager.h"

#include "buddies-list-view-delegate.h"

#undef KDEBUG_VISUAL
#define KDEBUG_VISUAL 0

BuddiesListViewDelegate::BuddiesListViewDelegate(QObject *parent) :
		QItemDelegate(parent), Model(0), ShowAccountName(true)
{
	triggerAllAccountsRegistered();
	configurationUpdated();

	DefaultAvatarSize = IconsManager::instance()->pixmapByPath("32x32/system-users.png").size();
	MessagePixmap = IconsManager::instance()->pixmapByPath("protocols/common/16x16/message.png");
}

BuddiesListViewDelegate::~BuddiesListViewDelegate()
{
	triggerAllAccountsUnregistered();
}

void BuddiesListViewDelegate::setModel(AbstractBuddiesModel *model)
{
	Model = model;
	QAbstractItemModel *itemModel = dynamic_cast<QAbstractItemModel *>(Model);
	if (itemModel)
		connect(itemModel, SIGNAL(destroyed(QObject *)), this, SLOT(modelDestroyed()));
}

void BuddiesListViewDelegate::accountRegistered(Account account)
{
	connect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(buddyStatusChanged(Contact, Status)));
}

void BuddiesListViewDelegate::accountUnregistered(Account account)
{
	disconnect(account.data(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(buddyStatusChanged(Contact, Status)));
}

void BuddiesListViewDelegate::buddyStatusChanged(Contact contact, Status oldStatus)
{
	Q_UNUSED(oldStatus)

	if (Model)
		emit sizeHintChanged(Model->buddyIndex(contact.ownerBuddy()));
}

void BuddiesListViewDelegate::modelDestroyed()
{
	Model = 0;
}

bool BuddiesListViewDelegate::useMessagePixmap(const QModelIndex &index) const
{
	if (index.parent().isValid()) // contact
	{
		Contact contact = qvariant_cast<Contact>(index.data(ContactRole));
		return contact && PendingMessagesManager::instance()->hasPendingMessagesForContact(contact);
	}
	else
	{
		Buddy buddy = qvariant_cast<Buddy>(index.data(BuddyRole));
		return buddy && PendingMessagesManager::instance()->hasPendingMessagesForBuddy(buddy);
	}
}

void BuddiesListViewDelegate::drawDebugRect(QPainter *painter, QRect rect, QColor color) const
{
	Q_UNUSED(rect)

	painter->save();
	painter->setPen(color);
	painter->drawRect(rect);
	painter->restore();
}

QStyleOptionViewItemV4 BuddiesListViewDelegate::getOptions(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = setOptions(index, option);

	const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
	opt.features = v2
		? v2->features
		: QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	opt.locale = v3 ? v3->locale : QLocale();
	opt.widget = v3 ? v3->widget : 0;

	return opt;
}

QSize BuddiesListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	BuddiesListViewItemPainter buddyPainter(this, getOptions(option, index), index);
	return buddyPainter.sizeHint();
}

QRect BuddiesListViewDelegate::buddyIconRect(const BuddiesListViewItemPainter &buddyPainter, const QRect &itemRect) const
{
	QRect result(0, 0, 0, 0);

	QPixmap icon = buddyPainter.buddyIcon();
	if (icon.isNull())
		return result;

	result.setWidth(icon.width());
	result.setHeight(icon.height());

	QPoint topLeft = itemRect.topLeft();

	if (!AlignTop)
		topLeft.setY(topLeft.y() + (itemRect.height() - icon.height()) / 2);

	result.moveTo(topLeft);

	return result;
}

QRect BuddiesListViewDelegate::buddyNameRect(const BuddiesListViewItemPainter &buddyPainter, const QRect &itemRect) const
{
	Q_UNUSED(buddyPainter)
	Q_UNUSED(itemRect)

	return QRect(0, 0, 0, 0);
}

void BuddiesListViewDelegate::paintBuddyIcon(const BuddiesListViewItemPainter &buddyPainter, QPainter *painter, const QRect &itemRect) const
{
	QPixmap icon = buddyPainter.buddyIcon();
	if (icon.isNull())
		return;

	QRect iconRect = buddyIconRect(buddyPainter, itemRect);
	painter->drawPixmap(iconRect, icon);
}

void BuddiesListViewDelegate::paintBuddyName(const BuddiesListViewItemPainter &buddyPainter, QPainter *painter, const QStyleOptionViewItem &option, const QRect &itemRect) const
{
// 	bool bold = isBold(index);
// 	QFont font = bold ? BoldFont : Font;
// 
// 	QColor textcolor = option.palette.color(QPalette::Normal, option.state & QStyle::State_Selected
// 		? QPalette::HighlightedText
// 		: QPalette::Text);
// 
// 	painter->setFont(bold);
// 	painter->setPen(textcolor);
// 
// 	QFontMetrics fontMetrics(font);
	Q_UNUSED(option)
	drawDebugRect(painter, buddyNameRect(buddyPainter, itemRect), QColor(0, 0, 255));
}

void BuddiesListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = setOptions(index, option);

	const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
	opt.features = v2
		? v2->features
		: QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	opt.locale = v3 ? v3->locale : QLocale();
	opt.widget = v3 ? v3->widget : 0;
	opt.showDecorationSelected = true;

	BuddiesListViewItemPainter buddyPainter(this, getOptions(option, index), index);

	int avatarSize = ShowAvatars ? DefaultAvatarSize.width() + 4 : 0;

	const QAbstractItemView *widget = dynamic_cast<const QAbstractItemView *>(opt.widget);
	if (!widget)
		return;

	QStyle *style = widget->style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

	const int hFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget);
	const int vFrameMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, 0, widget);

	QRect rect = opt.rect;
	rect.adjust(hFrameMargin, vFrameMargin, -hFrameMargin, -vFrameMargin);
	drawDebugRect(painter, rect, QColor(255, 0, 0));

	paintBuddyIcon(buddyPainter, painter, rect);
	paintBuddyName(buddyPainter, painter, option, rect);

	painter->save();
	painter->setClipRect(rect);
	painter->translate(rect.topLeft());

	QColor textcolor = option.palette.color(QPalette::Normal, option.state & QStyle::State_Selected
			? QPalette::HighlightedText
			: QPalette::Text);

	painter->setFont(Font);
	painter->setPen(textcolor);

	bool bold = isBold(index);
	QFontMetrics fontMetrics(bold ? BoldFont : Font);
	QFontMetrics descriptionFontMetrics(DescriptionFont);

	int displayHeight = fontMetrics.lineSpacing() + 3;

	QString description = ShowDescription ? index.data(DescriptionRole).toString() : QString::null;
	bool hasDescription = !description.isEmpty();

	QTextDocument *dd = 0;
	int descriptionHeight = 0;
	int textLeft = hFrameMargin + buddyPainter.iconsWidth(hFrameMargin);
	int textWidth = rect.width() - textLeft - hFrameMargin - avatarSize;

	if (hasDescription)
	{
		dd = buddyPainter.descriptionDocument(description, textWidth,
			option.state & QStyle::State_Selected
					? textcolor
					: DescriptionColor);

		descriptionHeight = (int)dd->size().height();
	}

	int itemHeight = AlignTop ? displayHeight : rect.height();

	int pixmapMargin = 0;


	if (useMessagePixmap(index))
		painter->drawPixmap(hFrameMargin + pixmapMargin, vFrameMargin + (itemHeight - MessagePixmap.height()) / 2, MessagePixmap);

	QString display = index.data(Qt::DisplayRole).toString();
	if (display.isEmpty())
	{
		painter->restore();
		if (dd)
			delete dd;
		return;
	}

	if (bold)
		painter->setFont(BoldFont);

		// TODO: 0.6.6
/*
		if (User.protocolData("Gadu", "Blocking").toBool())
			painter->setPen(QColor(255, 0, 0));
		else if (IgnoredManager::isIgnored(UserListElements(users)))
			painter->setPen(QColor(192, 192, 0));
		else if (config_file.readBoolEntry("General", "PrivateStatus") && User.protocolData("Gadu", "OfflineTo").toBool())
			painter->setPen(QColor(128, 128, 128));
*/
//		if (User.data("HideDescription").toString() != "true")

	int top = hasDescription
			? fontMetrics.ascent() + 1
			: ((itemHeight - fontMetrics.height()) / 2) + fontMetrics.ascent();

	QPen pen = painter->pen();
	if (option.state & QStyle::State_Selected)
		painter->setPen(textcolor);
	else
		painter->setPen(config_file.readColorEntry("Look", "UserboxFgColor"));

	Account account = qvariant_cast<Account>(index.data(AccountRole));
	QString accountDisplay;
	if (account)
		accountDisplay = account.accountIdentity().name();

	// only display account name when in contact-mode, not buddy-mode
	if ((option.state & QStyle::State_MouseOver && ShowAccountName) || index.parent().isValid())
	{
		int accountDisplayWidth = descriptionFontMetrics.width(accountDisplay);
		int displayWidth = fontMetrics.width(display);

		if (accountDisplayWidth + displayWidth + 16 > textWidth)
		{
			displayWidth = textWidth - accountDisplayWidth - 16;
			display = fontMetrics.elidedText(display, Qt::ElideRight, displayWidth);
		}
		else if (displayWidth > textWidth)
			display = fontMetrics.elidedText(display, Qt::ElideRight, textWidth);

		painter->drawText(textLeft, vFrameMargin, textWidth, displayHeight, Qt::AlignLeft | Qt::AlignTop, display);

		painter->setFont(DescriptionFont);
		painter->drawText(textLeft, vFrameMargin, textWidth, displayHeight, Qt::AlignRight | Qt::AlignVCenter, accountDisplay);
		painter->setFont(Font);
	}
	else
	{
		int displayWidth = fontMetrics.width(display);
		if (displayWidth > textWidth)
			display = fontMetrics.elidedText(display, Qt::ElideRight, textWidth);

		painter->drawText(textLeft, vFrameMargin, displayWidth, displayHeight, Qt::AlignLeft | Qt::AlignTop, display);
	}

#ifdef DEBUG_ENABLED
	if (debug_mask & KDEBUG_VISUAL)
		drawDebugRect(painter, QRect(textLeft, 0, textWidth, displayHeight), QColor(255, 0, 0));
#endif

	painter->setPen(pen);

	if (isBold(index))
		painter->setFont(Font);

	if (ShowAvatars && DefaultAvatarSize.isValid())
	{
		QPixmap displayAvatar = buddyPainter.buddyAvatar();
		if (!displayAvatar.isNull())
		{
			bool doGreyOut = AvatarGreyOut && qvariant_cast<Contact>(index.data(ContactRole)).currentStatus().isDisconnected();
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
			QString key = QString("msi-%1-%2,%3,%4")
					.arg(displayAvatar.cacheKey())
					.arg(doGreyOut)
					.arg(AvatarBorder)
					.arg(option.state & QStyle::State_Selected ? 1 : 0);
			QPixmap cached;
			if (QPixmapCache::find(key, &cached))
			{
				//kdebugm(KDEBUG_INFO, "Found key (%s)\n", qPrintable(key));
				//cached! draw and we're done
				int width = widget->viewport()->width() - opt.rect.left()
						- (cached.width() + (avatarSize - cached.width()) / 2);
				painter->drawPixmap(width - 2, vFrameMargin, cached);
			}
			else
			{
				if (displayAvatar.size() != DefaultAvatarSize)
					displayAvatar = displayAvatar.scaled(DefaultAvatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

				int width = widget->viewport()->width() - opt.rect.left()
						- (displayAvatar.width() + (avatarSize - displayAvatar.width()) / 2);

				// draw into cache
				QPainter p;
				p.begin(&displayAvatar);

				// grey out offline contacts' avatar
				if (doGreyOut)
					p.drawPixmap(0, vFrameMargin, QIcon(displayAvatar).pixmap(displayAvatar.size(), QIcon::Disabled));

				// draw avatar border
				if (AvatarBorder)
					p.drawRect(QRect(0, vFrameMargin, displayAvatar.width() - 1, displayAvatar.height() - 1));
#ifdef DEBUG_ENABLED
				if (debug_mask & KDEBUG_VISUAL)
					drawDebugRect(&p, QRect(0, 0, displayAvatar.width() - 1, displayAvatar.height() - 1), QColor(0, 255, 0));
#endif
				p.end();

				// draw to screen
				painter->drawPixmap(width - 2, vFrameMargin + 2, displayAvatar);
				QPixmapCache::insert(key, displayAvatar);
			}
#else
			if (displayAvatar.size() != DefaultAvatarSize)
				displayAvatar = displayAvatar.scaled(DefaultAvatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

			int width = widget->viewport()->width() - opt.rect.left()
					- (displayAvatar.width() + (avatarSize - displayAvatar.width()) / 2);

			// grey out offline contacts' avatar
			if (doGreyOut)
				painter->drawPixmap(width - 2, vFrameMargin + 2, QIcon(displayAvatar).pixmap(displayAvatar.size(), QIcon::Disabled));
			else
				painter->drawPixmap(width - 2, vFrameMargin + 2, displayAvatar);

			// draw avatar border
			if (AvatarBorder)
				painter->drawRect(QRect(width - 2, vFrameMargin + 2, displayAvatar.width(), displayAvatar.height()));
#ifdef DEBUG_ENABLED
			if (debug_mask & KDEBUG_VISUAL)
				drawDebugRect(painter, QRect(width - 2, vFrameMargin + 2, displayAvatar.width(), displayAvatar.height()), QColor(0, 255, 0));
#endif
#endif
		}
	}

	if (!hasDescription)
	{
		painter->restore();
		// don't need to delete descriptionDocument here, it is for sure NULL
		return;
	}

	top += vFrameMargin;

	painter->setFont(DescriptionFont);
	painter->translate(textLeft, top);

#ifdef DEBUG_ENABLED
	if (debug_mask & KDEBUG_VISUAL)
		drawDebugRect(painter, QRect(0, 0, dd->textWidth(), rect.height()), QColor(0, 0, 255));
#endif

	dd->drawContents(painter);
	delete dd;

	painter->restore();
}

bool BuddiesListViewDelegate::isBold(const QModelIndex &index) const
{
	if (!ShowBold)
		return false;

	QVariant statVariant = index.data(StatusRole);
	if (!statVariant.canConvert<Status>())
		return false;

	Status status = statVariant.value<Status>();
	return !status.isDisconnected();
}

void BuddiesListViewDelegate::configurationUpdated()
{
	Font = config_file.readFontEntry("Look", "UserboxFont");
	BoldFont = Font;
	BoldFont.setBold(true);
	
	ShowAccountName = !config_file.readBoolEntry("General", "SimpleMode", true);

	DescriptionFont = Font;
	DescriptionFont.setPointSize(Font.pointSize() - 2);

	ShowAvatars = config_file.readBoolEntry("Look", "ShowAvatars");
	AvatarBorder = config_file.readBoolEntry("Look", "AvatarBorder");
	AvatarGreyOut = config_file.readBoolEntry("Look", "AvatarGreyOut");
	AlignTop = config_file.readBoolEntry("Look", "AlignUserboxIconsTop");
	ShowBold = config_file.readBoolEntry("Look", "ShowBold");
	ShowDescription = config_file.readBoolEntry("Look", "ShowDesc");
	ShowMultiLineDescription = config_file.readBoolEntry("Look", "ShowMultilineDesc");
	DescriptionColor = config_file.readColorEntry("Look", "DescriptionColor");

	QListView *listView = dynamic_cast<QListView *>(parent());
	if (!listView)
		return;

	// hack to make listViee redo the layout
	listView->setSpacing(listView->spacing());
}
