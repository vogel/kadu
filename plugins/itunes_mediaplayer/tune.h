/*
 * tune.h
 * Copyright (C) 2006  Remko Troncon
 * Modified by Tomasz "Dorregaray" Rostanski 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef TUNE_H
#define TUNE_H

#include <QtCore/QString>

/**
 * \brief A class represinting a tune.
 */
class Tune
{
public:
	enum State 
	{
		unknown,
		stopped,
		paused,
		playing
	};

	/**
	 * \brief Constructs an empty tune
	 */
	Tune() : time_(0) { }
	
	const QString& name()     const { return name_;     }
	const QString& artist()   const { return artist_;   }
	const QString& album()    const { return album_;    }
	const QString& location() const { return location_; }
	unsigned int track()      const { return track_;    }
	unsigned int time()       const { return time_;     }
	State state()             const { return state_;    }
	unsigned int started()    const { return started_;  }

	/**
	 * \brief Returns a string representation of the tune's playing time.
	 */
	QString timeString() const 
	{
		return QString("%1:%2").arg(time_ / 60).arg(time_ % 60, 2, 10, QChar('0'));
	}

	/**
	 * \brief Checks whether this is a null tune.
	 */
	bool isNull() const 
	{
		return  name_.isEmpty() && 
			artist_.isEmpty()   && 
			album_.isEmpty()    && 
			location_.isEmpty() && 
			track_ == 0         && 
			time_ == 0          &&
			started_ == 0       &&
			state_ == Tune::unknown; 
	}

	/**
	 * \brief Compares this tune with another tune for equality.
	 */
	bool operator==(const Tune& t) const 
	{
		return  name_ == t.name_     && 
			artist_   == t.artist_   && 
			album_    == t.album_    && 
			location_ == t.location_ && 
			track_    == t.track_    && 
			time_     == t.time_     &&
			started_  == t.started_  &&
			state_    == t.state_;
	}

	/**
	 * \brief Compares this tune with another tune for inequality.
	 */
	bool operator!=(const Tune& t) const 
	{
		return !((*this) == t);
	}

	void setName(const QString& name)         { name_ = name;         }
	void setArtist(const QString& artist)     { artist_ = artist;     }
	void setAlbum(const QString& album)       { album_ = album;       }
	void setLocation(const QString& location) { location_ = location; }
	void setTrack(int track)                  { track_ = track;       }
	void setTime(unsigned int time)           { time_ = time;         }
	void setState(State state)                { state_ = state;       }
	void setStarted(unsigned int started)     { started_ = started;   }

private:
	QString name_, artist_, album_, location_,;
	unsigned int time_, track_, started_;
	State state_;
};

#endif
