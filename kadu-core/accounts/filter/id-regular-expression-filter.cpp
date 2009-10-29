 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "id-regular-expression-filter.h"

IdRegularExpressionFilter::IdRegularExpressionFilter(QObject *parent) :
		AbstractAccountFilter(parent)
{
}

IdRegularExpressionFilter::~IdRegularExpressionFilter()
{
}

void IdRegularExpressionFilter::setId(QString id)
{
	if (Id == id)
		return;

	Id = id;
	emit filterChanged();
}

bool IdRegularExpressionFilter::acceptAccount(Account account)
{
	if (Id.isEmpty())
		return true;
	
	QRegExp regularExpression = account.protocolHandler()->protocolFactory()->idRegularExpression();
	if (regularExpression.exactMatch(Id))
		return true;

	return regularExpression.matchedLength() == Id.length(); // for not-full matches
}
