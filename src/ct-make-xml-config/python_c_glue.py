#------------------------------------------------------------------------------ 
## Copyright (c) WAGO Kontakttechnik GmbH & Co. KG 
## 
## TODO: license terms
##
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------ 
## \file python_c_glue.py
## 
## \version $Revision: 1.1 $ 
## 
## \brief Altered version of linux kernel's menu-based configuration tool.
## Constructs an XML representation of the configuration tools menu which may
## be parsed by the configuration frontends (either web or console based)
## This file includes the glue layer between the C code parsing the .in files
## and the python classes generated from the XSD configuration describing the
## layout of the corresponding xml elements.
## 
## \author Andrej Gantvorg : WAGO Kontakttechnik GmbH & Co. KG
#------------------------------------------------------------------------------

#
# CHANGELOG:
# 2012-04-16 v1.0:   - initial veresion
# 2012-04-19 v1.1:   - enhanced error handling with user-friendly error
#                    messages (class SyntaxError)
#                    - support for implicit member notation (foo.bar .baz) via
#                    lastAttr (Needed if several anonyme elements are defined, 
#                    i.e.
#                    page_element.text="x" .url="y"
#                    page_element.text="z" .url="n"
#

# extend path to include xsd_defs which is located locally
import os,sys
#TODO: find a better way
sys.path.insert(0,os.path.abspath(os.path.dirname(os.path.realpath(__file__))))

from xsd_defs import *
import traceback
import shlex
import re

class SyntaxError(Exception):
  def __init__(self, expr, msg):
    self.expr = expr
    self.msg = msg


# global container with all nodes (needed for postprocess)
xml_nodes = []
lastAttr=None

# generateDS.py naming convention: if varName is a list, an add_${varName}
# method is created

def assignVar(parent, varName, value):
  if hasattr(parent, 'add_' + varName):
    getattr(parent, 'add_' + varName)(value)
  elif hasattr(parent,'set_' + varName):
    getattr(parent, 'set_' + varName)(value)
  else:
    parentName = type(parent).__name__
    raise SyntaxError(parentName+'.'+varName+'='+str(value), 
                      'No '+parentName+'.'+varName+' in XSD configuration.')

def getAttrRef(obj, attrName):
  if attrName == '':
    return None
  try: 
    attrValue=getattr(obj, attrName)

    if attrValue == None:
      obj.__dict__[attrName] = globals()[attrName]()
      return obj.__dict__[attrName]
    elif type(attrValue).__name__ == 'list':
      obj.__dict__[attrName].append(globals()[attrName]())
      return obj.__dict__[attrName][-1] 
    else:
      return attrValue 

  except Exception as e:
    objName = type(obj).__name__
    raise SyntaxError(objName+'.'+attrName,
                      'No '+objName+'.'+attrName+' in XSD configuration.')


def process(parent, params, value):
  global lastAttr
  
  if value == '':
    raise SyntaxError(params, 'Assignment value expected.')
  
  # monkey patch a postprocess method into node => just a proof of concept
  # if it turns out that we need this functionality, the solution should
  # be rethought
  if params == 'postprocess':
    fnDef = value.replace(r'\n', '\n')
    #TODO: we use user-provided code here -> need better error handling
    exec(fnDef)
    fnName = re.match('^ *def *(.*) *\(.*\):', fnDef).group(1)
    parent.postprocess = locals()[fnName]

  else: # regular processing

    tokens = params.partition('.')
    varName = tokens[0]
    restOfParams = tokens[2]

    if tokens[1] == '': # no '.' in params means we found the last attr in 
                        # foo.bar.baz... chain => assign parent.varName to value

      assignVar(parent, varName, value)
      return
    else: # params look like foo.bar.baz=x 
      
      # get parent.varName or parent.varName[-1] if there can be more than 
      # one varName
      attr = getAttrRef(parent, varName)

      if attr == None: # implicit notation (   .bar ( stands for next attribute 
                       # for last element))

        if lastAttr == None:
          raise SyntaxError(params, 'No parent found for attribute ' + params)
        else: 
          process(lastAttr, restOfParams, value)

      else: # explicit notation (foo.bar) 

        # store currently processed attr (foo) in lastAttr in case of bar2 
        # below:
        # foo.bar1 = x
        #    .bar2 = y
        lastAttr = attr
        process(attr, restOfParams, value)

def postprocess_dummy(self):
  pass

def parse(params, parent):
  try:
    # no need to hold reference to attribute to resolve implicit .foo in new node
    global lastAttr
    lastAttr = None

    xmlParams = params.partition('__XML:')[2]
    tokens = shlex.split(xmlParams, comments=True)

    # tokens[0] contains the class name
    if tokens[0] in globals():
      newObj_class = globals()[tokens[0]]
    else:
      raise SyntaxError(params, 'Element ' + tokens[0] + ' is not in the XSD definition.')

    newObj = newObj_class() 

    xml_nodes.append(newObj)

    # monkey patch a reference to the parent into newObj
    newObj.__parent = parent
    # now add a postprocess dummy to newObj
    # it is later replaced with a real one, if defined in the config
    # (part of the proof of concept for on-the-fly object
    # extension => alpha functionality, use with care!
    newObj.postprocess = postprocess_dummy

    if parent != None: # we aren't the root node =>
      # connect current node with its parent (parent.[currentNode] = newObj)
      assignVar(parent, tokens[0], newObj)

    for token in tokens[1:]:
      keyValue = token.partition('=') # partition empty => no 'foo.bar.baz = x'-like assignments
                                      # in this case, partition[1] = partition[2] = ''
                                      # Such a situation is nonetheless syntactically legal and
                                      # would produce something like <xml_foo/>
      
      if keyValue[1] != '':
        process(newObj, keyValue[0], keyValue[2])

    return newObj

  except SyntaxError as se:
    print >> sys.stderr, 'Error parsing line'
    print >> sys.stderr
    print >> sys.stderr, se.expr + ':'
    print >> sys.stderr
    print >> sys.stderr, se.msg
    print >> sys.stderr
    return None # is checked against in C code that knows about the file and the line nr.
  
  except Exception as e:
    print >> sys.stderr, 'Exception:'
    #TODO: proper error messages => user shall know what's wrong with his configuration!
    traceback.print_exc()
    sys.exit(1)

def postprocess_all():
  for node in xml_nodes:
    node.postprocess(node)
