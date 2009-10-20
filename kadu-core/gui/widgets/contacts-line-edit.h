/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_LINE_EDIT_H
#define CONTACTS_LINE_EDIT_H

#include <QtGui/QLineEdit>

class ContactsLineEdit : public QLineEdit
{
	Q_OBJECT

protected:
	virtual void keyReleaseEvent(QKeyEvent *e);

public:
	explicit ContactsLineEdit(QWidget *parent = 0);
	virtual ~ContactsLineEdit();

signals:
	void next();
	void previous();

};

#endif // CONTACTS_LINE_EDIT_H
