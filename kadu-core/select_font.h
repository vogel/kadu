#ifndef SELECT_FONT_H
#define SELECT_FONT_H

#include <qglobal.h>

#include <qfont.h>
#include <q3hbox.h>

class QLineEdit;

class SelectFont : public Q3HBox
{
	Q_OBJECT

	QFont currentFont;
	QLineEdit *fontEdit;

private slots:
	void onClick();

public:
	SelectFont(QWidget *parent = 0,  const char *name = 0);
	const QFont &font() const;
	void setFont(const QFont &font);

signals:
	void fontChanged(QFont font);

};

#endif // SELECT_FONT_H
