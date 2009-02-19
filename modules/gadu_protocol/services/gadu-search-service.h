/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SEARCH_H
#define GADU_SEARCH_H

#include <QtCore/QObject>

#include "gadu-search-record.h"
#include "gadu-search-result.h"

#include <libgadu.h>

class GaduSearch : public QObject
{
	Q_OBJECT

	gg_session *Session;

public:
	GaduSearch(gg_session *sess) : Session(sess) {}
	
signals:
	void newSearchResults(SearchResults &searchResults, int seq, int lastUin);
	
public slots:
	void newResults(gg_pubdir50_t res);
	void searchInPubdir(SearchRecord &searchRecord);
	void searchNextInPubdir(SearchRecord &searchRecord);
	void stopSearchInPubdir(SearchRecord &searchRecord);

};

#endif // GADU_SEARCH_H
