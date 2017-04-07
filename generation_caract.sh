
#!/bin/bash

cd ./sift/

for element in *

do	
	echo $element  
	sed '1d' $element  >tmp.txt && mv tmp.txt $element
	sed '1d' $element  >tmp.txt && mv tmp.txt $element
	sed '1d' $element  >tmp.txt && mv tmp.txt $element
	
	#echo $line|sed's/<.*>;/"/g'|tr -d ";" >> sample.txt
	i=1;
	while read line
	do	
        if  !((i %150)) ; then 
		echo $line >> "./result/$element"
	fi
	((i=i+1));
	done <$element
	#cat "./result/$element.txt"|sed ' s/<CIRCLE [1-9].*>; //g '|tr -d ";" > "./result2/$element.txt"
done


