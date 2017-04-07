
#!/bin/bash

cd ./sift/

	echo sample.txt|sed's/<.*>;/""/g'|tr -d ";" >> sample2.txt


#sed '/motif/substitut/g'
