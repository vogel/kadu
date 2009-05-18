 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FIRST_EMPTY
#define FIRST_EMPTY

#include <QtCore/QAbstractListModel>

class FirstEmpty : public QAbstractListModel
{
	Q_OBJECT

	QString EmptyString;
	int EmptyCount;

public:
	FirstEmpty(const QString &emptyString, QObject *parent = 0);
	explicit FirstEmpty(QObject *parent = 0);
	virtual ~FirstEmpty();

	int emptyCount() const { return EmptyCount; }

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

};

#endif // PROTOCOLS_MODEL
