my $file = $ARGV[0];
my $fileComment;
my @table1;
my @table2;

my $class = '';
my $obj = '';
my $doc = '';

open(IN, "Macro/$file");
if (<IN> =~ /\/\/\s*([^\r\n]*)/) {
  $fileComment = "<p>$1</p>\r\n";
}
while (<IN>) {
  if (/\/\*\*\s*(.*)\s*\*\//) {
    $doc = $1;
  } elsif (/\/\*\*\s*([^\r\n]*)/) {
    $doc = $1;
    while (<IN>) {
      if (/\*\//) {
        last;
      } elsif (/\*\s*([^\r\n]*)/) {
        $doc .= ($doc ? ' ' . $1 : $1);
      }
    }
  } elsif (/^class (\w+) \{/) {
    $class = $1;
    $obj = lc $class;
    $doc = '';
  } elsif ($doc eq '') {
    # Do not output.
  } elsif (/^  constructor(\([^\)]*\)) \{/) {
    push(@table1, << "HERE");
<tr>
  <td>import { $class } from \"$file\";<br>new $class$1</td>
  <td>$doc</td>
</tr>
HERE
    $doc = '';
  } elsif (/^  (\w+\([^\)]*)(,\s*\w+)\s*=\s.*\) \{/) {
    push(@table2, << "HERE");
<tr>
  <td>$obj.$1)<br>$obj.$1$2)</td>
  <td>$doc</td>
</tr>
HERE
    $doc = '';
  } elsif (/^  (\w+\([^\)]*\)) \{/) {
    push(@table2, << "HERE");
<tr>
  <td>$obj.$1</td>
  <td>$doc</td>
</tr>
HERE
    $doc = '';
  } elsif (/^function (\w+)\((\w+)\s*=\s.*\) \{/) {
    push(@table1, << "HERE");
<tr>
  <td>import { $1 } from \"$file\";<br>$1()<br>$1($2)</td>
  <td>$doc</td>
</tr>
HERE
    $doc = '';
  } elsif (/^function (\w+)(\([^\)]*\)) \{/) {
    push(@table1, << "HERE");
<tr>
  <td>import { $1 } from \"$file\";<br>$1$2</td>
  <td>$doc</td>
</tr>
HERE
    $doc = '';
  }
}
close(IN);

my $head = '';
my $foot = '';

open(IN, "Help/macroref.html");
while (<IN>) {
  if (/<h3>$file<\/h3>/) {
    last;
  }
  $head .= $_;
}
while (<IN>) {
  if (/<\/table>/) {
    last;
  }
}
while (<IN>) {
  $foot .= $_;
}
close(IN);

$table = join('', sort(@table1)) . join('', sort(@table2));
$table =~ s/\n/\r\n/g;

open(OUT, "> Help/macroref.html");
print OUT $head;
print OUT "<h3>$file</h3>\r\n";
print OUT $fileComment;
print OUT "<table>\r\n";
print OUT $table;
print OUT "</table>\r\n";
print OUT $foot;
close(OUT);
