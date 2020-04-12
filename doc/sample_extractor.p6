#!/usr/bin/perl6
use v6;

sub MAIN($doc_dir) {
    for dir(test => /^ .* \. cpp$/) -> $file {
        say "Removing " ~ $file;
        unlink($file);
    }

    for dir(test => /^ .* \. out$/) -> $file {
        say "Removing " ~ $file;
        unlink($file);
    }
    
    my Str $listing_name;
    my Str $calls_name;

    my Str $file = $doc_dir ~ '/doc.md';
    say "Extracting from '" ~ $file ~ "'.";

    for $file.IO.lines -> $line {
        if $listing_name and $line ~~ /^^\[\/\/\]\:\#end_cpp_listing_$<name>=[\w+]$$/ {
            $listing_name eq $<name>.Str 
                or die "Unexpected listing name. Expected: '" 
                    ~ $listing_name ~ "' received: '" ~ $<name>.Str ~ "'.";
            say "Created " ~ $listing_name ~ ".cpp";
            $listing_name=Nil;
        }
        if $calls_name and $line ~~ /^^\[\/\/\]\:\#end_calls_$<name>=[\w+]$$/ {
            $calls_name eq $<name>.Str 
                or die "Unexpected calls name. Expected: '" 
                    ~ $calls_name ~ "' received: '" ~ $<name>.Str ~ "'.";
            say "Created " ~ $calls_name ~ ".out";
            $calls_name=Nil;
        }
        if $listing_name {
            unless $line ~~ /^^\`\`\`/ {
                spurt $listing_name ~ ".cpp", $line ~ "\n", :append;
            }
        }
        if $calls_name {
            unless $line ~~ /^^\`\`\`/ {
                spurt $calls_name ~ ".out", $line ~ "\n", :append;
            }
        }
        if !$listing_name and $line ~~ /^^\[\/\/\]\:\#begin_cpp_listing_$<name>=[\w+]$$/ {
            $listing_name = $<name>.Str;
        }
        if !$calls_name and $line ~~ /^^\[\/\/\]\:\#begin_calls_$<name>=[\w+]$$/ {
            $calls_name = $<name>.Str;
        }
    }
}
