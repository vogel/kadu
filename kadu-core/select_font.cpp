#include <qfontdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "select_font.h"

SelectFont::SelectFont(QWidget *parent, const char *name)
	: QHBox(parent, name)
{
	setSpacing(5);

	fontEdit = new QLineEdit(this);
	fontEdit->setReadOnly(true);

	QPushButton *button = new QPushButton(tr("Select"), this);
	connect(button, SIGNAL(clicked()), this, SLOT(onClick()));
}

void SelectFont::setFont(const QFont &font)
{
	currentFont = font;
	fontEdit->setText(QString("%1 %2").arg(currentFont.family(), QString::number(currentFont.pointSize())));
}

const QFont & SelectFont::font() const
{
	return currentFont;
}

void SelectFont::onClick()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, currentFont);

	if (ok)
		setFont(f);
}
