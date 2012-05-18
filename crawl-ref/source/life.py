import sys
import re

data = sys.stdin.read()

# Settings
indent = 4

# Modes: "print", "dot"
mode = "dot"

# Strip out axed monster
data = re.sub("(?ms)#define AXED.*?Axed.*?$", "", data)

# Strip out block comments
data = re.sub("(?ms)\/\*.*?\*\/", "", data)

# Strip other comments
data = re.sub('//.*', '', data)
data = re.sub('#.*', '', data)

# Replace energy:
def energy(x):
    #print "Energy:", x.groups()
    return "(%s, %s)" % (x.group(1), x.group(2))
data = re.sub('([A-Z0-9_-]*_ENERGY)\((.*?)\)', energy, data)

# Replace mrd entries (with placeholder for parens)
def mrd(x):
    #print "mrd:", x.groups()
    return "<<{{%s}},%s>>" % (x.group(1), x.group(2))
data = re.sub('(?ms)mrd\((.*?)(\d*)\s*\)', mrd, data)

# Handle lone mrds that must be wrapped as tuples
def tuplify(x):
    #print "Tuplify:", "\n", data, "\n", "Match:", x.group(0)
    return  "%s(%s)," % x.groups()
data = re.sub('(\s*)(<<.*?>>,)', tuplify, data)

# Replace pipes by commas
def pipes(x):
    #print "Pipes:", "\n", data, "\n", "Match:", x.group(0)
    return "(%s)" % re.sub('(?ms)\s*\|\s*', ',', x.group(0))
data = re.sub('(?m)(\(.*\)|<<.*>>|[A-Z0-9_-]*)(\s*\|\s*(\(.*\)|<<.*>>|[A-Z0-9_-]*))+', pipes, data)

# Stringify bare enums:
def stringify(x):
    return "'%s'" % x.group(1)
data = re.sub("(?<!['\"])([A-Z0-9_-]+)", stringify, data)

# Replace placeholders
data = re.sub('(<<|{{)', '(', data)
data = re.sub('(>>|}})', ')', data)

# Replace }s by )s unless they're a monster glyph.
data = re.sub("}(?!['\"]);*", ')', data)
data = re.sub("{", '(', data)

# Last change needed to make it valid Python:
data = re.sub("static monsterentry mondata\[\] = ", "", data)

# Eval it, storing it as a tuple
mondata = eval(data)

# Titles for the fields, in order, indexed from 0
fields = (
    'id',
    'glyph',
    'color',
    'name',
    'flags',
    'resistances',
    'mass',
    'experience modifier',
    'genus',
    'species',
    'holiness',
    'magic resistance',
    'attacks',
    'hit dice',
    'ac',
    'ev',
    'spellbook',
    'corpse type',
    'zombie type',
    'shout type',
    'intelligence',
    'habitat',
    'flight class',
    'speed',
    'energy',
    'item use',
    'eats',
    'size',
)

# Formatting functions
def resist_dots(resist, level=1):
    level = min(level, 3)
    # HACK:
    if re.search('VUL', resist) is not None:
        return "%s%-20s (%s%s)" % (' ' * indent, resist, "x" * level, "."*(3-level))
    else:
        return "%s%-20s (%s%s)" % (' ' * indent, resist, "*" * level, "."*(3-level))

# Iterate over monsters, building a string as we go.
if mode == 'print':
    str = ""
    for monster in mondata:
        index = 0
        # Ignore these guys. Pointless.
        if monster[0] == 'MONS_TEST_SPAWNER':
            continue
        str += "===%s (%s)===\n" % (monster[3], monster[0])
        for attribute in monster:
            field = fields[index]
            str += "%s: " % field
            if field == 'flags' and isinstance(attribute, tuple):
                    for flag in attribute:
                        str += flag + ", "
            elif field == 'resistances':
                str += "\n"
                # Single resist, single level.
                if not isinstance(attribute, tuple):
                    str += resist_dots(attribute)
                else:
                    # Multiple resists
                    for resistance in attribute:
                        # Single level
                        if not isinstance(resistance, tuple):
                            str += resist_dots(resistance) + "\n"
                        # Multiple levels (possibly for multiple resists)
                        else:
                            # HACK: Cope with extraneous tuples
                            if len(resistance) == 1:
                                resistance = resistance[0]
                            resists, level = resistance
                            for resist in resists:
                                if not isinstance(resist, tuple): # Again, hack.
                                    str += resist_dots(resist, level) + "\n"
                                else:
                                    for r in resist:
                                        str += resist_dots(r, level) + "\n"
            elif field == 'attacks':
                str += "\n"
                for attack in attribute:
                    if isinstance(attack, tuple):
                        str += "%s%s (%s, %s)\n" % (' ' * indent, attack[2], attack[0], attack[1])
            elif field == 'hit dice':
                die = attribute
                str += "%s" % die[0]
                str += "\n"
                str += "hp calc: %dd%d[%+d]%+d" % (int(die[0]), int(die[2])+1, int(die[1]), int(die[3])-int(die[0]))
                str += "\n"
                str += "average hp: %s" % (int(die[0])*(int(die[1])+(float(die[2])+2)/2)+int(die[3])-int(die[0]))
            elif field == 'energy' and isinstance(attribute, tuple):
                    if len(attribute) == 2:
                        str += "%s, %s" % attribute
                    else:
                        str += "It's Complicated"
            else:
                str += "%s" % attribute

            str += "\n"
            index += 1
        str += "\n"
    print str
elif mode == 'dot':
    print "digraph Crawl {"
    for monster in mondata:

    print str
    print "}"


    str = 
    str = "}"
    print str
