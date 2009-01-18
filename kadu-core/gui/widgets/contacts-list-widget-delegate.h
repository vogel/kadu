/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_LIST_WIDGET_DELEGATE_H
#define CONTACTS_LIST_WIDGET_DELEGATE_H

#include <QtGui/QItemDelegate>

#include "contacts/contact.h"
#include "protocols/status.h"

#include "configuration_aware_object.h"

class QTextDocument;

class AbstractContactsModel;
class Account;

class ContactsListWidgetDelegate : public QItemDelegate, public ConfigurationAwareObject
{
	Q_OBJECT

	AbstractContactsModel *Model;

	QFont Font;
	QFont DescriptionFont;

	bool AlignTop;
	bool ShowBold;
	bool ShowDescription;
	bool ShowMultiLineDescription;
	QColor DescriptionColor;

	QTextDocument * descriptionDocument(const QString &text, int width) const;

	bool isBold(Contact contact) const;
	QString displayDescription(Contact contact) const;

private slots:
	void accountRegistered(Account *account);
	void accountUnregistered(Account *account);

	void contactStatusChanged(Account *account, Contact contact, Status oldStatus);

public:
	ContactsListWidgetDelegate(AbstractContactsModel *model, QObject *parent = 0);
	virtual ~ContactsListWidgetDelegate();

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual void configurationUpdated();
};

#endif // CONTACTS_LIST_WIDGET_DELEGATE_H
