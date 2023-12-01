ci: cInsertion.c coordReader.c
	gcc -fopenmp -std=c99 -O3 cInsertion.c coordReader.c -o ci.exe -lm

fi: fInsertion.c coordReader.c
	gcc -fopenmp -std=c99 -O3 fInsertion.c coordReader.c -o fi.exe -lm

comp: ompcInsertion.c coordReader.c
	gcc -fopenmp -std=c99 -O3 ompcInsertion.c coordReader.c -o comp.exe -lm

fomp: ompfInsertion.c coordReader.c
	gcc -fopenmp -std=c99 -O3 ompfInsertion.c coordReader.c -o fomp.exe -lm

icomp: ompcInsertion.c coordReader.c
	icc -qopenmp -std=c99 -O3 ompcInsertion.c coordReader.c -o icomp.exe -lm

ifomp: ompfInsertion.c coordReader.c
	icc -qopenmp -std=c99 -O3 ompfInsertion.c coordReader.c -o ifomp.exe -lm

clean:
	rm -f *.exe	
