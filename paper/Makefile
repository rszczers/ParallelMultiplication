PROJECT=parallel_multiplication_algorithms
SOURCE=main
OUTPUT_DIR=./temp/

all:
	latexmk -pdf \
	-bibtex \
	-pdflatex="pdflatex -interactive=nonstopmode" \
	-use-make \
	-output-directory=$(OUTPUT_DIR) \
	-aux-directory=$(OUTPUT_DIR) \
	$(SOURCE).tex
	mv $(OUTPUT_DIR)$(SOURCE).pdf ./$(PROJECT).pdf
	make clean 

clean:
	rm $(OUTPUT_DIR)*	
