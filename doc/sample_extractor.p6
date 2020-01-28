#!/usr/bin/perl6

sub MAIN($doc_dir) {
    chdir $doc_dir;
    for dir(test => /^ .* \. cpp$/) -> $file {
        say "Removing " ~ $file;
        unlink($file);
    }
    
    my Str $listing_name;
    
    for 'doc.md'.IO.lines -> $line {
        if $listing_name and $line ~~ /^^\[\/\/\]\:\#end_cpp_listing_$<name>=[\w+]$$/ {
            $listing_name eq $<name>.Str 
                or die "Unexpected listing name. Expected: '" 
                    ~ $listing_name ~ "' received: '" ~ $<name>.Str ~ "'.";
            $listing_name=Nil;
        }
        if $listing_name {
            unless $line ~~ /^^\`\`\`/ {
                spurt $listing_name ~ ".cpp", $line ~ "\n", :append;
            }
        }
        if !$listing_name and $line ~~ /^^\[\/\/\]\:\#begin_cpp_listing_$<name>=[\w+]$$/ {
            $listing_name = $<name>.Str;
        }
    }
}
