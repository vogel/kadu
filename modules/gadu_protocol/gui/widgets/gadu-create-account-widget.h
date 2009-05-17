 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CREATE_ACCOUNT_WIDGET_H
#define GADU_CREATE_ACCOUNT_WIDGET_H

#include <QtGui/QWidget>

class QGridLayout;
class QLabel;

class GaduCreateAccountWidget : public QWidget
{
	Q_OBJECT

	QList<QWidget *> HaveNumberWidgets;
	QList<QWidget *> DontHaveNumberWidgets;

	QLabel *TokenImage;
	QString TokenCode;
	QString TokenValue;

	void createGui();
	void createIHaveAccountGui(QGridLayout *gridLayout);
	void createRegisterAccountGui(QGridLayout *gridLayout);

private slots:
	void haveNumberChanged(bool haveNumber);
	void tokenFetched(const QString &tokenId, QPixmap tokenImage);

public:
	explicit GaduCreateAccountWidget(QWidget *parent = 0);
	virtual ~GaduCreateAccountWidget();

};

#endif // GADU_CREATE_ACCOUNT_WIDGET_H
