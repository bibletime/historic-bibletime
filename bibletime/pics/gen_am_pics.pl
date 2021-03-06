#!/usr/bin/perl -w

use strict;

my $header = << "EOT";
# This Makefile.am was automatically generated by running "perl bibletime/pics/gen_am_pics.pl"
# Please do not edit this file but rather run the script to update
EOT

my @sizes = ("16x16", "22x22", "32x32", "48x48", "64x64");

foreach my $dirname (@sizes) {
	print "Creating $dirname/Makefile.am ...\n";

	opendir(DIR, "$dirname");
	my @icons = grep { /^(lo)|(hi)|(cr)/ && -f "$dirname/$_" } readdir(DIR);
	closedir(DIR);

	open(OUT, ">", "$dirname/Makefile.am");
	print OUT $header;

	#Add EXTRA_DIST stuff
	print OUT "\n\nEXTRA_DIST =";
	foreach my $icon (@icons) {
		print OUT " $icon";
	}


	# Add KDE_ICON stuff
	print OUT "\n\nKDE_ICON =";
	my %inserted_icons;
	foreach my $icon (@icons) {
		$icon =~ s/^(?:hi|lo|cr)\d+-(?:action|app|mime|filesys)-(.+?)\.png/$1/;
		print OUT " $icon" unless (exists $inserted_icons{$icon});
		$inserted_icons{$icon} = 1;
	}

	close(OUT);
}
