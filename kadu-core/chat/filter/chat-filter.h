/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACT_CHAT_FILTER
#define ABSTRACT_CHAT_FILTER

#include <QtCore/QObject>

#include "exports.h"

class Chat;

class KADUAPI ChatFilter : public QObject
{
	Q_OBJECT

public:
	ChatFilter(QObject *parent = 0) : QObject(parent) {}

	virtual bool acceptChat(Chat *chat) = 0;

signals:
	void filterChanged();

};

#endif // ABSTRACT_CHAT_FILTER
