/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDUSER_H
#define ADDUSER_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include "../libgadu/lib/libgadu.h"

class Adduser : public QDialog {
	Q_OBJECT

	public:
		Adduser(QDialog* parent=0, const char *name=0);

	protected:
		QLineEdit * e_nickname, * e_uin, * e_fname, * e_lname, * e_altnick, * e_tel;
		QLabel * effect;

	protected slots:
		void Add();
};

#endif
