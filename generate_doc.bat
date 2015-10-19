doxygen Doxyfile
cd doc\latex
pdflatex refman.tex > refman.1.log
pdflatex refman.tex > refman.2.log
bibtex refman
pdflatex refman.tex > refman.3.log
copy /Y refman.pdf ..\refman.pdf