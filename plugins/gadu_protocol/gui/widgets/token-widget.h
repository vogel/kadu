/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef TOKEN_WIDGET_H
#define TOKEN_WIDGET_H

#include <QtGui/QWidget>

class QLabel;
class QLineEdit;

class GaduTokenFetcher;

class TokenWidget : public QWidget
{
	Q_OBJECT

	QLabel *TokenImage;
	QLineEdit *TokenCode;
	QMovie *WaitMovie;

	QString TokenId;

	GaduTokenFetcher *Fetcher;

private slots:
	void tokenFetched(const QString &tokenId, const QPixmap &tokenImage);

public:
	explicit TokenWidget(QWidget *parent = 0);
	virtual ~TokenWidget();

	QString tokenId();
	QString tokenValue();
	void setTokenValue(const QString &tokenValue);

public slots:
	void refreshToken();

signals:
	void modified();

};

#endif // TOKEN_WIDGET_H
