/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *	ustawienia dla config_wizarda
 *
 *	w tym pliku dodajemy zestawy skladni panelu informacyjnego, skladnie dymkow oraz zestawy kolorystyczne
 *	nalezy tylko pamietac o zmianie liczby definiujacej liczbe el. do wyswietlenia
 *	kazdy zestaw w osobnej linii, skladniki zestawow odddzielone przecinkami
 *
 *	Uwaga!! Wszelkich zmian dokonujemy przed kompilacja modulu
 */


/**
	konfiguracja paneli informacyjnych
**/

// liczba paneli
#define panelNumber 8

// wyglad paneli
QString panelLook[panelNumber]=
	{	"[#%u][, %f] %r [- %d] [ (%i)]", 
	
		"<body hspace=\"0\" vspace=\"0\"><table width=\"210\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">"
			"<tr valign=\"top\"><td><table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"> "
			"<tr valign=\"top\"><td rowspan=2 valign=top width=\"50\"><img width=\"50\"  align=\"left\" valign=\"top\" src=\"$KADU_SHARE/themes/icons/default/abouticon.png\" height=\"50\"></td> "
			"<td align=\"right\" width=\"100%\" valign=\"top\">[<font size=\"7\" color=\"Purple\" face=\"Creature\"><B>: %u :</B></font>]</td></tr> "
			"<tr valign=\"top\"><td align=\"right\" width=\"100%\" valign=\"top\">[<font size=\"-1\" color=\"#00007f\">IP: %i:%p</font>] "
			"[<font size=\"-1\" color=\"#005500\"><i><br>%v</i></font>]</td></tr></table><table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"> "
			"<tr valign=\"top\"> [<td  width=\"95\"  valign=\"top\">%g</td>] [<td align=\"center\" width=\"100%\"  valign=\"middle\"> "
			"tel:<font color=\"#555500\">  %m</font></td>]</tr> </table><hr> [<FONT size=\"+2\"color=\"#d10000\"><b>%f </b></FONT>][<FONT size=\"+2\"color=\"#d10000\"><b>%r</b></FONT>] "
			"[<BR><FONT size=\"-1\"color=\"#d10000\"><b>%n</b></FONT>] [<hr><table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"><tr><td> <font color=blue>%d</font></td></tr></table>] "
			"[<hr><font color=blue>e-mail: %e.</font>][<hr><font size=\"-1\"color=red><I>You are not on the list%o</I></font>] "
			"</td></tr></table></body>", 
	
		"<table><tr><td><img width=\"50\" height=\"63\" src=\"{~/.gg/zdjecia/%u.jpg ~/.gg/zdjecia/brak.png}\"> "
			"</td><td> <div align=\"left\"> [%u][<br>%a][<br>%e][<br>tel.: %m] [<br>IP: %i][<br>d: %v] "
			"[<br>%oYou are not on the list] </div> </td> </tr> </table> <hr> %s [<br>%d]",
	
		"<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"$KADU_SHARE/themes/icons/default/personalinfo.png\"></td><td> <div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i][<br>%oYou are not on the list] </div></td></tr></table> <hr> <b>%s</b> [<br>%d]",

		"<TABLE WIDTH=\"100%\" BORDER=\"0\" CELLPADDING=\"0\" ALIGN=\"CENTER\" BGCOLOR=\"gray82\">"
			"<TR>[<TD COLSPAN=\"2\"><font color=gray35>%d</font></TD>]</TR><TR> "
			"[<TD WIDTH=\"75\" ALIGN=\"RIGHT\" BGCOLOR=\"gray69\"><font color=black size=2 name=verdana>%s</font></TD> "
			"<TD ALIGN=LEFT WIDTH=\"100%\" BGCOLOR=\"gray77\"><font color=black size=2>%u</font></TD>] "
			"</TR> <TR> [<TD WIDTH=\"75\" ALIGN=\"RIGHT\" BGCOLOR=\"gray77\"><font color=black size=2 name=verdana>e-mail</font></TD> "
			"<TD ALIGN=LEFT WIDTH=\"100%\" BGCOLOR=\"gray69\"><font color=black size=2>%e</font></TD>]</TR> "
			"<TR>[<TD WIDTH=\"75\" ALIGN=\"RIGHT\" BGCOLOR=\"gray69\"><font color=black size=2 name=verdana>tel.</font></TD> "
			"<TD ALIGN=LEFT WIDTH=\"100%\" BGCOLOR=\"gray77\"><font color=black size=2>%m</font></TD>]</TR> "
			"<TR> [<TD ALIGN=CENTER COLSPAN=\"2\" ><font color=gray35>%i:%p</font></TD>] </TR> <TR> "
			"[<TD ALIGN=CENTER COLSPAN=\"2\" ><font color=gray35>%v</font></TD>] </TR> </TABLE>",
			
		"<TABLE WIDTH=\"100%\" BORDER=\"0\" CELLPADDING=\"0\" ALIGN=\"CENTER\" BGCOLOR=\"gray82\">"
			"<TR> [<TD WIDTH=\"80\" ALIGN=\"RIGHT\" BGCOLOR=\"gray69\"><font color=black size=2px name=verdana>%s</font></TD> "
			"<TD ALIGN=LEFT WIDTH=\"100%\" BGCOLOR=\"gray77\"><font color=black size=2px name=verdana>%u</font></TD>] "
			"</TR> </TR> <TR>[<TD WIDTH=\"75\" ALIGN=\"RIGHT\" BGCOLOR=\"gray69\"><font color=black size=2 name=verdana>tel.</font></TD> "
			"<TD ALIGN=LEFT WIDTH=\"100%\" BGCOLOR=\"gray77\"><font color=black size=2>%m</font></TD>]</TR> "
			"<TR> [<TD WIDTH=\"75\" ALIGN=\"RIGHT\" BGCOLOR=\"gray77\"><font color=black size=2 name=verdana>e-mail</font></TD> "
			"<TD ALIGN=LEFT WIDTH=\"100%\" BGCOLOR=\"gray69\"><font color=black size=2>%e</font></TD>] "
			"<TR>[<TD COLSPAN=\"2\"><font color=gray35>%d</font></TD>]</TR> <TR> [<TD ALIGN=CENTER COLSPAN=\"2\" ><font color=gray35>%i</font></TD>] "
			"</TR> <TR> [<TD ALIGN=CENTER COLSPAN=\"2\" ><font color=gray35>%v</font></TD>] <TR> </TR> </TABLE> "
			"[%o<font color=red size=3 name=verdana>You are not on the list</font>]",
		"<body background=\"$KADU_SHARE/modules/data/config_wizard/ronk2/tlo.jpg\" hspace=\"0\" vspace=\"0\"><table width=\"160px\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"><tr valign=\"top\"><td width=\"160px\"><table width=\"160px\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"><tr valign=\"top\"><td  valign=top width=\"50px\"><img width=\"50px\" align=\"left\" valign=\"top\" src=\"{$HOME/fotki/%u.png $HOME/fotki/%u.png}{~$HOME/fotki/%u.png $KADU_SHARE/modules/data/config_wizard/ronk2/tux.png}\"></td><td align=\"right\" width=\"110px\" valign=\"top\"><table width=\"105px\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">[<tr valign=\"middle\"><td width=\"5px\"><img  valign=\"middle\" src=\"$KADU_SHARE/modules/data/config_wizard/ronk2/%s.png\"></td><TD widht=\"105px\" align=\"left\"><font size=\"4\" color=\"Purple\"  face=\"Flubber\"><B>%u</B></font></td></tr>][<tr valign=\"middle\"><td width=\"5px\"><img  valign=\"middle\" src=\"$KADU_SHARE/modules/data/config_wizard/ronk2/mobile.png\"></td><td align=\"left\" width=\"105px\" valign=\"middle\"><font color=\"#555500\">%m</font></td></tr>]</table></td></tr></table>[<table width=\"160px\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"><tr valign=\"middle\" ><td width=\"5px\"><img  valign=\"middle\" src=\"$KADU_SHARE/modules/data/config_wizard/ronk2/message.png\"></td><TD widht=\"105px\" align=\"left\"><font color=blue>%e</font></TD></tr></table>]<hr width=\"100%\">[<FONT size=\"+2\"color=\"#d10000\"><b>%f </b></FONT>][<FONT size=\"+2\"color=\"#d10000\"><b>%r</b></FONT>][<BR><FONT size=\"-1\"color=\"#d10000\"><b>%n</b></FONT>][<hr width=\"100%\"><table width=\"160px\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"><tr><td width=\"160px\"><div align=\"justify\"><font color=blue>%d</font></div></td></tr></table>][<hr width=\"100%\"><table width=\"160px\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"><tr valign=\"middle\" ><td width=\"5px\"><img  valign=\"middle\" src=\"$KADU_SHARE/modules/data/config_wizard/ronk2/Blokuje.png\"></td><TD widht=\"155px\" align=\"left\"><font size=\"-1\"color=red><I>You are not on the list%o</I></font></TD></tr></table>]</td></tr></table></body>",
		"<body background=\"#4f4f4f\"><table width=234 cellpadding=0 cellspacing=0 border=0><tr><td align=right>[<font color=\"green\">][UIN :: %u][<font>]</td></tr>[<tr><td align=right><font color=\"green\">MAIL :: <a href=\"mailto:%e\">%e</a><font></td></tr>][<tr><td align=right><font color=yellow>IP :: %i: %p</font></td></tr>][<tr><td align=right><font color=orange>TEL :: +48:%m</font></td></tr>]</table><hr><b>[<font color=\"green\">]%f %r[</font>]</b>[<hr><font color=yellow><i>%v</i></font>][<hr><font color=orange>%d</font>]"
	};
	
// nazwy wygladow (albo autorzy)
QString panelLookName[panelNumber]=
	{	"Default", "RonK", "joi", "dorr", "maz", "Abaddon", "RonK2", "Ciuciu"	};


/**
	konfiguracja skladni dymkow
**/

// liczba dymkow
#define hintsNumber 2

// skladnie dymkow
QString hintLook[hintsNumber]=
	{ 	"<b>%n</b> changed status to <b>%s</b>",
		"<b>%n</b> status ::: <b>%s</b>[<br> desc ::: <i>%d</i>]"
	};

// nazwy skladni
QString hintLookName[hintsNumber]=
	{	"Default",
		"Ciuciu"
	};


/** 
	konfiguracja kolorow dymkow
**/

//liczba kolorow
#define hintColorNumber 4


//kolory dymkow. skladnia: bc, fc
//bc - background color, fc - font color
QString hintColors[hintColorNumber][2]=
	{	"#f0f0f0", "#000000", 
		"#ffffff", "#000000", 
		"#2b2b2b", "#aa0000",
		"#000080", "#FFFFFF"
	};

//nazwy kolorow
QString hintColorsNames[hintColorNumber]=
	{	"Default",
		"Blck and white",
		"Dark grey",
		"Dark blue"
	};


/**
	konfiguracja kolorow kadu
**/
#define colorNumber 3

//kolory kadu. skladnia: chat_my_bc, chat_my_fc,chat_usr_bc, chat_usr_fc, info_panel_bc, info_panel_fc, userbox_bc, userbox_fc
//bc - background color, fc - font color
QString colors[colorNumber][8]=
	{	"#e0e0e0", "#000000", "#f0f0f0", "#000000", "#e6e7e6", "#000000", "#ffffff", "#000000",
		"#444444", "#c7d2ff", "#000839", "#d8d8d8", "#505050", "#aaff00", "#000000", "#b2b3ff",
		"#00007F", "#FFFFFF", "#AAFFFF", "#000000", "#000080", "#FFFFFF", "#000080", "#FFFFFF"
	};

QString colorsNames[colorNumber]=
	{	"Default",
		"Dark grey",
		"Dark blue"
	};
