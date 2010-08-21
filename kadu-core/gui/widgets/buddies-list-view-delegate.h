/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef BUDDIES_LIST_VIEW_DELEGATE_H
#define BUDDIES_LIST_VIEW_DELEGATE_H

#include <QtGui/QItemDelegate>

#include "accounts/accounts-aware-object.h"
#include "buddies/buddy.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/buddies-list-view-delegate-configuration.h"
#include "status/status.h"

class QTextDocument;
class QTreeView;

class AbstractBuddiesModel;
class Account;
class BuddiesListViewItemPainter;

class BuddiesListViewDelegate : public QItemDelegate, public ConfigurationAwareObject, public AccountsAwareObject
{
	Q_OBJECT

	AbstractBuddiesModel *Model;
	BuddiesListViewDelegateConfiguration Configuration;

	bool isBold(const QModelIndex &index) const;

	void drawDebugRect(QPainter *painter, QRect rect, QColor color) const;

	QStyleOptionViewItemV4 getOptions(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	QRect buddyIconRect(const BuddiesListViewItemPainter &buddyPainter, const QRect &itemRect) const;
	QRect buddyAvatarRect(const BuddiesListViewItemPainter &buddyPainter) const;
	QRect buddyNameRect(const BuddiesListViewItemPainter &buddyPainter, const QRect &itemRect) const;
	QRect buddyDescriptionRect(const BuddiesListViewItemPainter &buddyPainter) const;

	void paintBuddyIcon(const BuddiesListViewItemPainter &buddyPainter, QPainter *painter, const QRect &itemRect) const;
	void paintMessageIcon(const BuddiesListViewItemPainter &buddyPainter) const;
	void paintBuddyName(const BuddiesListViewItemPainter &buddyPainter, QPainter *painter, const QStyleOptionViewItem &option, const QRect &itemRect) const;
	void paintAccountName(const BuddiesListViewItemPainter &buddyPainter) const;
	void paintBuddyDescription(const BuddiesListViewItemPainter &buddyPainter) const;

private slots:
	void buddyStatusChanged(Contact contact, Status oldStatus);
	void modelDestroyed();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	explicit BuddiesListViewDelegate(QObject *parent = 0);
	virtual ~BuddiesListViewDelegate();

	virtual void setModel(AbstractBuddiesModel *model);

	virtual void setShowAccountName(bool show);

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual void configurationUpdated();

	bool useMessagePixmap(const QModelIndex &index) const;

};

// for MOC
#include "contacts/contact.h"

#endif // BUDDIES_LIST_VIEW_DELEGATE_H
