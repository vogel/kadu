/*
 * %kadu copyright begin%
 * Copyright 2009 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 root (root@slack.(none))
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "model/roles.h"
#include "icons-manager.h"

#include "buddies-list-view-delegate.h"

BuddiesListViewDelegate::BuddiesListViewDelegate(QObject *parent)
	: QItemDelegate(parent), Model(0), ShowAccountName(true)
{
	triggerAllAccountsRegistered();
	configurationUpdated();

	DefaultAvatarSize = IconsManager::instance()->loadPixmap("ContactsTab").size();
	MessagePixmap = IconsManager::instance()->loadPixmap("Message");
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
	if (Model)
		emit sizeHintChanged(Model->buddyIndex(contact.ownerBuddy()));
}

void BuddiesListViewDelegate::modelDestroyed()
{
	Model = 0;
}

QTextDocument * BuddiesListViewDelegate::descriptionDocument(const QString &text, int width, QColor color) const
{
	QString description = text;
	description.replace("\n", ShowMultiLineDescription ? "<br/>" : " " );

	QTextDocument *doc = new QTextDocument();

	doc->setDefaultFont(DescriptionFont);
	if (DescriptionColor.isValid())
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

int BuddiesListViewDelegate::iconsWidth(const QModelIndex &index, int margin) const
{
	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));

	int result = 0;
	if (!pixmap.isNull())
		result += pixmap.width() + margin;
	if (useMessagePixmap(index))
		result += MessagePixmap.width() + margin;

	return result;
}

QSize BuddiesListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize size(0, 0);

	QStyleOptionViewItemV4 opt = setOptions(index, option);

	const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(&option);
	opt.features = v2
		? v2->features
		: QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
	const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
	opt.locale = v3 ? v3->locale : QLocale();
	opt.widget = v3 ? v3->widget : 0;

	int avatarSize = DefaultAvatarSize.width() + 4;

	const QTreeView *widget = dynamic_cast<const QTreeView *>(opt.widget);
	if (!widget)
		return size;
	int width = widget->viewport()->width() - avatarSize;
	int indentation = index.parent().isValid()
		? widget->indentation()
		: 0;

	QStyle *style = widget ? widget->style() : QApplication::style();
	const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;

	QFontMetrics fontMetrics(Font);
	int displayHeight = fontMetrics.lineSpacing() + 3;

	QString description = ShowDescription ? index.data(DescriptionRole).toString() : QString::null;
	int descriptionHeight = 0;

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	int textLeft = textMargin + iconsWidth(index, textMargin);

	if (!description.isEmpty())
	{
		int neededSpace = indentation + textLeft + textMargin + avatarSize;
		QTextDocument *dd = descriptionDocument(description, widget->columnWidth(0) - neededSpace, DescriptionColor);
		descriptionHeight = (int)dd->size().height();
		delete dd;
	}

	int pixmapHeight = pixmap.height();
	int height = qMax(qMax(pixmapHeight, displayHeight + descriptionHeight), avatar(index).isNull() ? 0 : avatarSize);

	return QSize(width, height);
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

	int avatarSize = DefaultAvatarSize.width() + 4;

	const QAbstractItemView *widget = dynamic_cast<const QAbstractItemView *>(opt.widget);
	if (!widget)
		return;

	QStyle *style = widget->style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

	const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;

	QRect rect = opt.rect;

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

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	int pixmapHeight = pixmap.height();

	QString description = ShowDescription ? index.data(DescriptionRole).toString() : QString::null;
	bool hasDescription = !description.isEmpty();

	QTextDocument *dd = 0;
	int descriptionHeight = 0;
	int textLeft = textMargin + iconsWidth(index, textMargin);
	int textWidth = rect.width() - textLeft - textMargin - avatarSize;

	if (hasDescription)
	{
		dd = descriptionDocument(description, textWidth,
			option.state & QStyle::State_Selected
			? textcolor
			: DescriptionColor);

		descriptionHeight = (int)dd->size().height();
	}

	int height = qMax(pixmapHeight, displayHeight + descriptionHeight);
	int itemHeight = AlignTop ? displayHeight : rect.height();

	int pixmapMargin = 0;

	if (!pixmap.isNull())
	{
		painter->drawPixmap(textMargin, (itemHeight - pixmap.height()) / 2, pixmap);
		pixmapMargin = pixmap.width() + textMargin;
	}

	if (useMessagePixmap(index))
		painter->drawPixmap(textMargin + pixmapMargin, (itemHeight - MessagePixmap.height()) / 2, MessagePixmap);

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
		accountDisplay = account.name();

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

		painter->drawText(textLeft, 0, textWidth, displayHeight, Qt::AlignLeft | Qt::AlignTop, display);

		painter->setFont(DescriptionFont);
		painter->drawText(textLeft, 0, textWidth, displayHeight, Qt::AlignRight | Qt::AlignVCenter, accountDisplay);
		painter->setFont(Font);
	}
	else
	{
		int displayWidth = fontMetrics.width(display);
		if (displayWidth > textWidth)
			display = fontMetrics.elidedText(display, Qt::ElideRight, textWidth);

		painter->drawText(textLeft, 0, displayWidth, displayHeight, Qt::AlignLeft | Qt::AlignTop, display);
	}

	painter->setPen(pen);

	if (isBold(index))
		painter->setFont(Font);

	QPixmap displayAvatar = avatar(index);
	if (!displayAvatar.isNull())
	{
		if (DefaultAvatarSize.isValid() && displayAvatar.size() != DefaultAvatarSize)
			displayAvatar = displayAvatar.scaled(DefaultAvatarSize, Qt::KeepAspectRatio);

		int avatarWidth = displayAvatar.width();
		int width = widget->viewport()->width() - opt.rect.left() - (avatarWidth + (avatarSize - avatarWidth)/2);
		if (!displayAvatar.isNull())
			painter->drawPixmap(width - 2, 2, displayAvatar);
	}

	if (!hasDescription)
	{
		painter->restore();
		// don't need to delete descriptionDocument here, it is for sure NULL
		return;
	}

	top += 5;

	painter->setFont(DescriptionFont);
	painter->translate(textLeft, top);

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

QPixmap BuddiesListViewDelegate::avatar(const QModelIndex &index) const
{
	QVariant avatar = index.data(AvatarRole);
	if (!avatar.canConvert<QPixmap>())
		return QPixmap();

	return avatar.value<QPixmap>();
}

void BuddiesListViewDelegate::configurationUpdated()
{
	Font = config_file.readFontEntry("Look", "UserboxFont");
	BoldFont = Font;
	BoldFont.setBold(true);

	DescriptionFont = Font;
	DescriptionFont.setPointSize(Font.pointSize() - 2);

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
