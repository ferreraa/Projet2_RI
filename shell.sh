
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
		echo $line >> "sample.txt"
	fi
	((i=i+1));
	done <$element
done


