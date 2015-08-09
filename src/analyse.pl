#!/usr/bin/perl
use strict;
use warnings;
use Switch;
use English;

my $method = shift;
my $dir = './debug';
my $dataline;
foreach my $fp (glob("$dir/debug_*")) {
    open (my $fh, "<", $fp) or die "can't read open '$fp': $OS_ERROR";    
    while (<$fh>) { # <$fh> returns false at end of file
        unless (/$method/) {
            break;
        }

        switch($_) {
            case /ETA/ {
                my @timedata = split "\t", $_;
                my $eta = $timedata[1];
                printf "%s", $eta;
            }
            case /ARGS/ {

            }
        }
    }
    close $fh or die "can't read close '$fp': $OS_ERROR";
}
