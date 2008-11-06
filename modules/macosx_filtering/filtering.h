/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILTERING_H
#define FILTERING_H

#include <QtCore/QEvent>
#include <QtGui/QWidget>

#include "configuration_aware_object.h"
#include "usergroup.h"

class QMacSearchBox;

class Filtering : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	UserGroup *filter;
	QMacSearchBox *search;

	void createDefaultConfiguration();
	
	void clearFilter();
	void filterWith(const QString& f);
	bool checkString (const QString &hay, const QString& needle);
	
private slots:
	void textChanged(const QString& s);

protected:

	virtual void configurationUpdated();

public:
	Filtering();
	virtual ~Filtering();

};

extern Filtering *filtering;

#endif
