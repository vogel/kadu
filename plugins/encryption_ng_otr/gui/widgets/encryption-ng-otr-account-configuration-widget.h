/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_OTR_ACCOUNT_CONFIGURATION_WIDGET_H
#define ENCRYPTION_NG_OTR_ACCOUNT_CONFIGURATION_WIDGET_H

#include "gui/widgets/account-configuration-widget.h"

class EncryptionNgOtrAccountConfigurationWidget : public AccountConfigurationWidget
{
	Q_OBJECT

	void createGui();

public:
	explicit EncryptionNgOtrAccountConfigurationWidget(const Account &account, QWidget *parent = 0);
	virtual ~EncryptionNgOtrAccountConfigurationWidget();

	virtual void apply();
	virtual void cancel();

};

#endif // ENCRYPTION_NG_OTR_ACCOUNT_CONFIGURATION_WIDGET_H
