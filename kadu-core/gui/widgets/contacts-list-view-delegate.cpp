/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
#include "buddies/account-data/contact-account-data.h"
#include "buddies/model/buddies-model.h"
#include "configuration/configuration-file.h"
#include "model/roles.h"
#include "icons-manager.h"

#include "contacts-list-view-delegate.h"

ContactsListViewDelegate::ContactsListViewDelegate(QObject *parent)
	: QItemDelegate(parent), Model(0)
{
	triggerAllAccountsRegistered();
	configurationUpdated();

	DefaultAvatarSize = IconsManager::instance()->loadPixmap("ContactsTab").size();
}

ContactsListViewDelegate::~ContactsListViewDelegate()
{
	triggerAllAccountsUnregistered();
}

void ContactsListViewDelegate::setModel(AbstractBudiesModel *model)
{
	Model = model;
	QAbstractItemModel *itemModel = dynamic_cast<QAbstractItemModel *>(Model);
	if (itemModel)
		connect(itemModel, SIGNAL(destroyed(QObject *)), this, SLOT(modelDestroyed()));
}

void ContactsListViewDelegate::accountRegistered(Account account)
{
	connect(account.data(), SIGNAL(contactStatusChanged(Account, Buddy, Status)),
			this, SLOT(contactStatusChanged(Account, Buddy, Status)));
}

void ContactsListViewDelegate::accountUnregistered(Account account)
{
	disconnect(account.data(), SIGNAL(contactStatusChanged(Account, Buddy, Status)),
			this, SLOT(contactStatusChanged(Account, Buddy, Status)));
}

void ContactsListViewDelegate::contactStatusChanged(Account account, Buddy c, Status oldStatus)
{
	if (Model)
		emit sizeHintChanged(Model->buddyIndex(c));
}

void ContactsListViewDelegate::modelDestroyed()
{
	Model = 0;
}

QTextDocument * ContactsListViewDelegate::descriptionDocument(const QString &text, int width, QColor color) const
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

QSize ContactsListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
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
	int textLeft = pixmap.isNull()
		? textMargin
		: pixmap.width() + textMargin * 2;

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

void ContactsListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

	QFontMetrics fontMetrics(Font);
	int displayHeight = fontMetrics.lineSpacing() + 3;

	QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
	int pixmapHeight = pixmap.height();

	QString description = ShowDescription ? index.data(DescriptionRole).toString() : QString::null;
	bool hasDescription = !description.isEmpty();

	QTextDocument *dd = 0;
	int descriptionHeight = 0;
	int textLeft = pixmap.isNull()
		? textMargin
		: pixmap.width() + 2 * textMargin;

	if (hasDescription)
	{
		dd = descriptionDocument(description, rect.width() - textLeft - textMargin - avatarSize,
			option.state & QStyle::State_Selected
			? textcolor
			: DescriptionColor);

		descriptionHeight = (int)dd->size().height();
	}

	int height = qMax(pixmapHeight, displayHeight + descriptionHeight);
	int itemHeight = AlignTop ? displayHeight : rect.height();

	if (!pixmap.isNull())
		painter->drawPixmap(textMargin, (itemHeight - pixmap.height()) / 2, pixmap);

	QString display = index.data(Qt::DisplayRole).toString();
	if (display.isEmpty())
	{
		painter->restore();
		if (dd)
			delete dd;
		return;
	}

	if (isBold(index))
	{
		QFont bold = QFont(Font);
		bold.setWeight(QFont::Bold);
		painter->setFont(bold);
	}

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

	painter->drawText(textLeft, top, display);
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

bool ContactsListViewDelegate::isBold(const QModelIndex &index) const
{
	if (!ShowBold)
		return false;

	QVariant statVariant = index.data(StatusRole);
	if (!statVariant.canConvert<Status>())
		return false;

	Status status = statVariant.value<Status>();
	return !status.isDisconnected();
}

QPixmap ContactsListViewDelegate::avatar(const QModelIndex &index) const
{
	QVariant avatar = index.data(AvatarRole);
	if (!avatar.canConvert<QPixmap>())
		return QPixmap();

	return avatar.value<QPixmap>();
}

void ContactsListViewDelegate::configurationUpdated()
{
	Font = config_file.readFontEntry("Look", "UserboxFont");
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
