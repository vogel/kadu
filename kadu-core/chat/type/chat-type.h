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
#include <QtCore/QVariant>
#include <QtGui/QIcon>

#include "exports.h"

class ChatDetails;
class ChatShared;

class KADUAPI ChatType : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatType)

public:
	ChatType() {}
	virtual ~ChatType() {}

	virtual int sortIndex() const = 0;
	virtual QString name() const = 0;
	virtual QString displayName() const = 0;
	virtual QIcon icon() const = 0;

	virtual ChatDetails * createChatDetails(ChatShared *chatData) const = 0;

	bool operator == (const ChatType &compare) const;
	bool operator < (const ChatType &compare) const;

};

Q_DECLARE_METATYPE(ChatType *)

#endif // CHAT_TYPE_H
