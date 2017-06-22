"""
Script to convert an .fnt (font) file exported from tools like
BMFont and Glyph Designer to a .json format which we can use
to render fonts at runtime
"""
import sys
import os
import json
import re

class RawJson(unicode):
    pass

# patch json.encoder module to avoid escaping strings
# so that they are copied verbatim in the resuling json file
for name in ['encode_basestring', 'encode_basestring_ascii']:
    def encode(o, _encode=getattr(json.encoder, name)):
        return o if isinstance(o, RawJson) else _encode(o)
    setattr(json.encoder, name, encode)

regex = re.compile("([a-zA-Z0-9]+)=((\"[^\"]*\")|(\S+))")

def line_to_json(line):
    """
    Convert a line from the .fnt file to json
    """
    result = {}
    expressions = line.strip().split(' ')
    iterator = iter(expressions)
    next(iterator)
    for expression in iterator:
        if len(expression) == 0:
            continue

        props = regex.findall(line)
        for prop in props:
            value = prop[1]
            if ',' in value:
                value = '[%s]' % value
            elif value == '"""':
                value = '"\\""'
            elif value == '"\\"':
                value = '"\\\\"'
            result[prop[0]] = RawJson(value)
    return (expressions[0], result)

def main(argv):
    if len(argv) != 2:
        raise ValueError('Usage: fnt_to_json input.fnt output.json')

    # Read the .fnt file into memory
    data = []
    with open(argv[0]) as f:
        data = f.readlines()

    output = {
        'chars': {},
        'kernings': {}
    }

    # Convert to json line by line
    for line in data:
        if line.startswith('chars') or line.startswith('kernings'):
            continue
        elif line.startswith('char'):
            (name, result) = line_to_json(line)
            output['chars'][str(result['id'])] = result
        elif line.startswith('kerning'):
            (name, result) = line_to_json(line)
            char_id = str(result['first'])
            if char_id not in output['kernings']:
                output['kernings'][char_id] = {}

            output['kernings'][char_id][str(result['second'])] = result['amount']
        else:
            (name, result) = line_to_json(line)
            output[name] = result


    # Write result
    with open(argv[1], 'w') as f:
        f.write(json.dumps(output))

if __name__ == "__main__":
    main(sys.argv[1:])
