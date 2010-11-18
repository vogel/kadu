/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACCOUNT_CREATE_WIDGET_H
#define ACCOUNT_CREATE_WIDGET_H

#include "accounts/account.h"
#include "gui/widgets/modal-configuration-widget.h"

#include "exports.h"

class KADUAPI AccountCreateWidget : public ModalConfigurationWidget
{
	Q_OBJECT

public:
	explicit AccountCreateWidget(QWidget *parent = 0) : ModalConfigurationWidget(parent) {}
	virtual ~AccountCreateWidget() {}

signals:
	void accountCreated(Account account);
	void cancelled();

};

#endif // ACCOUNT_CREATE_WIDGET_H
