#! /usr/bin/perl

	# urlの./.cgi?   → image_num = 0 ←を = で区切って配列@image_numに格納
	# $image_num[0]=image_num $image_num[1]=0みたいな感じ
	# $input=$ENV{'QUERY_STRING'};
	# @image_num=split(/=/, $input);
$input = $ENV{'QUERY_STRING'};
@image_num = split(/=/, $input);

print ("Content-type: text/html\n\n");

print ("<html><body>\n");
print ("<h1><strong>類似画像検索</strong></h1>\n");
print("<strong>【インターセクション】</strong>");
#******************他のページのリンク*********************************************************
print ("<p>");

print ("<a href=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/rgb_color_histgram.cgi?image_num=58\">rgb_color_histgram</a>\n");

print ("<a href=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/luv_color_histgram.cgi?image_num=94\">luv_color_histgram</a>\n");

print ("<a href=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/hsv_color_histgram.cgi?image_num=0\">hsv_color_histgram</a>\n");

print ("<a href=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/gabor.cgi?image_num=95\">gabor</a>\n");

print ("</p>"); 
#******************ここまで*********************************************************
print ("<p><strong>クエリ類似</strong></p>");
#　　　　　　　　　　　　!特徴量ファイルの変更!
open(FH,"< sample_gabor0.txt");
$histgram_size=24*1*1;
$i=0;

while($line = <FH>){

    for($j = $histgram_size; $j>0; $j--){
		$line =~ s/$j://;
    }

    @temp = split(/ /, $line);
    
    $histgram_sum=0;

    for($j=0; $j<$histgram_size; $j++){
        $image_data[$i][$j] = $temp[$j];
		$histgram_sum += $temp[$j];
    }
    
    for($j=0; $j < $histgram_size; $j++){
	    $image_data[$i][$j] /= $histgram_sum;
	    # print("$image_data[$i][$j]\n");
    }

    $i++;
    # print("\n");
}
close(FH);

# 画像数
$image_num = 100;
# 類似度計算の変数の初期化
	for($i=0; $i < $image_num; $i++){ 
	   $ruiji[$i] =0; 
	}
	
    $No = $image_num[1];
#	for($i=0; $i < $image_num; $i++){
#    	for($j=0; $j < $histgram_size; $j++){
#        	# 類似度の計算   
#            if($image_data[$i][$j] < $image_data[$No][$j]){
#        	$ruiji[$i] += $image_data[$i][$j];
#        	}else{
#        	$ruiji[$i] += $image_data[$No][$j];
#        }
#    }
#}
	for($i=0; $i < $image_num; $i++){
		for($j=0; $j < $histgram_size; $j++){
		$ruiji[$i] += sqrt(($image_data[$i][$j]-$image_data[$No][$j])*($image_data[$i][$j]-$image_data[$No][$j]));
	}
}

	for($i=0; $i < $image_num; $i++){
    	$max=0.0;
        $k=0;
    		for($j=0; $j < $image_num; $j++){
				if($ruiji[$j] >= $max){
	    			$max = $ruiji[$j];
	    			$k = $j;
				}			
    		}
        $ruiji[$k]=-1.0;
    
        printf ("<a href=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/testtest.cgi?image_num=$k\"><img src=\"http://ryosukeuec.s602.xrea.com/kadai/kadai2b/imgdata/$k.jpg\" width=\"120\" height=\"100\"></a>\n");
    
        if($i==0){ 
            printf ("<hr><br>\n"); 
            print ("<p><strong>類似画像一覧</strong></p>");
        }
}

print ("</body></html>\n");









