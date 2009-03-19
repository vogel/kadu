#ifndef KADU_TOKEN_DIALOG_H
#define KADU_TOKEN_DIALOG_H

#include "base.h"
#include "image-widget.h"

class TokenDialog : public QDialog
{
	Q_OBJECT

	QLineEdit *tokenedit;

public:
	TokenDialog(QPixmap tokenImage, QDialog *parent = 0);
	void getValue(QString &tokenValue);
	QString getValue();

};

#endif
