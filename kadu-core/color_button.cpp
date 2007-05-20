#include <qcolordialog.h>

#include "color_button.h"

ColorButton::ColorButton(QWidget *parent, const char *name)
	: QPushButton(parent, name)
{
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void ColorButton::onClick()
{
	if (setColor(QColorDialog::getColor(currentColor, this, "Color dialog")))
		emit changed(currentColor);
}

const QColor & ColorButton::color() const
{
	return currentColor;
}

bool ColorButton::setColor(const QColor &color)
{
	if (!color.isValid())
		return false;

	currentColor = color;
	QPixmap pm(35,10);
	pm.fill(color);
	setPixmap(pm);

	return true;
}
