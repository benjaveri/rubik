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
