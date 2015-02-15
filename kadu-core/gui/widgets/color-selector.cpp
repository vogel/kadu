/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QPixmap>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QGridLayout>

#include "misc/misc.h"
#include "debug.h"

#include "color-selector.h"

const char colors[16][8] = {"#FF0000", "#A00000", "#00FF00", "#00A000", "#0000FF", "#0000A0", "#FFFF00",
	"#A0A000", "#FF00FF", "#A000A0", "#00FFFF", "#00A0A0", "#FFFFFF", "#A0A0A0", "#808080", "#000000"};

ColorSelectorButton::ColorSelectorButton(const QColor &qcolor, int width, QWidget *parent) :
		QPushButton(parent), color(qcolor)
{
	const int WIDTH1 = 15;
	const int BORDER1 = 3;

	QPixmap p(WIDTH1 * width + (width - 1) * (BORDER1 * 2), WIDTH1);
	p.fill(qcolor);
	setIcon(p);
//	setAutoRaise(true);
	setMouseTracking(true);
	setFixedSize(WIDTH1 * width + (BORDER1 * 2) + (width - 1) * (BORDER1 * 2), WIDTH1 + (BORDER1 * 2));
	setToolTip(color.name());
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void ColorSelectorButton::buttonClicked()
{
	emit clicked(color);
}

ColorSelector::ColorSelector(const QColor &defColor, const QWidget *activatingWidget, QWidget *parent) :
		QWidget(parent, Qt::Popup)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);

	QList<QColor> qcolors;
	int i;

	for (i = 0; i < 16; ++i)
		qcolors.append(colors[i]);

	int selector_width = 4; //sqrt(16)
	QGridLayout *grid = new QGridLayout(this);

	i = 0;
	foreach (const QColor &color, qcolors)
	{
		ColorSelectorButton *btn = new ColorSelectorButton(color, 1, this);
		grid->addWidget(btn, i / selector_width, i % selector_width);
		connect(btn, SIGNAL(clicked(const QColor &)), this, SLOT(iconClicked(const QColor &)));
		++i;
	}
	if (!qcolors.contains(defColor))
	{
		ColorSelectorButton* btn = new ColorSelectorButton(defColor, 4, this);
		grid->addWidget(btn, 4, 0, 1, 4);
		connect(btn, SIGNAL(clicked(const QColor &)), this, SLOT(iconClicked(const QColor &)));
	}

	// oblicz pozycj� widgetu do kt�rego r�wnamy
	QPoint w_pos = activatingWidget->mapToGlobal(QPoint(0,0));
	// oblicz rozmiar selektora
	QSize e_size = sizeHint();
	// oblicz rozmiar pulpitu
	QSize s_size = QApplication::desktop()->size();
	// oblicz dystanse od widgetu do lewego brzegu i do prawego
	int l_dist = w_pos.x();
	int r_dist = s_size.width() - (w_pos.x() + activatingWidget->width());
	// oblicz pozycj� w zale�no�ci od tego czy po lewej stronie
	// jest wi�cej miejsca czy po prawej
	int x;
	if (l_dist >= r_dist)
		x = w_pos.x() - e_size.width();
	else
		x = w_pos.x() + activatingWidget->width();
	// oblicz pozycj� y - centrujemy w pionie
	int y = w_pos.y() + activatingWidget->height()/2 - e_size.height()/2;
	// je�li wychodzi poza doln� kraw�d� to r�wnamy do niej
	if (y + e_size.height() > s_size.height())
		y = s_size.height() - e_size.height();
	// je�li wychodzi poza g�rn� kraw�d� to r�wnamy do niej
	if (y < 0)
		y = 0;
	// ustawiamy selektor na wyliczonej pozycji
	move(x, y);

	kdebugf2();
}

void ColorSelector::iconClicked(const QColor &color)
{
	emit colorSelect(color);
	close();
}

#include "moc_color-selector.cpp"
