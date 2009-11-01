/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ONLINE_CONTACT_FILTER_H
#define ONLINE_CONTACT_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-contact-filter.h"

class OnlineContactFilter : public AbstractContactFilter
{
	Q_OBJECT

	bool Enabled;

public:
	OnlineContactFilter(QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptContact(Buddy contact);

};

Q_DECLARE_METATYPE(OnlineContactFilter *)

#endif // ONLINE_CONTACT_FILTER_H
