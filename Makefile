# "Makefile" for yascm Scheme Interpreter.
# Copyright (C) 2015 Hmgle <dustgle@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

TARGET = yascm

all:: $(TARGET)

yascm: yascm_flex.l yascm_bison.y yascm.h yascm.c
	bison -d yascm_bison.y -o yascm_bison.tab.c
	flex -o yascm_flex.lex.c yascm_flex.l
	cc -O3 yascm_bison.tab.c yascm_flex.lex.c yascm.c -o $@

test:: yascm
	./yascm < tests/tests.scm

clean::
	-rm -f $(TARGET) *.o *.tab.h *.tab.c *.lex.c
