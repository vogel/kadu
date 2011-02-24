/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef ROSTER_ITEM_H
#define ROSTER_ITEM_H

#include <QList>
#include <QObject>
#include <QVariant>
#include <QRect>
#include <QImage>

class rosterItem: public QObject {
Q_OBJECT

public:
	enum {Contact=0, MetaContact=1, Group=2, SubGroup=3};
	rosterItem(QString jid=QString(), QString dn=QString(), int type=0, rosterItem *parent = 0, QString s=QString());
	~rosterItem();

	void appendChild(rosterItem*);
	void takeChild(rosterItem*);

	void setExpanded(bool);

	rosterItem *child(int);
	rosterItem *child(QString, int col=0);
	int childCount() const;
	int availCount() const;
	int columnCount() const;
	int contactCount() const;
	QString data(int) const;
	int row() const;
	rosterItem *parent();
	int type(){return t;};

	void sort(bool showSubgroups=FALSE);

	void moveItem(int, int);

	void removeSubgroups();

	void setSubscription(QString);
	void setStatus(QString);
	void setDescr(QString);
	void setName(QString);

	QImage icon();

	const QList<rosterItem *> & childList() const { return childItems; }

private:
	bool expanded;
	int t;
	QList<rosterItem*> childItems;
	QString displayName, descr, jid, subscription, status;
	rosterItem *parentItem;
};
#endif
