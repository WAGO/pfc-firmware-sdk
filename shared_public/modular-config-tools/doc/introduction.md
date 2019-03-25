# Introduction to modular-config-tools framework
This is an introduction to modular-config-tools framework to create modular and unit testable config tools.

## Basic configuration for a modular config tool
To create a modular config tool you can just add a new target for an executable to the makefile of this project.
But you are also free to create your own project as well. As an example how to do that you may take a look to the
makefile settings for build target *helloworld.elf* in this project. The *helloworld.elf* config tool is the example
config tool for this project (see [examples page](examples.html "Examples")).
Its key makefile settings are explained now.

### Build settings
To use the framework core functionality the primary library *libconfigtoolutils* are linked statically. In addition the
library *libconfigtoolfail* is linked statically which provides a config tool common implementation for *wc_Fail*,
needed to use the assertion macros from *common-header* project in header file *wc/assertion.h*.

To keep the config tool modular and unit testable, its source modules cannot contain a main function. To use a common,
predefined main function implementation the *helloworld.elf* config tool added the common main source module
[<i>"src/common/main.c"</i>](main_8c.html "Common main source module") to its source list.

### Entry points for a modular config tool
By convention the *ctparts_ctMain* function from [<i>"ctparts/ctmain.h"</i>](ctmain_8h.html "ctmain.h details page")
is the entry point for the config tool core functionality.
All possible parameter option evaluation and resource acquisition should be already done at this point.
Options and resources are shipped in with structures
[<i>ctutil_Options_t</i>](structctutil__Options.html "ctutil_Options_t details page") and
[<i>ctutil_Resources_t</i>](structctutil__Resources.html "ctutil_Resources_t details page").
Both structures are customizable for config tool specific parameter options and resources.

To prepare the *ctparts_ctMain* function (e.g. evaluate parameter options and acquire resources) the main function in
the common main source module [<i>"src/common/main.c"</i>](main_8c.html "Common main source module") calls the
*ctparts_PrepareMain* function from [<i>"ctparts/pmain.h"</i>](pmain_8h.html "pmain.h details page").
This is by convention the entry point for the config tool functionality to prepare options and resources.
This function has to call the specific config tool main function *ctparts_ctMain*.

## Common main and predefined prototypes
As a help to implement the
*ctparts_PrepareMain* function from [<i>"ctparts/pmain.h"</i>](pmain_8h.html "pmain.h details page")
you may use the
*ctutil_CommonMain* function from [<i>"ctutil/common_main.h"</i>](common_main_8h.html "common_main.h details page").

This function takes a function table and a user specific context to run a predefined preparation sequence.
Common resources acquired by calling the function *ctutil_OpenCommonResources* and released afterwards by calling
the function *ctutil_ReleaseCommonResources*. Also the *ctparts_ctMain* function is called in the sequence.
The following function pointers may be additionally part of the sequence and the function table
(optional functions may be NULL in given function table):
- *ctparts_PrintInfoText_ptr* - Prints program information text
  (optional)
- *ctparts_SetOptionDefaults_ptr* - Sets config tool option defaults
  (mandatory, optionally may also allocate a specific options struct)
- *ctparts_ReleaseSpecificOptions_ptr* - Releases config tool specific options
  (optional, mandatory if specific options struct allocated)
- *ctparts_EvaluateEarlyOptions_ptr* - Evaluates options that have to be honored before other options
  (optional)
- *ctparts_EvaluateOptions_ptr* - Evaluates config tool options
  (optional)
- *ctparts_OpenSpecificResources_ptr* - Opens config tool specific resources
  (optional)
- *ctparts_ReleaseSpecificResources_ptr* - Releases config tool specific resources
  (optional, mandatory if *ctparts_OpenSpecificResources_ptr* used)

To implement preparation functions you may use the prototypes defined in
[<i>"ctutil/common_main.h"</i>](common_main_8h.html "common_main.h details page").
To set the function table *ctparts_commonMainFunctions_t* to the predefined defaults the helper function 
*ctutil_SetDefaultCommonMainFunctions* is available.

## Unit tests for a modular config tool
tbd

Core functionality is tested by calling *ctparts_ctMain* function.

Preparations may be tested by calling separated functions if necessary.
