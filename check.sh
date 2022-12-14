execD="./DorianTest/target/release/rush-nowel"
execL="./LouisTest/ft_president"
execN="./NicolaTest/ft_president.sh"

myFolder="retd"
for ((j=1 ; 7 - $j ; j++)) do
	myText="test$j.txt"
	mkdir $myFolder
	mkdir -p $myFolder/$myText
	for ((i=1 ; 21 - $i ; i++))
		do 
		echo "" > $myFolder/$myText/test{$i} &&
		gtimeout 2 $execD $myText $myFolder/$myText/test{$i} $i
		#$execL <$myText >$myFolder/$myText/test{$i} $i 
	done
done
#$execD $myText $myFolder/$myText/test{$i} $i