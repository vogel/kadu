/***************************************************************************
                          history.h  -  description
                             -------------------
    begin                : Sun Sep 9 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HISTORY_H
#define HISTORY_H

#include <qdialog.h>
#include <qmultilineedit.h>
#include "../libgadu/lib/libgadu.h"

class History : public QDialog {
    Q_OBJECT
    public:
	History(uin_t);

    protected:
	QMultiLineEdit *body;
};

char *timestamp(time_t = 0);
void appendHistory(unsigned int uin, unsigned char * msg, bool, time_t = 0);

#endif
