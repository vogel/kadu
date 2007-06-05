/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PREVIEW_H
#define PREVIEW_H

#include "userlistelement.h"

#include "kadu_text_browser.h"

class Preview : public KaduTextBrowser
{
	Q_OBJECT

	UserListElement ule;

public:
	Preview(QWidget *parent = 0, char *name = 0);
	~Preview();

public slots:
	void syntaxChanged(const QString &content);

};

#endif // PREVIEW_H
