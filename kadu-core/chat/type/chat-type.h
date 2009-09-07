/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef CHAT_TYPE_H
#define CHAT_TYPE_H

#include <QtCore/QString>
#include <QtGui/QIcon>

class ChatType
{
	int SortIndex;
	QString Name;
	QString DisplayName;
	QIcon Icon;

public:
	ChatType();
	ChatType(int sortIndex, const QString &name, const QString &displayName, const QIcon &icon);
	ChatType(const ChatType &copy);

	int sortIndex() const { return SortIndex; }
	QString name() const { return Name; }
	QString displayName() const { return DisplayName; }
	QIcon icon() const { return Icon; }

	bool operator == (const ChatType &compare) const;
	bool operator < (const ChatType &compare) const;

};

#endif // CHAT_TYPE_H
