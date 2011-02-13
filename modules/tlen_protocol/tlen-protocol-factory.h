/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef TLEN_PROTOCOL_FACTORY_H
#define TLEN_PROTOCOL_FACTORY_H

#include "protocols/protocol-factory.h"

class TlenProtocolFactory : public ProtocolFactory
{
	Q_OBJECT
	Q_DISABLE_COPY(TlenProtocolFactory)

	static TlenProtocolFactory *Instance;
	QList<StatusType *> SupportedStatusTypes;
	QRegExp IdRegularExpression;

	TlenProtocolFactory();

public:
	static TlenProtocolFactory * instance() { return Instance; }
	static void createInstance();
	static void destroyInstance();

	virtual Protocol * createProtocolHandler(Account account);
	virtual AccountDetails * createAccountDetails(AccountShared *accountShared);
	virtual ContactDetails * createContactDetails(ContactShared *contactShared);
	virtual AccountAddWidget * newAddAccountWidget(QWidget *parent = 0);
	virtual QWidget * newCreateAccountWidget(QWidget *parent = 0);
	virtual AccountEditWidget * newEditAccountWidget(Account, QWidget*);
	virtual ContactWidget * newContactWidget(Contact contact, QWidget *parent = 0);

	virtual QList<StatusType *> supportedStatusTypes();
	virtual QString idLabel();
	virtual QRegExp idRegularExpression();

	virtual QString name() { return "tlen"; }
	virtual QString displayName() { return "Tlen"; }
	virtual QIcon icon();
};

#endif // TLEN_PROTOCOL_FACTORY_H
