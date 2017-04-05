creation_histo: rdjpeg.c proc.c creation_histo.c 
	cc rdjpeg.c proc.c creation_histo.c -o creation_histo

read_image: rdjpeg.c read_image.c
	cc rdjpeg.c read_image.c -o read_image

testproc: cgiu.c proc.c testproc.c
	cc cgiu.c proc.c testproc.c -o testproc
clean: 
	rm creation_histo *.o


