 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACT_BUDDIES_MODEL_H
#define ABSTRACT_BUDDIES_MODEL_H

class QModelIndex;

class Buddy;

class AbstractBuddiesModel
{

public:
	virtual Buddy buddyAt(const QModelIndex &index) const;
	virtual const QModelIndex buddyIndex(Buddy buddy) const = 0;

};

#endif // ABSTRACT_BUDDIES_MODEL_H
