/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACT_TOOL_TIP_H
#define ABSTRACT_TOOL_TIP_H

class QPoint;

class Contact;

class AbstractToolTip
{

public:
	virtual void showToolTip(const QPoint &point, Contact contact) = 0;
	virtual void hideToolTip() = 0;

};

#endif // ABSTRACT_TOOL_TIP_H
