/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qsocketnotifier.h>
#include "../libgadu/lib/libgadu.h"

class PersonalInfoDialog : public QDialog
{
	Q_OBJECT

	private:
		QLineEdit* NicknameEdit;
		QLineEdit* NameEdit;
		QLineEdit* SurnameEdit;
		QComboBox* GenderCombo;
		QLineEdit* BirthyearEdit;
		QLineEdit* CityEdit;
		QSocketNotifier* SocketReadNotifier;
		QSocketNotifier* SocketWriteNotifier;
		struct gg_http* http;
		enum DialogState { READY, READING, WRITTING };
		DialogState State;
		void socketEvent();
		void createSocketNotifiers();
		void deleteSocketNotifiers();
		void fillFields();

	private slots:
		void OkButtonClicked();
		void dataReceived();
		void dataSent();

	protected:
		virtual void closeEvent(QCloseEvent * e);

	public:
		PersonalInfoDialog(QDialog *parent=0, const char *name=0);
};

#endif
