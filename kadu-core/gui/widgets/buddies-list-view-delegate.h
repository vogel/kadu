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
#include "configuration/configuration-aware-object.h"
#include "buddies/buddy.h"
#include "status/status.h"

class QTextDocument;

class AbstractBuddiesModel;
class Account;

class BuddiesListViewDelegate : public QItemDelegate, public ConfigurationAwareObject, public AccountsAwareObject
{
	Q_OBJECT

	AbstractBuddiesModel *Model;

	QFont Font;
	QFont BoldFont;
	QFont DescriptionFont;

	bool AlignTop;
	bool ShowAccountName;
	bool ShowBold;
	bool ShowDescription;
	bool ShowMultiLineDescription;
	QColor DescriptionColor;

	QSize DefaultAvatarSize;

	QPixmap MessagePixmap;

	QTextDocument * descriptionDocument(const QString &text, int width, QColor color) const;
	bool isBold(const QModelIndex &index) const;
	QPixmap avatar(const QModelIndex &index) const;

	bool useMessagePixmap(const QModelIndex &index) const;
	int iconsWidth(const QModelIndex &index, int margin) const;

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

	virtual void setShowAccountName(bool show) { ShowAccountName = show; }

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual void configurationUpdated();
};

// for MOC
#include "contacts/contact.h"

#endif // BUDDIES_LIST_VIEW_DELEGATE_H
