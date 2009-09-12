/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_ACCOUNT
#define TLEN_ACCOUNT

#include <QtCore/QString>

#include "open-chat-with/tlen-open-chat-with-runner.h"

#include "accounts/account.h"

class TlenAccount : public Account
{
	TlenOpenChatWithRunner *OpenChatRunner;

public:
	explicit TlenAccount(const QUuid &uuid = QUuid());
	virtual ~TlenAccount();

	virtual bool setId(const QString &id);

	virtual void load();
	virtual void store();

};

#endif // TLEN_ACCOUNT
