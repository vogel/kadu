/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * vcardfactory.h - class for caching vCards
 * Copyright (C) 2003  Michail Pishchagin
 */

#ifndef VCARD_FACTORY_H
#define VCARD_FACTORY_H

#include <QObject>
#include <QMap>
#include <QStringList>

#include "xmpp_vcard.h"
#include "xmpp_tasks.h"

namespace XMPP {
	class VCard;
	class Jid;
	class Task;
	class JT_VCard;
}
using namespace XMPP;

class VCardFactory : public QObject
{
	Q_OBJECT
	
public:
	static VCardFactory* instance();
	const VCard *vcard(const Jid &);
	void setVCard(const Jid &, const VCard &);
	void setVCard(Task *rootTask, const Jid &, const VCard &v, QObject* obj = 0, const char* slot = 0);
	JT_VCard *getVCard(const Jid &, Task *rootTask, const QObject *, const char *slot, bool cacheVCard = true);
	
signals:
	void vcardChanged(const Jid&);
	
protected:
	void checkLimit(QString jid, VCard *vcard);
	
private slots:
	void updateVCardFinished();
	void taskFinished();
	
private:
	VCardFactory();
	~VCardFactory();
	
	static VCardFactory* instance_;
	const int dictSize_;
	QStringList vcardList_;
	QMap<QString,VCard*> vcardDict_;

	void saveVCard(const Jid &, const VCard &);
};

#endif
