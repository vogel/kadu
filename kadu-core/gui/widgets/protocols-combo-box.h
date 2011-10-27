/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef PROTOCOLS_COMBO_BOX_H
#define PROTOCOLS_COMBO_BOX_H

#include "protocols/protocol-factory.h"
#include "exports.h"

#include "gui/widgets/actions-combo-box.h"

class AbstractProtocolFilter;
class ProtocolsModelProxy;

class KADUAPI ProtocolsComboBox : public ActionsComboBox
{
	Q_OBJECT
	Q_PROPERTY(ProtocolFactory* currentProtocol READ currentProtocol WRITE setCurrentProtocol)

	ProtocolsModelProxy *ProxyModel;

protected:
	virtual void valueChanged(const QVariant &value, const QVariant &previousValue);
	virtual bool compare(const QVariant &left, const QVariant &right) const;

public:
	explicit ProtocolsComboBox(QWidget *parent = 0);
	virtual ~ProtocolsComboBox();

	void setCurrentProtocol(ProtocolFactory *protocol);
	ProtocolFactory * currentProtocol();

	void addFilter(AbstractProtocolFilter *filter);
	void removeFilter(AbstractProtocolFilter *filter);

signals:
	void protocolChanged(ProtocolFactory *protocol = 0, ProtocolFactory *lastProtocol = 0);

};

#endif // PROTOCOLS_COMBO_BOX_H
