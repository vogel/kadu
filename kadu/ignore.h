/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IGNORE_H
#define IGNORE_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qvaluelist.h>

#include "misc.h"
#include "libgadu.h"

class Ignored : public QDialog {
	Q_OBJECT
	public:
		Ignored(QDialog *parent = 0, const char *name = 0);

	private:
		QListBox *list;
		QLineEdit *e_uin;

	private slots:
		void remove();
		void add();
		void getList();	
};

int readIgnored();
int writeIgnored(QString filename = QString::null);
void addIgnored(UinsList uins);
void delIgnored(UinsList uins);
bool isIgnored(UinsList uins);

extern QValueList<UinsList> ignored;

#endif
