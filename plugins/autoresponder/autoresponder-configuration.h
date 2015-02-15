/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AUTORESPONDER_CONFIGURATION_H
#define AUTORESPONDER_CONFIGURATION_H

#include <QtCore/QString>

/**
 * @addtogroup Autoresponder
 * @{
 */

/**
 * @class AutoresponderConfiguration
 * @short Configuration class for Autoresponder.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class is used to configure Autoresponder.
 *
 * Properties:
 * <ul>
 *   <li>autoRespondText - content of auto response</li>
 *   <li>respondConferences - if true then conferences will be responded too</li>
 *   <li>respondOnlyFirst - if true response will be sent only after first message</li>
 *   <li>statusAvailable - if true response will be sent when own status is online</li>
 *   <li>statusBusy - if true response will be sent when own status is busy</li>
 *   <li>statusInvisible - if true response will be sent when own status is invisible</li>
 * </ul>
 */
class AutoresponderConfiguration
{
	QString AutoRespondText;
	bool RespondConferences;
	bool RespondOnlyFirst;
	bool StatusAvailable;
	bool StatusBusy;
	bool StatusInvisible;

public:
	AutoresponderConfiguration();
	AutoresponderConfiguration(const AutoresponderConfiguration &copyMe);

	AutoresponderConfiguration & operator = (const AutoresponderConfiguration &copyMe);

	/**
	 * @short Set value of autoRespondText property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param autoRespondText new value of autoRespondText property
	 */
	void setAutoRespondText(const QString &autoRespondText);

	/**
	 * @short Return value of autoRespondText property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of autoRespondText property
	 */
	QString autoRespondText() const;

	/**
	 * @short Set value of respondConferences property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param respondConferences new value of respondConferences property
	 */
	void setRespondConferences(bool respondConferences);

	/**
	 * @short Return value of respondConferences property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of respondConferences property
	 */
	bool respondConferences() const;

	/**
	 * @short Set value of respondOnlyFirst property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param respondOnlyFirst new value of respondOnlyFirst property
	 */
	void setRespondOnlyFirst(bool respondOnlyFirst);

	/**
	 * @short Return value of respondOnlyFirst property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of respondOnlyFirst property
	 */
	bool respondOnlyFirst() const;

	/**
	 * @short Set value of statusAvailable property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param statusAvailable new value of statusAvailable property
	 */
	void setStatusAvailable(bool statusAvailable);

	/**
	 * @short Return value of statusAvailable property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of statusAvailable property
	 */
	bool statusAvailable() const;

	/**
	 * @short Set value of statusBusy property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param statusBusy new value of statusBusy property
	 */
	void setStatusBusy(bool statusBusy);

	/**
	 * @short Return value of statusBusy property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of statusBusy property
	 */
	bool statusBusy() const;

	/**
	 * @short Set value of statusInvisible property.
	 * @author Rafał 'Vogel' Malinowski
	 * @param statusInvisible new value of statusInvisible property
	 */
	void setStatusInvisible(bool statusInvisible);

	/**
	 * @short Return value of statusInvisible property.
	 * @author Rafał 'Vogel' Malinowski
	 * @return value of statusInvisible property
	 */
	bool statusInvisible() const;

};

/**
 * @}
 */

#endif // AUTORESPONDER_CONFIGURATION_H
