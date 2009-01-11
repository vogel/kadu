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

#include <QtGui/QStyledItemDelegate>

#include "configuration_aware_object.h"

class Contact;

class ContactsListWidgetDelegate : public QStyledItemDelegate, public ConfigurationAwareObject
{
	QFontMetrics *DescriptionFontMetrics;

	Contact contact(const QModelIndex &index) const;

public:
	ContactsListWidgetDelegate(QObject *parent = 0);

	virtual void configurationUpdated();

};

#endif // CONTACTS_LIST_WIDGET_DELEGATE_H
