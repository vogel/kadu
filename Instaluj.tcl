#!/bin/bash
################################################################################
# Ten program jest wolnym oprogramowaniem. Mo¿na go rozprowadzaæ i modyfikowaæ #
# zgodnie z licencj± GNU General Public License opublikowan± przez             #
# Free Software Foundation w wersji 2 z pó¼niejszymi zmianami.                 #
#                                                                              #
#                                       Pawe³ Salawa (Googie) boogie@arkada.pl #
################################################################################

# Na wstepie, bash sprawdzi czy mamy pakiet TK:
#\
if [ -z `which wish` ]; then
#\
echo
#\
echo "-----"
#\
echo "Nie mozna odnalezc programu wish w zmiennej PATH."
#\
echo "Prawdopodobnie TK (badz tez caly TCL) nie jest zainstalowany."
#\
echo "Z regoly obydwa pakiety znajduja sie na plycie instalacyjnej"
#\
echo "kazdego systemu unixowego, wiec latwo mozna je doinstalowac."
#\
exit 1
#\
fi
# Jesli jest, to uruchaiamy go:
#\
exec wish "$0" "$@"

### I teraz juz TCL/TK :)
set scriptversion 1.0.1

### Sprawdzanie kompatybilnosci TCL/TK:
if {$tk_version < 8.3} {
    toplevel .err
    set winheight 80
    set winwidth 300
    set geomx [expr [expr [winfo screenwidth .] - $winwidth] / 2]
    set geomy [expr [expr [winfo screenheight .] - $winheight] / 2]
    wm maxsize .err $winwidth $winheight
    wm minsize .err $winwidth $winheight
    wm geometry .err +$geomx+$geomy
    wm title .err "Uwaga!"
    label .err.txt -text "Wersja TCL/TK: $tk_patchLevel\nWymagana >= 8.3\nMozliwe jest niestabilne zachowanie instalatora." -font "Helvetica -12 bold"
    button .err.bt -text "OK" -bd 1 -command {destroy .err; set ok 1}
    pack .err.txt .err.bt -side top
    vwait ok
}

### Wersja kadu:
set fd [open configure r]
gets $fd data
gets $fd data
set kadu_ver [string range [lindex $data end] 0 end-1]
close $fd

### Inicjalizacja
set winheight 260
set winwidth 400
set geomx [expr [expr [winfo screenwidth .] - $winwidth] / 2]
set geomy [expr [expr [winfo screenheight .] - $winheight] / 2]
wm maxsize . $winwidth $winheight
wm minsize . $winwidth $winheight
wm geometry . +$geomx+$geomy
wm title . "Instalator Kadu v$scriptversion"

### Slonko :)
if {![info exists IIK]} {
# ^^^^^ jesli instalator startuje z internetowego instalatora,
# to nie trzeba kreaowac obrazka 2 raz.

image create photo kadu -format gif -data {
R0lGODlhZABJAOcAAAICAubaBrqyBoJ+Bu4mCmpmAlJOAlYSBkJCBjIyAiYm
AhoaAuYmCpaOBureBhYWAsK6Bg4OAu7iBvLmBl5eXgYGAvaqBs7CBpqSBkIK
AlZWVmJCBvbqBqKaBnJuBkJCQjY2NtbOBk4aBqaeBk5OTnYSBq6KBl5aAvru
BnZyBrKqBt62BgoKAvYqCjo2AoaCBi4uLjIyMtrSBmoSBuo+CkpKAh4eAlpW
An56Bv7yBi4uBqqiBj4+Bv42Cl4SBhoOCvoqCt7WBnpyBs7GBhoaGhYWFhIG
BgoKBhISAo6KBrquBi4qAmZiBlZSAiIiAnJqBu4uCkZGAoIWBtLGBsK2Bm5q
Ajo6Anp2BmJaAoqGBgYGBsq+Bq6mBtbKBrauBg4ODmYSBtrOBiYiAuLWBv7u
BhYWBt7SBoJ6Br62BioqBn52Ao6GBsa+BhYSAmZeAhIOAhISBr6yBm5mAjIu
AgoKClZSBoJ6DkZCAh4aAhoWAvLiBnZuAk5KAv4qCpKKBj46ArKmBkZGRjo6
Om4WBl5WBoZ+BjYyBiomAiIeAkI+AkpGAm5qCkI+CloSBmISBkZCBk5KBlpW
BubaCiYmBjo2Bn52BnZuBq6mCtbKClZSClJOBrKqCiIiBhISCg4OBhYWCiYi
BiIeBmpmBl5aBmJaBo6GCkI+BkpGBhoaBnpyCm5qBjo6BrauChIOBm5mBjIu
Bh4aBj46Bv//////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////////////////////yH5BAEKAP8ALAAAAABkAEkAAAj+AP8JHEiwoMGD
CBMqXMiwocOHECNKnEixosWLGDNOBABAo8ePEul0BEmy5EGOIzNqMcmyoBYA
KzFybEnz38uZF1/SqdkS5UWYAHbyZAm0IsqYQ00WpYgyZVKSHJFGhPnyaUKq
FnFKPGoV4U2fW50+bNpVIVCtNcmWvRoVrVK1a71q0SJSakm4cdmC9Xp2btOm
dgW+9JuXIdCqLr/6jeChceMUsfwiJri08EKqKQdzXIJAVYgwYczkGE1aQhgI
jxIhQXtTpluQHCn8jdBkxBjSuHPnptKERcrXU/ey/ArARYopupMrz9GFE/CI
msXejdpmh4Pl2HVLelFB+tijgWH+z60QagcE3Hog1KkTRE925ZUqCIV4GC/J
r0lym8Hxl8X597qVAtN3fz2XUVMumIHCaBIAAgosQL3yyCmPtKLJdQCShsIa
R3hX0F9zfaXUSyxgcNtoDmhSwUtvkMJFAKNxUAcAm2SYmw3hgQfigERxxIIA
uHXQlChd6HYGjTbiNgWOH4oU1U2DzVfSTbFwQZoZlHQCQBl1mOGHCmHkdoYW
KuhmxiuGKLGgcgW4FV1faWnRBAejoVDIEhw9YGUONhhyiGikhQGKErpdARSg
yYXh1o4GegRABZ7gZsl4J+DGyVyQAGgoR5EsJ8Eq0jUlZU9aVNDBaGQ8cRMT
E+CGyEv+mb5XSVSaYLfHZHHNRNoYUWFBJ26hcHSKpijFgV0XvhWG02hm6MBR
pbo5B0Cs2cXRIQASZIdEeGvlYAaeACzya26hCEICtdnBwVG2y2HCArdlkYHD
S50gqock7GYBAwCsZFgGRxgql0KjVknC0RHGkhYJAIdg4m0qAEiSoboABJzc
wPB2RbEpuWUCkw0OT4DBmu8JeUiry03qYVkcvSIxbm4AdUWSo0kygh/YSSIG
rnmhlF9uHDzCUSzs0pydGYNZJhhHiOJ2iSccPfIygGFwUkp2mhD81EuaoKwb
Fae8ZEgDZGQ3gSXOGYIdFf+u3FJdPP4DExPYBQ3UIg4nN0H+JR6/ROhyNXA0
aloFdlRUdhxAEhQAX5ByySU7XGLCBp9oiRIkJOsGcdw81QeY3Lpm5wAjZ7V1
FEeoLHLBche0IRxNgxHnk08AZt1XWy89UkiYy23R9kqEtfQmz3KvBGAhg5VA
gBSNHOAIAQRAYUF2otBBmJOcgzRYEdlT1hGADrTC0SBA9GH++TSssNwEhPj1
m4hQRaXBQiJlkqEdQDFwvvk9iBCxckITDma0NhU6YO8yMMlbdi7hpAOcrwU+
+IEWluC1mvmhE+4zCFVaoxGUyCZjA+EIJdzzHkNwJAMt6EMLZsCRTzRtO6/4
C0Jw57aHRAcic5kEgFiBEh/0wRHeKPEAbioBCh2ZBTwyySB9glKJEymHDPh7
iRFQ8gg0dIAIRfgCDRsCmBpepipePAlHQrGIKVQQNxxwxSe+UoROWK5A9IFS
GGc4R73QixKQiEQhBFKIStiBEKSLCgi0GCLiEQiEZkEkFxlVyJsE4gONJCBD
OLgVRdrwL9g7zL7qqDQccrKT8ZMkKO/zOeh8cpQEcZ8hDTMXVG5kgBNJmiuX
GLxXWnKWAjlLVl6HSzp2Lzi/7OWHVglMYSLwlGK8pSuDGRZRonJwuwyKMWf4
kUIic5rAJCY2O1jLbXpzIQEBADs=
}}

### Lokalizacja przegladarki www
if {![info exists IIK]} {
    # ^^^ to samo co ze slonkiem.
    foreach browser {konqueror galeon mozilla netscape} {
        catch {exec which $browser} res
        if {[llength $res] == 1} {
            set webbrowser $res
            break
        }
    }
    if {![info exists webbrowser]} {
        foreach browser {links lynx} {
            catch {exec which $browser} res
            if {[llength $res] == 1} {
                set webbrowser "[exec which xterm] -fg grey -bg black -e $res"
                break
            }
        }
    }
}

### Kolorystyka
set themeopts "-background grey87 -activebackground white -foreground black -activeforeground black"

###########
### Zmienne

global kdedir qtdir installdir var cwin pad
set cwin 1
if {[info exists env(QTDIR)]} {
    set qtdir $env(QTDIR)
} else {
    set qtdir /usr/lib/qt
}
if {[info exists env(KDEDIR)]} {
    set kdedir $env(KDEDIR)
} else {
    set kdedir /opt/kde
}
set installdir /usr
set var(makedoc) 1
set var(makedeb) 0
set var(makessl) 1
set var(libgadu) 0
set var(makekde) 1
set var(default) 1
set var(os) "other"
set maxwins 1

#########
### Dymki
proc tips {w msg} {
    bind $w <Enter> "after 1000 \"tips_aux %W [list $msg]\""
    bind $w <Leave> "after cancel \"tips_aux %W [list $msg]\"
                     after 100 {catch {destroy .tips_help}}"
}

proc tips_aux {w msg} {
    set t .tips_help
    catch {destroy $t}
    toplevel $t
    wm overrideredirect $t 1
    if {$::tcl_platform(platform) == "macintosh"} {
     unsupported1 style $t floating sideTitlebar
    }
    pack [label $t.l -text $msg -relief raised -bd 1 -bg lightyellow] -fill both
    set x [expr [winfo rootx $w]+6+[winfo width $w]/2]
    set y [expr [winfo rooty $w]+6+[winfo height $w]/2]
    wm geometry $t +$x\+$y
    bind $t <Enter> {after cancel {catch {destroy .tips_help}}}
    bind $t <Leave> "catch {destroy .tips_help}"
}


###############
### Glowne okno

frame .l -relief raised -borderwidth 1
frame .r
frame .r.u
frame .r.d
frame .l.c
eval button .l.c.img -image kadu -bd 0 -command {"catch {exec $webbrowser http://kadu.net &}"} $themeopts
label .l.c.txt -text "Kadu\nv$kadu_ver"

pack .r -side right -fill both -expand yes
pack .l -side left -fill y
pack .l.c -side left
pack .l.c.img -side bottom
pack .l.c.txt -side top
pack .r.u -side top -fill both -expand yes
pack .r.d -side bottom -fill x

label .r.d.status -padx 0.5c
tips .r.d.status "Liczba bierzacego kroku,\nw stosunku do wszystkich krokow."
eval button .r.d.close -text "Zaniechaj" -borderwidth 1 -command exit $themeopts
eval button .r.d.next -text {"Dalej >>"} -borderwidth 1 -command next $themeopts
eval button .r.d.prev -text {"<< Wstecz"} -borderwidth 1 -command prev -state disabled $themeopts

pack .r.d.next .r.d.prev .r.d.status -side right
pack .r.d.close -side left

bind . <Escape> exit



### Okno 1 (Instalacja domyslna lub reczna)
set w .r.u.$maxwins
set pad($w) 3c
frame $w
frame $w.1
frame $w.2
radiobutton $w.1.def -text "Zainstaluj Kadu z domyslnymi parametrami." -variable var(default) -value 1 -bd 1 -selectcolor "blue"
radiobutton $w.2.def -text "Sam wybiore potrzebne opcje." -variable var(default) -value 0 -bd 1 -selectcolor "blue"
foreach b "$w.1.def $w.2.def" {
    eval $b configure $themeopts
}
pack $w.1 $w.2 -side top -fill x
pack $w.1.def -side left
pack $w.2.def -side left
tips $w.1.def "Wszystkie ustawienia beda\nwybrane domyslnie. Przejdziesz\nautomatycznie do ostatniego kroku."
tips $w.2.def "Bedziesz mogl wybrac opcje,\nktore najbardziej Ci odpowiadaja\nw kolejnych krokach instalacji."



### Okno 2 (Sciezka do katalogu QT)
incr maxwins
set w .r.u.$maxwins
set pad($w) 2.5c
frame $w
label $w.l -text "Katalog QT:"
frame $w.o
entry $w.o.e -width 24 -textvariable qtdir
button $w.o.b -text "Przegladaj" -borderwidth 1 -command {
    set qt_dir [tk_chooseDirectory -initialdir $qtdir -title {Wybierz katalog QT} -mustexist 1]
    if {"$qt_dir" != ""} {
        set qtdir $qt_dir
    }
}
eval $w.o.b configure $themeopts
frame $w.f
button $w.f.find -text "Znajdz automatycznie" -bd 1 -command {
    set tmp [findQT /usr]
    if {"$tmp" != "" && "$tmp" != "$qtdir"} {
        set qtdir $tmp
    }
}
pack $w.o.b $w.o.e -side right
pack $w.l $w.o -side top
tips $w.o.b "Kliknij aby znalezc i wybrac\nodpowiedni katalog"



### Okno 3 (Sciezka do katalogu KDE)
incr maxwins
set w .r.u.$maxwins
set pad($w) 1.5c
frame $w

set ww $w.check
frame $ww
checkbutton $ww.c -variable var(makekde) -selectcolor blue -text "Mam zainstalowane KDE" -command {EnDisKde $var(makekde)}
pack $ww.c
eval $ww.c configure $themeopts

set ww $w.kde
frame $ww
label $ww.l -text "Katalog KDE:"
frame $ww.o
entry $ww.o.e -width 24 -textvariable kdedir
button $ww.o.b -text "Przegladaj" -borderwidth 1 -command {
    set kde_dir [tk_chooseDirectory -initialdir $kdedir -title {Wybierz katalog KDE} -mustexist 1]
    if {"$kde_dir" != ""} {
        set kdedir $kde_dir
    }
}
eval $ww.o.b configure $themeopts

pack $ww.o.b $ww.o.e -side right
pack $ww.l $ww.o -side top
pack $w.check -side top
pack $w.kde -side top -pady 0.5c
tips $ww.o.b "Kliknij aby znalezc i wybrac\nodpowiedni katalog"

proc EnDisKde {sw} {
    foreach path {.r.u.4.kde.l .r.u.4.kde.o.e .r.u.4.kde.o.b} {
        if {$sw} {
            $path configure -state normal
        } else {
            $path configure -state disabled
        }
    }
}



### Okno 4 (Katalog instalacji)
incr maxwins
set w .r.u.$maxwins
set pad($w) 2.5c
frame $w
label $w.l -text "Katalog instalacji:"
frame $w.o
entry $w.o.e -width 14 -textvariable installdir
button $w.o.b -text "Przegladaj" -borderwidth 1 -command {
    set install_dir [tk_chooseDirectory -initialdir $installdir -title {Wybierz katalog instalacji} -mustexist 1]
    if {"$install_dir" != ""} {
        set installdir $install_dir
    }
}
eval $w.o.b configure $themeopts
pack $w.o.b $w.o.e -side right
pack $w.l $w.o -side top
tips $w.o.b "Kliknij aby znalezc i wybrac\nodpowiedni katalog"



### Okno 5 (Dodatkowe opcje)
incr maxwins
set w .r.u.$maxwins
set pad($w) 1c
frame $w
foreach {path varname text} {deb makedeb "Kompiluj z debugowaniem" doc makedoc "Zainstaluj dokumentacje" \
ssl makessl "Obsluga szyfrowania SSL" lib libgadu "Zlinkuj z istniejaca\nbiblioteka libgadu"} {
    set ww $w.$path
    frame $ww
    checkbutton $ww.c -variable var($varname) -selectcolor blue -text "$text"
    eval $ww.c configure $themeopts
    pack $ww.c -side left
    pack $w.$path -side top -fill x -pady 2
}
tips $w.deb "Jesli Kadu napotka krytyczny blad\ni nieoczekiwanie zakonczy dzialanie,\nto ta opcja spowoduje, ze bedzien\nmozna latwo zlokalizowac usterke\ni wyslac raport do autorow."
tips $w.doc "Zainstaluje dokumentacje Kadu,\naby byla ona dostepna w kazdym momencie."
tips $w.ssl "Dzieki tej opcji bedzie\nmozliwe szyfrowanie rozmow."
tips $w.lib "Instalator nie bedzie kompilowal biblioteki\nlibgadu od nowa, lecz wykozysta isteniejaca\n(musisz byc pewien, ze masz juz te biblioteke)."



### Okno 6 (Podsumowanie i instalacja)
incr maxwins
set w .r.u.$maxwins
frame $w
scrollbar $w.s -command "$w.txt yview" -bd 1
text $w.txt -wrap word -width 37 -height 12 -yscrollcommand "$w.s set"
$w.txt tag configure smallfont -font "helvetica 8"
pack $w.txt -fill both -side left
pack $w.s -side right -fill y



### Procedury instalacyjne
proc install {} {
    upvar #0 kdedir kdedir qtdir qtdir installdir installdir var var maxwins mw env env
    .r.d.next configure -state disabled
    .r.d.prev configure -state disabled
    set p .r.u.progress
    label $p
    pack $p -pady 6 -side top
    set args ""
    if {!$var(makedoc)} {
        lappend args --disable-doc
    }
    if {$var(makedeb)} {
        lappend args --with-debug
    }
    if {!$var(makessl)} {
        lappend args --without-openssl
    }
    if {$var(libgadu)} {
        lappend args --with-existing-libgadu
    }
    set env(KDEDIR) "$kdedir"
    set env(QTDIR) "$qtdir"
    append env(LD_LIBRARY_PATH) ":$qtdir/lib"
    
    $p configure -text "Status:\nKonfiguracja: ./configure --prefix=$installdir\n$args"
    set fd [open "|./configure --prefix=$installdir $args" r]
    fileevent $fd readable "Install $fd"
    vwait var(ins)
    if {[string match {*Run make now*} $var(ins)]} {
        $p configure -text "Status:\nBlad krytyczny!\nSprawdz ostatnie informacje\nz okna powyzej."
        vwait forever
    }
    unset var(ins)
    
    $p configure -text "Status:\nKompilacja... (make)"
    set fd [open "|make" r]
    fileevent $fd readable "Install $fd"
    vwait var(ins)
    if {[string match {make[1]: * * `*`} $var(ins)]} {
        $p configure -text "Status:\nBlad krytyczny!\nSprawdz ostatnie informacje\nz okna powyzej."
        vwait forever
    }
    unset var(ins)
    
    if {"$env(USER)" == "root"} {
        $p configure -text "Status:\nKopiowanie plikow... (make install)"
        wm title . "Kopiowanie plikow... $scriptversion"
        set fd [open "|make install" r]
        fileevent $fd readable "Install $fd"
    } else {
        toplevel .info
        set winheight 134
        set winwidth 380
        set geomx [expr [expr [winfo screenwidth .] - $winwidth] / 2]
        set geomy [expr [expr [winfo screenheight .] - $winheight] / 2]
        wm maxsize .info $winwidth $winheight
        wm minsize .info $winwidth $winheight
        wm geometry .info +$geomx+$geomy
        wm title .info "Instalacja"
        if {$var(libgadu)} {
            label .info.txt -text "Nie masz uprawnien administratora. Zaloguj sie jako
administrator (komenda: su), wejdz do katalogu
kadu ([pwd]) i wykonaj komende
'make install'."
            set winheight 84
            set geomx [expr [expr [winfo screenwidth .] - $winwidth] / 2]
            set geomy [expr [expr [winfo screenheight .] - $winheight] / 2]
            wm maxsize .info $winwidth $winheight
            wm minsize .info $winwidth $winheight
            wm geometry .info +$geomx+$geomy
        } else {
            label .info.txt -text "Nie masz uprawnien administratora. Zaloguj sie jako
administrator (komenda: su), wejdz do katalogu
kadu ([pwd]) i wykonaj komende
'make install'. Nastepnie edytuj plik /etc/ld.so.conf
i dodaj do niego linie (o ile takiej nie zawiera):
$installdir/lib
i na koniec wykonaj komende: ldconfig" -justify left
        }
        button .info.bt -text "  OK  " -bd 1 -command {set ok 1; destroy .info}
        pack .info.txt .info.bt -side top
        bind .info <Return> {set ok 1; destroy .info}
        vwait ok
    }
    $p configure -text "Status:\nInstalacja zakonczona."
    .r.d.close configure -text "Zamknij"
}

proc Install {fd} {
    upvar #0 maxwins mw var var
    if {[gets $fd data] != -1} {
        .r.u.$mw.txt insert end "$data\n" smallfont
        if {"$data" != ""} {
            set var(lastdata) "$data"
        }
        if {"[.r.u.$mw.s activate]" == ""} {
            .r.u.$mw.txt see end
        }
    } else {
        if {[eof $fd]} {
            fileevent $fd readable {}
            catch {close $fd}
            set var(ins) $var(lastdata)
        }
    }
}
proc next {} {
    upvar #0 cwin cwin var var installdir installdir kdedir kdedir qtdir qtdir maxwins mw pad pad
    pack forget .r.u.$cwin
    if {$cwin == 1 && $var(default)} {
        set cwin $mw
    } else {
        incr cwin
    }
    if {[info exists pad(.r.u.$cwin)]} {
        eval pack .r.u.$cwin -pady $pad(.r.u.$cwin)
    } else {
        pack .r.u.$cwin
    }
    update
    if {$cwin == $mw} {
        .r.d.next configure -text Instaluj -command {install; .r.d.prev configure -state disabled}
        .r.u.$mw.txt delete 0.0 end
        .r.u.$mw.txt insert end "Kadu zostanie zainstalowane w: $installdir\n\nKatalog QTDIR: $qtdir\n\n"
        if {$var(makekde)} {
            .r.u.$mw.txt insert end "Katalog KDE: $kdedir\n\n"
        } else {
            .r.u.$mw.txt insert end "Elementy KDE nie zostana zainstalowane.\n\n"
        }
        set args ""
        if {!$var(makedoc)} {
            lappend args --disable-doc
        }
        if {$var(makedeb)} {
            lappend args --with-debug
        }
        if {!$var(makessl)} {
            lappend args --without-openssl
        }
        if {$var(libgadu)} {
            lappend args --with-existing-libgadu
        }
        .r.u.$mw.txt insert end "Dodatkowe opcje konfiguracji:\n"
        foreach arg "$args" {
            .r.u.$mw.txt insert end "$arg\n"
        }
        update
    }
    if {$cwin > 1} {
        .r.d.prev configure -state normal
    }
    .r.d.status configure -text "\[$cwin/$mw\]"
}
proc prev {} {
    upvar #0 cwin cwin var var maxwins mw pad pad
    pack forget .r.u.$cwin
    if {$cwin == $mw && $var(default)} {
        set cwin 1
    } else {
        incr cwin -1
    }
    if {[info exists pad(.r.u.$cwin)]} {
        eval pack .r.u.$cwin -pady $pad(.r.u.$cwin)
    } else {
        pack .r.u.$cwin
    }
    if {$cwin < $mw} {
        .r.d.next configure -text "Dalej >>" -command next
    }
    if {$cwin == 1} {
        .r.d.prev configure -state disabled
    }
    .r.d.status configure -text "\[$cwin/$mw\]"
}
eval pack .r.u.1 -pady $pad(.r.u.1)
.r.d.status configure -text "\[$cwin/$maxwins\]"

### KDE 3.x tego potrzebuje do sprawnego dzialania :)
pack propagate . false


