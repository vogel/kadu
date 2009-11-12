/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TLEN_CONTACT_ACCOUNT_DATA_WIDGET
#define TLEN_CONTACT_ACCOUNT_DATA_WIDGET

#include "gui/widgets/contact-widget.h"

class TlenContact;

class TlenContactWidget : public ContactWidget
{
	Q_OBJECT

	TlenContactData;
	void createGui();

public:
	explicit TlenContactWidget(TlenContact contact, QWidget *parent = 0);
	~TlenContactWidget();

};

#endif // TLEN_CONTACT_ACCOUNT_DATA_WIDGET
