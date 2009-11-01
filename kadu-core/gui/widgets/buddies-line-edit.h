/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDIES_LINE_EDIT_H
#define BUDDIES_LINE_EDIT_H

#include <QtGui/QLineEdit>

class BuddiesLineEdit : public QLineEdit
{
	Q_OBJECT

protected:
	virtual void keyReleaseEvent(QKeyEvent *e);

public:
	explicit BuddiesLineEdit(QWidget *parent = 0);
	virtual ~BuddiesLineEdit();

signals:
	void next();
	void previous();

};

#endif // BUDDIES_LINE_EDIT_H
