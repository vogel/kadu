/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANONYMOUS_CONTACT_FILTER_H
#define ANONYMOUS_CONTACT_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-contact-filter.h"

class AnonymousContactFilter : public AbstractContactFilter
{
	Q_OBJECT

	bool Enabled;

public:
	AnonymousContactFilter(QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptContact(Buddy contact);

};

Q_DECLARE_METATYPE(AnonymousContactFilter *)

#endif // ANONYMOUS_CONTACT_FILTER_H
