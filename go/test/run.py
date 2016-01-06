# Copyright (c) 2016, Ben de Waal
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * The names of the contributors may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
import os

CUR = os.path.abspath(os.path.dirname(__file__))
TMP = os.path.join(CUR,"..","temp")

try: os.mkdir(TMP)
except: pass

files = sorted([ f for f in os.listdir(CUR) if os.path.isfile(os.path.join(CUR,f)) ])

errors = False
for file in files:
    # skip files that are not tests
    if file[0] < '0' or file[0] > '9': continue
    if file.endswith("-expected"): continue

    # set up
    input = os.path.join(CUR,file)
    output = os.path.join(TMP,file)
    expected = input+"-expected"

    # execute test
    os.system("../bin/go '%s' > '%s'" % (input,output))

    # diff against expected
    if os.system("diff -b '%s' '%s'" % (expected,output)):
        print "FAILED"
        errors = True

if not errors:
    print "ALL TESTS PASSED"
