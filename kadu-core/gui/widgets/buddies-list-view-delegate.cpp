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
#include <QtGui/QHeaderView>
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
		QItemDelegate(parent), Model(0), Configuration(parent)
{
	triggerAllAccountsRegistered();
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

void BuddiesListViewDelegate::setShowAccountName(bool showAccountName)
{
	Configuration.setShowAccountName(showAccountName);
}

QStyleOptionViewItemV4 BuddiesListViewDelegate::getOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const
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
	QSize baseSizeHint = QItemDelegate::sizeHint(option, index);

	BuddiesListViewItemPainter buddyPainter(Configuration, getOptions(index, option), index);
	return QSize(0, buddyPainter.height());
}

void BuddiesListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 options = getOptions(index, option);

	const QAbstractItemView *widget = dynamic_cast<const QAbstractItemView *>(options.widget);
	if (!widget)
		return;

	QStyle *style = widget->style();
	style->drawControl(QStyle::CE_ItemViewItem, &options, painter, widget);

	BuddiesListViewItemPainter buddyPainter(Configuration, options, index);
	buddyPainter.paint(painter);
/*
		// TODO: 0.6.6
/ *
		if (User.protocolData("Gadu", "Blocking").toBool())
			painter->setPen(QColor(255, 0, 0));
		else if (IgnoredManager::isIgnored(UserListElements(users)))
			painter->setPen(QColor(192, 192, 0));
		else if (config_file.readBoolEntry("General", "PrivateStatus") && User.protocolData("Gadu", "OfflineTo").toBool())
			painter->setPen(QColor(128, 128, 128));
* /
//		if (User.data("HideDescription").toString() != "true")

	if (Configuration.showAvatars() && Configuration.defaultAvatarSize().isValid())
	{
		QPixmap displayAvatar = buddyPainter.buddyAvatar();
		if (!displayAvatar.isNull())
		{
			bool doGreyOut = Configuration.avatarGreyOut() && qvariant_cast<Contact>(index.data(ContactRole)).currentStatus().isDisconnected();
#if QT_VERSION >= QT_VERSION_CHECK(4,6,0)
			QString key = QString("msi-%1-%2,%3,%4")
					.arg(displayAvatar.cacheKey())
					.arg(doGreyOut)
					.arg(Configuration.avatarBorder())
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
				if (displayAvatar.size() != Configuration.defaultAvatarSize())
					displayAvatar = displayAvatar.scaled(Configuration.defaultAvatarSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

				int width = widget->viewport()->width() - opt.rect.left()
						- (displayAvatar.width() + (avatarSize - displayAvatar.width()) / 2);

				// draw into cache
				QPainter p;
				p.begin(&displayAvatar);

				// grey out offline contacts' avatar
				if (doGreyOut)
					p.drawPixmap(0, vFrameMargin, QIcon(displayAvatar).pixmap(displayAvatar.size(), QIcon::Disabled));

				// draw avatar border
				if (Configuration.avatarBorder())
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

*/
}
