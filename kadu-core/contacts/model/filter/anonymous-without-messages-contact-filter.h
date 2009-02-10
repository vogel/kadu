/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANONYMOUS_WITHOUT_MESSAGES_CONTACT_FILTER_H
#define ANONYMOUS_WITHOUT_MESSAGES_CONTACT_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-contact-filter.h"

class AnonymousWithoutMessagesContactFilter : public AbstractContactFilter
{
	Q_OBJECT

	bool Enabled;

public:
	AnonymousWithoutMessagesContactFilter(QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptContact(Contact contact);

};

Q_DECLARE_METATYPE(AnonymousWithoutMessagesContactFilter *)

#endif // ANONYMOUS_WITHOUT_MESSAGES_CONTACT_FILTER_H
