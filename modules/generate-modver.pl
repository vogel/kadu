#! /usr/bin/perl -w

open CONFIG, $ARGV[0] or die("Nie moge otworzyc pliki $ARGV[0]");
while(<CONFIG>) {
	chop;
	if(/.*=.*/){
		@option=split '=';
		$options{lc($option[0])}=$option[1];
	}
}
close CONFIG;

open MODVER, ">modver.h" or die("Nie moge otworzyc pliki modver.h");
select MODVER;
print "#ifndef _MODVER_H\n";
print "#define DESCRIPTION \"" . $options{"description"} . "\"\n";
print "#define DESCRIPTION_PL \"" . $options{"description[pl]"} . "\"\n";
print "#define AUTHOR \"" . $options{"author"} . "\"\n";
if($options{"version"} ne "core"){
print "#define VERSION \"" . $options{"version"} . "\"\n";
}
print "#define _MODVER_H\n";
print "#endif\n";
close MODVER
