/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef HISTORY_SEARCH_PARAMETERS_H
#define HISTORY_SEARCH_PARAMETERS_H

#include <QtCore/QString>

class HistorySearchParameters
{
	QString Query;
	
	public:
		HistorySearchParameters();
		
		HistorySearchParameters & setQuery(const QString &query);
		QString query() { return Query; }
		
};

#endif // HISTORY_SEARCH_PARAMETERS_H
