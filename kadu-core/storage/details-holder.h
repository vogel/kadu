/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DETAILS_HOLDER_H
#define DETAILS_HOLDER_H

template <class Class, class DetailsClass, class ManagerClass>
class DetailsHolder
{
	DetailsClass *Details;

	void setNewDetails(DetailsClass *details)
	{
		Details = details;
		if (!Details)
			return;

		detailsAdded();
		ManagerClass::instance()->detailsLoaded((Class *)this);
	}

	void removeOldDetails()
	{
		if (!Details)
			return;

		detailsAboutToBeRemoved();
		delete Details;
		Details = 0;

		ManagerClass::instance()->detailsUnloaded((Class *)this);
	}

protected:
	DetailsHolder() :
			Details(0)
	{
	}

	virtual ~DetailsHolder()
	{
		removeOldDetails();
	}

	virtual void detailsAdded() {}
	virtual void detailsAboutToBeRemoved() {}

public:
	void setDetails(DetailsClass *details)
	{
		removeOldDetails();
		setNewDetails(details);
	}

	DetailsClass * details() const
	{
		return Details;
	}

};

#endif // DETAILS_HOLDER_H
