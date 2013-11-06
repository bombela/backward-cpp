
mdfiles:=\
		../README.md

htmlfiles:=$(mdfiles:.md=.html)

all: $(htmlfiles)
 
$(mdfiles:.md=.html): $(mdfiles)

%.html: %.md _head.html _foot.html
	redcarpet --parse-fenced_code_blocks $< > $@.body && cat _head.html $@.body _foot.html > $@

serve:
	cd .. && python -m SimpleHTTPServer
