/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ACCOUNT_CONFIGURATION_WIDGET_H
#define ACCOUNT_CONFIGURATION_WIDGET_H

#include "accounts/account.h"
#include "exports.h"

#include "modal-configuration-widget.h"

class KADUAPI AccountConfigurationWidget : public ModalConfigurationWidget
{
	Q_OBJECT

	Account MyAccount;

public:
	explicit AccountConfigurationWidget(const Account &account, QWidget *parent = 0);
	virtual ~AccountConfigurationWidget();

	Account account() const;

};

#endif // ACCOUNT_CONFIGURATION_WIDGET_H
