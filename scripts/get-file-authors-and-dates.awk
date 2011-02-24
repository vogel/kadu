BEGIN {
	FS = "\t"
	ALIAS["Rafał Malinowski (malinowskirafal@wp.pl)"] = "Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)"
	ALIAS["Juzef (juzefwt@tlen.pl)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Juzef (juzef@arch.localdomain)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Juzef (juzefwt@gmail.com)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Juzef, Juzef (juzefwt@gmail.com)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Juzef, Wojciech Treter (juzefwt@gmail.com)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Wojtek (juzef@slack.(none))"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["root (root@slack.(none))"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Bartlomiej Zimon (uzi18@o2.pl)"] = "Bartłomiej Zimoń (uzi18@o2.pl)"
	ALIAS["Bartłomiej Zimoń (uzi18@go2.pl)"] = "Bartłomiej Zimoń (uzi18@o2.pl)"
	ALIAS["Piotr Galiszewski (piotrgaliszewski@gmail.com)"] = "Piotr Galiszewski (piotr.galiszewski@kadu.im)"
	ALIAS["Kermit (plaza.maciej@gmail.com)"] = "Maciej Płaza (plaza.maciej@gmail.com)"
	ALIAS["Piotr Dąbrowski (ultr@ultra.tux-net)"] = "Piotr Dąbrowski (ultr@ultr.pl)"
	ALIAS["ultr (ultr@ultr.pl)"] = "Piotr Dąbrowski (ultr@ultr.pl)"
	ALIAS["Michał Obrembski (byczy@Syster-Desktop.(none))"] = "Michał Obrembski (byku@byku.com.pl)"
	ALIAS["darom (darom@alari.pl)"] = "Dariusz Markowicz (darom@alari.pl)"
	ALIAS["prudy (prudy1@o2.pl)"] = "Przemysław Rudy (prudy1@o2.pl)"
	ALIAS["Przemysław Rudy (prudy@prasus.localdomain)"] = "Przemysław Rudy (prudy1@o2.pl)"
	ALIAS["Piotr Pełzowski (patpi@ubuntu.(none))"] = "Piotr Pełzowski (floss@pelzowski.eu)"
	ALIAS["Robert Kolatzek (neo@lucid.(none))"] = "Robert Kolatzek (neo@heilsberg.org)"
	ALIAS["Robert Kolatzek (neo@r2d2.(none))"] = "Robert Kolatzek (neo@heilsberg.org)"
	ALIAS["Robert K (neo@heilsberg.org)"] = "Robert Kolatzek (neo@heilsberg.org)"
	ALIAS["neo (neo@heilsberg.org)"] = "Robert Kolatzek (neo@heilsberg.org)"
	ALIAS["maemo (maemo@maemo-desktop.(none))"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["root (root@bash.(none))"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["Tomasz Rostański (rozteck@interia.pl)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["tomek (rostan@podkowa.net)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["Tomek (rozteck@interia.pl)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["tomek (tomek@bash.(none))"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["Tomek (tomek@macbook.local)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["Tomek (tomek@tomasz-rostanskis-macbook.local)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["tomek (tomek@tomeks-macbook.local)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["trostanski (rostan@podkowa.net)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["trostanski (trostanski@localhost.localdomain)"] = "Tomasz Rostański (rozteck@interia.pl)"
	ALIAS["unknown (tomek@.(none))"] = "Tomasz Rostański (rozteck@interia.pl)"
}

{
	author = $2
	if (author in ALIAS)
		author = ALIAS[author]

	# do not join dates, use 2002, 2003, 2004 instead of 2002-2004
	if (author in AUTHORS) {
		AUTHORS[author] = AUTHORS[author] ", " substr($1, 0, 4)
	} else {
		AUTHORS[author] = substr($1, 0, 4)
	}
}

END {
	for (author in AUTHORS) {
		printf sprintf(" * Copyright %s %s\n", AUTHORS[author], author)
	}
}
