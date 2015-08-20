PROJECT=parallel_multiplication_algorithms
SOURCE=main.tex
OUTPUT_DIR=./temp/

all:
	latexmk -pdf \
	-bibtex \
	-pdflatex="pdflatex -interactive=nonstopmode" \
	-use-make \
	-output-directory=$(OUTPUT_DIR) \
	$(SOURCE)
	mv $(OUTPUT_DIR)main.pdf ./$(PROJECT).pdf
