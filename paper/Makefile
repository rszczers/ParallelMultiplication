PROJECT=parallel_multiplication_algorithms
SOURCE=main
OUTPUT_DIR=./temp/

all:
	latexmk -pdf \
	-bibtex \
	-pdflatex="pdflatex --shell-escape -interactive=nonstopmode" \
	-use-make \
	-output-directory=$(OUTPUT_DIR) \
	-aux-directory=$(OUTPUT_DIR) \
	$(SOURCE).tex
	mv $(OUTPUT_DIR)$(SOURCE).pdf ./$(PROJECT).pdf
	sleep 1

clean:
	rm -rf $(OUTPUT_DIR)*	
