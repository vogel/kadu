#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qcolor.h>
#include <qpushbutton.h>

/**
    Kontrolka do wyboru koloru
**/
class ColorButton : public QPushButton
{
	Q_OBJECT

	QColor currentColor;

public:
	ColorButton(QWidget *parent = 0, const char * name = 0);
	const QColor &color() const;
	bool setColor(const QColor &color);

private slots:
	void onClick();

signals:
	void changed(const QColor &color);

};

#endif // COLOR_BUTTON_H
