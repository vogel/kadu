/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QKeyEvent>

#include "buddies-line-edit.h"

BuddiesLineEdit::BuddiesLineEdit(QWidget *parent) :
		QLineEdit(parent)
{
}

BuddiesLineEdit::~BuddiesLineEdit()
{
}

void BuddiesLineEdit::keyReleaseEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Up)
		emit previous();
	else if (e->key() == Qt::Key_Down)
		emit next();
	else
		QWidget::keyReleaseEvent(e);
}
