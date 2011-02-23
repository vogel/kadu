/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
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

#ifndef EMOTICONS_SELECTOR_BUTTON_H
#define EMOTICONS_SELECTOR_BUTTON_H

#include <QtGui/QLabel>

class EmoticonSelectorButton : public QLabel
{
	Q_OBJECT

	QString EmoticonString;
	QString AnimPath;
	QString StaticPath;

private slots:
	void buttonClicked();

protected:
	class MovieViewer;
	friend class MovieViewer;

	/**
		Funkcja obs�uguj�ca ruch kursora na obszarze emotikonki.
	**/
	void mouseMoveEvent(QMouseEvent *e);

public:
	/**
		Konstruktor tworz�cy przycisk z emotikonk�
		\param parent widget na kt�rym osadzona ma by� przycisk z emotikonk�
		\param emoticon_string nazwa kt�ra ma by� wy�wietlana po najechaniu na
		przycisk
		\param static_path �cie�ka do statycznej emotikonki
		\param anim_path �cie�ka do animowanej emotikonki
	**/
	EmoticonSelectorButton(const QString &emoticon_string, const QString &static_path, const QString &anim_path, QWidget *parent);

signals:
	/**
		Sygna� emitowany po naci�ni�ciu przycisku z emotikonk�
		przekazywana jest warto�� emoticon_string (przewa�nie jest to tag)
	**/
	void clicked(const QString &emoticon_string);

};


// TODO: make it ignoring wheel events so the widget can be scrolled with mouse wheel
class EmoticonSelectorButton::MovieViewer : public QLabel
{
	Q_OBJECT

protected:
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

public:
	explicit MovieViewer(EmoticonSelectorButton *parent);

signals:
	void clicked();

};

#endif // EMOTICONS_SELECTOR_BUTTON_H
