
SOURCE=slide.md
TARGET=output.pdf
FONTSIZE=10pt

all:
	pandoc $(SOURCE) \
		-o $(TARGET)   \
		--latex-engine=lualatex \
		-t beamer \
		-V theme:Slankdev01 \
		-V colortheme:default \
		-V fontsize:$(FONTSIZE) \
		-H h-lualatexja.tex


