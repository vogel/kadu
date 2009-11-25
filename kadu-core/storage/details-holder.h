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

template <class DetailsClass>
class DetailsHolder
{
	DetailsClass *Details;

	void removeDetails()
	{
		if (!Details)
			return;

		detailsAboutToBeRemoved();
		delete Details;
		Details = 0;
	}

protected:
	DetailsHolder() :
			Details(0)
	{
	}

	virtual ~DetailsHolder()
	{
		removeDetails();
	}

	virtual void detailsAdded() {}
	virtual void detailsAboutToBeRemoved() {}

public:
	void setDetails(DetailsClass *details)
	{
		removeDetails();
		Details = details;
		if (Details)
			detailsAdded();
	}

	DetailsClass * details() const
	{
		return Details;
	}

};

#endif // DETAILS_HOLDER_H
