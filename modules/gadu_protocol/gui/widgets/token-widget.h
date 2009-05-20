 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TOKEN_WIDGET_H
#define TOKEN_WIDGET_H

#include <QtGui/QWidget>

class QLabel;
class QLineEdit;

class TokenFetcher;

class TokenWidget : public QWidget
{
	Q_OBJECT

	QLabel *TokenImage;
	QLineEdit *TokenCode;

	QString TokenId;

	TokenFetcher *Fetcher;

private slots:
	void tokenFetched(const QString &tokenId, QPixmap tokenImage);
	void refreshToken();

public:
	explicit TokenWidget(QWidget *parent = 0);
	virtual ~TokenWidget();

	QString tokenId();
	QString tokenValue();

};

#endif // TOKEN_WIDGET_H
