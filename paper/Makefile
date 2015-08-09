SOURCE = quick.tex
OUTPUT_DIR = ./temp/

all:
	latexmk -pdf \
	-pdflatex="pdflatex -interactive=nonstopmode" \
	-use-make \
	-output-directory=$(OUTPUT_DIR) \
	$(SOURCE)
	mv $(OUTPUT_DIR)quick.pdf ./
