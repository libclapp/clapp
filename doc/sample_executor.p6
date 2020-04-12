#!/usr/bin/perl6
use v6;
use Test;

sub exec(Str $comment, Str $cmd, Str $expected_output) {
    my $output = qqx{$cmd};
    is($output, $expected_output.substr(0, *-1), "testing '" ~ $comment ~ "': '" ~ $cmd ~ "'");
}

sub MAIN($out_file) {
    my Str $output;
    my Str $cmd;
    my Str $comment;

    say "Executing '" ~ $out_file ~ "'";
    my $contents = $out_file.IO.slurp;

    for $contents.lines -> $line {
        if $line ~~ /^^\#' '$<comment>=[.+]$$/ {
             if $output and $cmd and $comment {
                exec($comment, $cmd, $output);
                $output = Nil;
                $cmd = Nil;
                $comment = Nil;
            }
            $comment = $<comment>.Str;
        } elsif $line ~~ /^^\$' '$<cmd>=[.+]$$/ {
            $cmd = $<cmd>.Str;
        } else {
            $output ~= $line ~ "\n";
        }
    }

    if $output and $cmd and $comment {
        exec($comment, $cmd, $output);
    }

    done-testing;
}
