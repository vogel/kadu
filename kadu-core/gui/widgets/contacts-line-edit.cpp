/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QKeyEvent>

#include "contacts-line-edit.h"

ContactsLineEdit::ContactsLineEdit(QWidget *parent) :
		QLineEdit(parent)
{
}

ContactsLineEdit::~ContactsLineEdit()
{
}

void ContactsLineEdit::keyReleaseEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Up)
		emit previous();
	else if (e->key() == Qt::Key_Down)
		emit next();
	else
		QWidget::keyReleaseEvent(e);
}
