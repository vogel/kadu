BEGIN {
	FS = "\t"
	ALIAS["Rafał Malinowski (malinowskirafal@wp.pl)"] = "Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)"
	ALIAS["Juzef (juzefwt@tlen.pl)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Juzef (juzefwt@gmail.com)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Juzef, Juzef (juzefwt@gmail.com)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Juzef, Wojciech Treter (juzefwt@gmail.com)"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Wojtek (juzef@slack.(none))"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["root (root@slack.(none))"] = "Wojciech Treter (juzefwt@gmail.com)"
	ALIAS["Bartlomiej Zimon (uzi18@o2.pl)"] = "Bartłomiej Zimoń (uzi18@o2.pl)"
	ALIAS["Bartłomiej Zimoń (uzi18@go2.pl)"] = "Bartłomiej Zimoń (uzi18@o2.pl)"
	ALIAS["Piotr Galiszewski (piotrgaliszewski@gmail.com)"] = "Piotr Galiszewski (piotr.galiszewski@kadu.im)"

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
