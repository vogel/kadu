/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IDENTITIES_COMBO_BOX_H
#define IDENTITIES_COMBO_BOX_H

#include <QtGui/QComboBox>

#include "identities/identity.h"
#include "exports.h"

class IdentityModel;
class QSortFilterProxyModel;
class ActionsProxyModel;

class KADUAPI IdentitiesComboBox : public QComboBox
{
	Q_OBJECT

	IdentityModel *Model;
	ActionsProxyModel *ActionsModel;

	QAction *CreateNewIdentityAction;

	Identity CurrentIdentity;

private slots:
	void activatedSlot(int index);
	void resetIdentity();

public:
	explicit IdentitiesComboBox(QWidget *parent = 0);
	virtual ~IdentitiesComboBox();

	void setCurrentIdentity(Identity identity);
	Identity currentIdentity();

};

#endif // IDENTITIES_COMBO_BOX_H
