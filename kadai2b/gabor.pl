#!/usr/local/bin/perl

$input = $ENV{'QUERY_STRING'};
@num = split(/=/,$input);

print "Content-type: text/html\n\n";
print "<html><body>\n";
#print "<h3>$num[1]</h3>\n";

#print "<p><a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/rgb.cgi?num=0\">rgb</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/rgb2.cgi?num=0\">rgb2×2</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/rgb3.cgi?num=0\">rgb3×3</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/rgb4.cgi?num=0\">rgb4×4</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/hsv.cgi?num=0\">hsv</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/hsv2.cgi?num=0\">hsv2×2</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/hsv3.cgi?num=0\">hsv3×3</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/hsv4.cgi?num=0\">hsv4×4</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/luv.cgi?num=0\">luv</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/luv2.cgi?num=0\">luv2×2</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/luv3.cgi?num=0\">luv3×3</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/luv4.cgi?num=0\">luv4×4</a>\n";
print "<a href=\"http://www.nicovideo.jp/watch/1382087508\">gabor</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/gabor2.cgi?num=0\">gabor2×2</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/gabor3.cgi?num=0\">gabor3×3</a>\n";
#print "<a href=\"http://img.cs.uec.ac.jp/kohara-y/skkadai5/gabor4.cgi?num=0\">gabor4×4</a></p>\n";

print "<h1>Query Image</h1>\n";

open(FH,"sample_gabor0.txt");
$HIST_SIZE=24*1*1;
$i=0;
while($line=<FH>){
    for($j=$HIST_SIZE;$j>0;$j--){
	$line =~ s/$j://;
    }
    @tmp = split(/ /,$line);
    $sum=0;
    for($j=0;$j<$HIST_SIZE;$j++){
        $data[$i][$j] = $tmp[$j];
	$sum += $tmp[$j];
    }
    for($j=0;$j<$HIST_SIZE;$j++){
	$data[$i][$j] /= $sum;
    }
    $i++;
}
close(FH);

$NUM=100;
for($i=0;$i<$NUM;$i++){ $simil[$i]=0.0; }
$No = $num[1];
for($i=0;$i<$NUM;$i++){
    for($j=0;$j<$HIST_SIZE;$j++){
        if($data[$i][$j] < $data[$No][$j]){$simil[$i] += $data[$i][$j];}
        else                              {$simil[$i] += $data[$No][$j];}
    }
}

for($i=0;$i<$NUM;$i++){
    $max=0.0;$k=0;
    for($j=0;$j<$NUM;$j++){
	if($simil[$j] >= $max){
	    $max = $simil[$j];
	    $k = $j;
	}
    }
    $simil[$k]=-1.0;
    printf "<a href=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/gabor.cgi?num=$k\"><img src=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/imgdata/$k.jpg\" width=\"120\" height=\"100\"></a>\n";
    if($i==0){ printf "<hr><br>\n"; }
}
print "</body></html>\n";
