//------------------------------------------------------------------------------ 
/// Copyright (c) WAGO GmbH & Co. KG 
/// 
/// TODO: license terms
///
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------ 
/// \file make-xml-config.c
/// 
/// \version $Revision: 1.2 $ 
/// 
/// \brief Altered version of linux kernel's menu-based configuration tool.
/// Constructs an XML representation of the configuration tools menu which may
/// be parsed by the configuration frontends (either web or console based)
/// 
/// \author Roman Zippel <zippel@linux-m68k.org>
/// \author Andrej Gantvorg : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

/*
 * Copyright (C) 2002 Roman Zippel <zippel@linux-m68k.org>
 * Released under the terms of the GNU GPL v2.0.
 */

// CHANGELOG:
// 2012-04-16: v1.0:  - initial version
// 2012-04-19  v1.1:  - silblings of unchecked nodes are no longer ignored 
//                    - TraverseMenu refactored
//                    - Error output to stderr instead of stdout
// 2012-05-03  v1.2:  - Meaningful error message for unchecked config keys
// 2013-05-10  v1.3:  - Generate string-based uids based on ptxdist's target
//					   variables from the .in file 
// 2013-11-15  v1.4:  - Print fatal parsing errors to stderr instead of stdout
// 2019-08-28  v1.5:  - Print all unset keys instead of aborting on the first
// occurence
//


//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------

// No includes should be placed before <Python.h> as it redefines standard
// macros!
#include <Python.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define LKC_DIRECT_LINK
#include "lkc.h"

#include "python_c_glue.h"
#include "conf.h"

input_mode_t input_mode = oldaskconfig;

int indent = 1;
int valid_stdin = 1;
int sync_kconfig = 0;
int conf_cnt = 0;
char line[128];
struct menu *rootEntry = NULL;

//-- Function: DumpXMLRepresentation ------------------------------------------- 
/// 
/// This function is called when the complete menu tree has been parsed.
/// It causes the XML representation of the menu to be written out to stdout.
///
/// \param pyRootNode: menu root
/// \return 
//------------------------------------------------------------------------------

static void DumpXMLRepresentation(PyObject *poRootNode)
{
  //TODO: error handling
  PyObject *poRes;
  PyObject *poMainModule = PyImport_AddModule(PY_MAIN_MODULE);
  PyObject *poGlobalDict = PyModule_GetDict(poMainModule);
  PyObject *poPostProcessFn = PyDict_GetItemString(poGlobalDict, PY_POSTPROCESS_FN);
  poRes = PyObject_CallFunction(poPostProcessFn, "");

  if(poRes == NULL)
  {
    //TODO: cleanup in case of an error
    fprintf(stderr, "make-xml-config: Error in postprocess routine\n");
    exit(1);
  }
  // either NULL or Py_None
  Py_XDECREF(poRes);

  PyObject *poStdout = PySys_GetObject("stdout");

  poRes = PyObject_CallMethod(poRootNode, "export", "(Oi)", poStdout, 0);
  if(poRes == NULL)
  {
    //TODO: cleanup in case of an error
    fprintf(stderr, "make-xml-config: Error exporting the XML description\n");
    exit(1);
  }

  Py_DECREF(poStdout);
  // either NULL or Py_None
  Py_XDECREF(poRes);

  // we are finished -> release the python tree root's reference
  Py_DECREF(poRootNode);
}

//-- Function: TraverseMenu ---------------------------------------------------- 
/// 
/// Recursively traverse the menu tree
///
/// \param node:      Current top node
/// \param poParseFn: python function from the glue layer to create the node's 
/// python representation.
/// \param poParent:  python representation of the parent node
/// \param configEntryPrefix: this unique prefix tags all config tool menu entries
/// in the ptxdist configuration files.
/// 
/// \return 
//------------------------------------------------------------------------------

void TraverseMenu(struct menu *node, 
                  PyObject *poParseFn, 
                  PyObject *poParent,
                  char *configEntryPrefix) 
{
  char *nodeName = "";
  PyObject *poNode = Py_None;
  PyObject *poParentRef = Py_None;

  if(!node->sym || !node->sym->name) 
  {
    // don't process symbolless/unnamed nodes, jump to their silblings and
    // children immediately
    goto recursion;
  }

  if (sym_get_tristate_value(node->sym) != yes) 
  {
    // ignore unchecked symbols and their children
    // look at their silblings, though
    goto recursion_silblings;
  }

  nodeName= node->sym->name;

  if((strstr(nodeName, configEntryPrefix) == nodeName)) 
  {
    // node name has the proper prefix -> process XML data

    // The xml object description is located in node->help;
    // We append the target variable name (sth like [CT_MENU_]FOO_BAR_BAZ) to it
    // because it is unique and can be used as a uid
    char *help = node->help;
    char *uidVal = nodeName + strlen(configEntryPrefix);

    int sizeofXmlRepr = strlen(help) + strlen(" uid=") + strlen(uidVal) + 1;
    char *xmlRepr = malloc(sizeofXmlRepr);

    if(NULL == xmlRepr)
    {
      printf("Error during memory allocation when parsing node %s.\n", nodeName);
      exit(1);
    }

    // BUGME: why do we do this?
    if(help[strlen(help)-1] == '\n')
    {
      help[strlen(help)-1] = '\0';
    }

    snprintf(xmlRepr, sizeofXmlRepr, "%s uid=%s", help, uidVal);
   
    PyObject *poArgs = Py_BuildValue("(sO)", xmlRepr, poParent);

    // poNode is a new reference => we have to decref it after usage
    poNode = PyObject_CallObject(poParseFn, poArgs);

    if(poArgs)
    {
      Py_DECREF(poArgs);
    }

    if(poNode == NULL || poNode == Py_None)
    {
      fprintf(stderr, "make-xml-config: ");
      fprintf(stderr, "Error processing node %s in file %s(line %d)\n", 
              nodeName,
              node->file->name, 
              node->lineno);
      exit(1);
    } 
    else if (poParent != Py_None)
    { // not the root node
      // is also referenced by parent -> can be released here
      Py_DECREF(poNode);
    } 
    else 
    {
      // save for a decref on cleanup
      poParentRef = poNode;
    }
  } // if(strstr(nodeName, configEntryPrefix) == nodeName) 

recursion:
  
  // traverse children...
  if(node->list)
  {
    TraverseMenu(node->list, poParseFn, poNode, configEntryPrefix);
  }

recursion_silblings:

  // ...and the silblings
  if(node->next)
  {
    TraverseMenu(node->next, poParseFn, poParent, configEntryPrefix);
  }

  if( poParent == Py_None && nodeName &&  
      strstr(nodeName, configEntryPrefix) == nodeName)
  {
    // we are the root node -> finish him :)
    DumpXMLRepresentation(poParentRef);      
  }
}  
//-- Function: GetFullGlueFilePath --------------------------------------------- 
/// 
/// Helper function used to construct an absolute path of the python glue layer
/// file. It is assumed that this file resides in the same directory as the
/// main binary itself.
///
/// \param buf      The character buffer to store the path in.
/// \param bufSize  The buf's size.
/// 
/// \return true if the path could be constructed, false on errors
//------------------------------------------------------------------------------


static bool GetFullGlueFilePath(char *buf, size_t bufSize)
{
  size_t exePathLength = 0;
  char *exeName = NULL;

  exePathLength = readlink("/proc/self/exe", buf, bufSize);
  
  // no error, no truncation by readlink
  if(exePathLength != -1 && exePathLength < bufSize)
  {
    buf[exePathLength] = '\0'; // readlink doesn't terminate result
    exeName = strrchr(buf, '/') + 1;

    if(strlen(buf) - strlen(exeName) + strlen(PYTHON_C_GLUE_FILE) < bufSize)
    {
      strncpy(exeName, PYTHON_C_GLUE_FILE, (bufSize - strlen(buf) - 1));
      return true;
    }
    else
    {
      fprintf(stderr, "make-xml-config: path to glue file too long: %s\n", buf);
      return false;
    }

  }
  else
  {
    fprintf(stderr, "make-xml-config (readlink): %s\n", strerror(errno));
    return false;
  }
}

//-- Function: find_unset_symbols ----------------------------------------------
/// 
/// Parse the configuration and detect all unset symbols. Actually this is a
/// dumbed-down version of conf.c:check_conf
///
/// \param menu Root of the menu structure to analyze.
/// 
/// \return found
//------------------------------------------------------------------------------


static bool find_unset_keys(struct menu *menu)
{
  struct symbol *sym;
  struct menu *child;
  bool found = false; 

  if (!menu_is_visible(menu))
    return false;

  sym = menu->sym;
  
  if (sym && !sym_has_value(sym)) {
    fprintf(stderr, "Checking ptxconfig: %s has no value assigned!\n", sym->name); 
    found = true;
  }

  for (child = menu->list; child; child = child->next) {

    // only change value from false to true
    if(find_unset_keys(child)) {
       found = true;
    }
  }

  return found;
}

//-- Function: main ------------------------------------------------------------ 
/// 
/// Main function
///
/// \param argc
/// \param argv
/// 
/// \return 
//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  // TODO: sane default values for release version
  char *menuFile = "./test.in";
  char *configFile = "./ptxconfig";
  char *configEntryPrefix = "CT_MENU_";
  int opt = 0;

  static struct option long_opts[] = {
    {"configfile", required_argument, 0, 'c'},
    {"menufile", required_argument, 0, 'm'},
    {"prefix", required_argument, 0, 'p'},
    {0,0,0,0},
  };

	while ((opt = getopt_long(argc, argv, "m:c:p:", long_opts, NULL)) != -1) {
		switch (opt) {
    case 'm':
      menuFile = optarg;
      break;
    case 'c':
      configFile = optarg;
      break;
    case 'p':
      configEntryPrefix = optarg;
      break;
  	}
	}

  FILE *expFile = NULL; 
  char glueFileFullPath[PATH_MAX];
  int ret = 0;

  PyObject *poMainModule;

  PyObject *poGlobalDict;
  PyObject *poParseFn = Py_None;

	input_mode = silentoldconfig;
  sync_kconfig = 1;

 	conf_parse(menuFile);
	conf_read(configFile);
	
  valid_stdin = isatty(0) && isatty(1) && isatty(2);

  // The original kconfig code checks here for config keys with unset values by
  // calling check_conf from within a loop. Unset keys appear when someone
  // extends the distribution's rule files but doesn't check in the updated
  // ptxconfig file. In this case, the original code exits with a (not very
  // verbose) error message and 'ptxdist go' aborts giving the user no clue
  // about the reasons.

  // We want to make as few changes as possible to the original code in case
  // make-xml-config will have to be ported to a newer kconfig version. That's
  // why we use an own dumbed-down version of check_conf to check for the same
  // condition and to exit in a more graceful way.
 
	if(find_unset_keys(&rootmenu))
  {
    fprintf(stderr, "\nmake-xml-config: Error in configuration: ");
    fprintf(stderr, "New unset config keys found (see above).\n");
    fprintf(stderr, "Update the ptxconfig file by calling 'ptxdist ");
    fprintf(stderr, "menuconfig' and saving the changes.\n");
    exit(EXIT_FAILURE);  
  }

  // XML/Python-related stuff follows

  Py_NoSiteFlag = 1; // get rid of "import site failed" with host-python
  Py_Initialize();

  if(GetFullGlueFilePath(glueFileFullPath, sizeof(glueFileFullPath)))
  {
    expFile = fopen(glueFileFullPath, "r");
  }

  if(expFile)
  {

    ret = PyRun_SimpleFile(expFile, glueFileFullPath);

    if(ret != 0)
      goto cleanup;

    // mainModule, globalDict, parseFn are all borrowed references
    // Python's borrowed references don't have to be decref'ed

    poMainModule = PyImport_AddModule(PY_MAIN_MODULE);

    if(!poMainModule)
      goto cleanup;

    poGlobalDict = PyModule_GetDict(poMainModule);

    if(!poGlobalDict)
      goto cleanup;

    poParseFn = PyDict_GetItemString(poGlobalDict, PY_PARSE_FN);

    if(!poParseFn)
      goto cleanup;

    TraverseMenu(&rootmenu, poParseFn, Py_None, configEntryPrefix); 
  } 
  else 
  {
    fprintf(stderr, "make-xml-config: Error opening %s (pwd=%s, expFile=%p)\n", 
            glueFileFullPath, 
            get_current_dir_name(), 
            expFile);
  }

cleanup:
  if(expFile) fclose(expFile);
  
  Py_Finalize();


  return 0;

}

