/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_STATUS_H
#define GADU_STATUS_H

#include "gadu_exports.h"
#include "status.h"

class GADUAPI GaduStatus : public UserStatus
{

public:
	GaduStatus();
	virtual ~GaduStatus();

	GaduStatus &operator = (const UserStatus &copyMe);

	virtual QPixmap pixmap(eUserStatus status, bool has_desc, bool mobile) const;
	virtual QString pixmapName(eUserStatus status, bool has_desc, bool mobile) const;

	int toStatusNumber() const;
	static int toStatusNumber(eUserStatus status, bool has_desc);

	void fromStatusNumber(int statusNumber, const QString &description);

	virtual UserStatus *copy() const;
	virtual QString protocolName() const;

};

#endif // GADU_STATUS_H
