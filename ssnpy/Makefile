# MIT License
# Copyright (c) 2017 Susanow
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.



def:
	@echo "Usage: make [OPTIONS]"
	@echo "OPTIONS"
	@echo "  install      install ssnpy (need root permit)"
	@echo "  uninstall    uninstall ssnpy (need root permit)"
	@echo "  clean        clean tmp files (need root permit)"

install:
	./setup.py install
	cp misc/ssnctl /usr/local/bin

uninstall:
	./setup.py install --record /tmp/file.txt
	cat /tmp/file.txt | xargs rm -rf
	rm /usr/local/bin/ssnctl

clean:
	rm -rf build
	rm -rf dist
	rm -rf susanow.egg-info
	rm -rf __pycache__

