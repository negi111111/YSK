#!/usr/local/bin/perl
print "Content-type: text/html\n\n";

print "<html><body>\n";
print "<h3><strong>類似画像検索</strong></h3>\n";
$input=$ENV{"QUERY_STRING"};
@num=split(/=/, $input);

#******************他のページのリンク*********************************************************
print ("<p>");

print "<a href=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/gabor.cgi?num=0\">gabor</a>\n";

print ("</p>"); 
#ここまで


#******************1~24の特徴量を読み込んで，切り取って，ピクセルを正規化*********************************************************
#sample_gabor0.txtからファイルを読み込む
#<は省略してもOK

open(FH, "<sample_gabor0.txt");
$histgram_size=24*1*1;
$i=0;
#sample_gabor0.txtはこんなのが一行にあるデータなので
#1:1.21018261 2:0.97494529 3:0.89204497 4:1.06087995 5:0.95407576 6:1.01882576 7:1.29455593 8:1.03302235 9:1.03151118 10:1.11488294 11:1.05522238 12:1.08176848 13:1.41920439 14:1.20534918 15:1.17709541 16:1.27629986 17:1.23853880 18:1.30323639 19:1.57382201 20:1.43504519 21:1.40771103 22:1.40347998 23:1.49179099 24:1.53805657 ・・・

while($load_hist=<FH>){
#print("$load_hist\n");
for($j=$histgram_size; $j>0; $j--){
#置換演算子：s/// 
#オプション　s文字列を単一行として扱う
$load_hist=~s/$j://;
print("$load_hist");
#このように変換する
#	1:1.21018261 2:0.97494529
#	1.21018261　0.97494529
}
print("$load_hist");
#print("$load_hist\n");
@image_array=split(/ /, $load_hist);
print("$image_array[0]\n");

#正規化するために初期化しとく
$sum=0;

#image_arrayに格納された特徴量を各画像$image_dataに割り振り
for($j=0; $j<$histgram_size; $j++){
$image_data[$i][$j]=$image_array[$j];
$sum+=$image_array[$j];
}
#print("$image_data[0][0]\n");
#正規化
for($j=0; $j<$histgram_size; $j++){
$image_data[$i][$j]/=$sum;
}


}
close(FH);































print "</body></html>\n";
