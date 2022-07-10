import sys
import os
import getopt
import pathlib
from css_html_js_minify import process_single_html_file, process_single_js_file, process_single_css_file, html_minify, js_minify, css_minify

search_text = "__LOCATION__"
replace_text = "location"

outfilename = 'convert.txt'
outfilemin = ()
outfile = ()

def wrap(filename, data):
    # Create raw text variable named: filename with '.' replaced by '_'
    tmp = 'const char * %s = R"rawtext(\n' % (filename.replace('.', '_'))
    return tmp + data + ')rawtext";\n\n'


def convert(filename):
    # Open text file in read only mode
    with open(filename, 'r') as file:
        data = ''
        excluding = False
        Lines = file.readlines()
        
        count = 0

        for line in Lines:
            # read the file line by line, excluding the EXCLUDE sections
            count += 1
            if line.find('__BEGIN_EXCLUDE__') >= 0: excluding = True
            if not excluding :
                # Append it to data, replacing replacement text on the fly
                data += line.replace(search_text, replace_text)
            if line.find('__END_EXCLUDE__') >= 0: excluding = False

    # Write the replaced content to outfile
    outfile.write(wrap(filename, (data)))

    root, ext = os.path.splitext(filename)
    basname = os.path.basename(filename)
    final = data
    # Minimize certain file types 
    if ext == '.js':
        # js_minify sometimes sticks a ';' in front of else and catch
        # fix it up.
        final = wrap(basname, (js_minify(data).
          replace(';else','else').replace(';catch', 'catch')))
    elif ext == '.css':
        final = wrap(basname, (css_minify(data)))
    elif ext == '.html':
        final = wrap(basname, (html_minify(data)))
    else:
        print('WARNING - usupported file type: ' + ext)

    # Write the minimized content to outfilemin
    outfilemin.write(final)

# process command line arguments
arglist = sys.argv[1:]

# Options
options = "hmo:"

# Long options
long_options = ["Help", "My_file", "Output="]

try:
    # Parsing argument
    args, values = getopt.getopt(arglist, options, long_options)
    
    # checking each argument
    for arg, argv in args:

        if arg in ("-h", "--Help"):
            print ("TODO Display Help")
            
        elif arg in ("-o", "--Output"):
            print (("Output file: % s") % (argv))
            outfilename = argv

    outfile = open(outfilename, 'w')
    root, ext = os.path.splitext(outfilename)
    minname = root + '.min' + ext
    print (("Minimized output file: % s") % (minname))
    outfilemin = open(minname, 'w')
    for value in values:
        print (("Converting: % s") % (value))
        convert(value)
            
except getopt.error as err:
    # output error, and return with an error code
    print (str(err))
