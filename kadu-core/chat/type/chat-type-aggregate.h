/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_TYPE_AGGREGATE_H
#define CHAT_TYPE_AGGREGATE_H

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QIcon>

#include "chat/type/chat-type.h"

#include "exports.h"

class KADUAPI ChatTypeAggregate : public ChatType
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatTypeAggregate)

public:
	ChatTypeAggregate() {}
	virtual ~ChatTypeAggregate() {}

	virtual int sortIndex() const;
	virtual QString name() const;
	virtual QString displayName() const;
	virtual QIcon icon() const;

	virtual ChatDetails * createChatDetails(ChatShared *chatData) const;

};

Q_DECLARE_METATYPE(ChatTypeAggregate *)

#endif // CHAT_TYPE_AGGREGATE_H
