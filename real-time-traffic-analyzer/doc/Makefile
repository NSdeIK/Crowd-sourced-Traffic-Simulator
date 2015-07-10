all:	clean validate_xmllint traf-analy-eng.pdf
epub:	clean validate_xmllint traf-analy-eng.epub

traf-analy-eng.pdf: traf-analy-eng.xml traf-analy-eng.xls
	dblatex -T native traf-analy-eng.xml -p traf-analy-eng.xls

traf-analy-eng.epub: traf-analy-eng.xml
	dbtoepub -o traf-analy-eng.epub traf-analy-eng.xml


.PHONY: validate_jing
validate_jing:
	@if [ ! -e jing.jar ]; then \
		wget https://jing-trang.googlecode.com/files/jing-20091111.zip; \
		unzip jing-20091111.zip > /dev/null; \
		mv jing-20091111/bin/jing.jar jing.jar; \
		rm -rf jing-20091111*; \
	fi

	xmllint --xinclude traf-analy-eng.xml --output output.xml
	java -jar jing.jar -c http://docbook.org/xml/5.1CR3/rng/docbookxi.rnc output.xml
	@rm -f output.xml


.PHONY: validate_xmllint
validate_xmllint:
	xmllint --xinclude traf-analy-eng.xml --output output.xml
	xmllint --relaxng http://docbook.org/xml/5.1CR3/rng/docbookxi.rng output.xml --noout
	rm -f output.xml


.PHONY: clean
clean:
	rm -vf traf-analy-eng.pdf traf-analy-eng.epub

