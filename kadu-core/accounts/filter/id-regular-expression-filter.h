 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ID_REGULAR_EXPRESSION_FILTER
#define ID_REGULAR_EXPRESSION_FILTER

#include "accounts/filter/abstract-account-filter.h"

class IdRegularExpressionFilter : public AbstractAccountFilter
{
	Q_OBJECT

	QString Id;

protected:
	virtual bool acceptAccount(Account *account);

public:
	IdRegularExpressionFilter(QObject *parent = 0);
	virtual ~IdRegularExpressionFilter();

	void setId(QString id);

};

#endif // ID_REGULAR_EXPRESSION_FILTER
