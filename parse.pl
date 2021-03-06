#!/usr/bin/perl
use strict;
use warnings;
use English;
use feature "switch";
$ARGV[0] = "CANNON" unless defined $ARGV[0];
my $method = shift;

my $dir = shift;
#my $dir = './debug';

foreach my $fp (glob("$dir/debug_*")) {
    open (my $fh, "<", $fp) or die "cannot open '$fp': $OS_ERROR";
    if(<$fh> =~ /\s$method$/gim) {
        my @data; 
        while (<$fh>) {
            my @tmp = /(\d+\.\d+)|(\d+)/g;
            foreach(@tmp) {
                if((defined $_) and !($_ =~ /^$/)){
                    push @data, $_;
                }
            }
        }
        print join("\t", @data);
        print "\n";
    }
    close $fh or die "cannot close '$fp': $OS_ERROR";
}
