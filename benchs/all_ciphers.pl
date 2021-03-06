#!perl

=head Description

TODO

=cut

use strict;
use warnings;
use feature qw(say);
use autodie qw(open close);
$| = 1; # Disabling output buffering

use Cwd;
use File::Path qw(make_path);
use File::Basename;

my @secure_elements = (
    { name => 'tiny',   l => 2,  s => 2  },
    { name => 'small',  l => 8,  s => 16 },
    { name => 'medium', l => 16, s => 32 },
    { name => 'large',  l => 32, s => 64 }
    );

my @ciphers = qw(AES Ascon Photon TinyJambu);
my @universalize_options = qw(-universal -no-universal);
my $path_to_ciphers = "tests/automated/programs";
my $python = "pypy3";
my $path_to_dump      = "benchs/results/dump_all_ciphers.txt";
my $path_to_compiler  = "compiler/compiler.py";

# Moving to upper directory if inside "bench"
if (getcwd() =~ m{/benchs$}) {
    say "$0... moving one directory up.";
    chdir "..";
}

# Making sure that $path_to_ciphers is valid
if (! -d $path_to_ciphers) {
    die "Cannot find cipher directory (should be located at '$path_to_ciphers')";
}
for my $cipher (@ciphers) {
    if (! -f "$path_to_ciphers/$cipher.c") {
        die "Cannot find cipher $cipher at '$path_to_ciphers/$cipher.c'";
    }
}

# Making sure that $path_to_compiler is valid
if (! -f $path_to_compiler) {
    die "Cannot find compiler (should be located at '$path_to_compiler')";
}

# Creating the results directory if needed
if (! -f $path_to_dump) {
    my (undef, $path) = fileparse($path_to_dump);
    make_path $path;
}

open my $FH, '>>', $path_to_dump;

my $total = @secure_elements * @ciphers * @universalize_options;
my $count = 1;
for my $universal (@universalize_options) {
    say $FH "Universalization: $universal";
    for my $cipher (@ciphers) {
        for my $se (reverse @secure_elements) {
            print "\r$0... Running: $count/$total";
            $count++;

            my $path_to_cipher = "$path_to_ciphers/$cipher.c";

            my ($name, $l, $s) = @$se{qw(name l s)};
            my $r = $l * 3;
            say $FH "$cipher -- $name ($l/$s)";

            my $output = `$python compiler/compiler.py $path_to_cipher -o /tmp/bench_all_ciphers.bc -r $r -lin $l -lout $l -s $s $universal -stats`;
            say $FH $output;
            say $FH "\n\n";
        }
    }
}
print "\r$0... done.                 \n";
