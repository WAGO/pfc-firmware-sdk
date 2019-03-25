/*****************************************************************************/
/*! \defgroup CIFX_TOOLKIT cifX DPM Toolkit
*
*  The cifX DPM Toolkit allows users to write their own drivers
*  accessing the dualport directly.
*                                                                            */
/*****************************************************************************/

/*****************************************************************************/
/*! \defgroup CIFX_TK_STRUCTURE Toolkit Structure Definitions
* \ingroup CIFX_TOOLKIT
*
* Defines Device instance/ChannelInstance structures and internal data
* storage
*                                                                            */
/*****************************************************************************/

/*****************************************************************************/
/*! \defgroup CIFX_TOOLKIT_FUNCS cifX DPM Toolkit specific functions
*   \ingroup CIFX_TOOLKIT
*
*   Defines cifX DPM Toolkit specific functions, e.g. Initialize Toolkit,
*   or add cards to toolkit management
*
*   Example for Toolkit initialization
*   \code
*  long lRet = cifXTKitInit();
*
*  if(CIFX_NO_ERROR == lRet)
*  {
*    PDEVICEINSTANCE ptDevInstance = (PDEVICEINSTANCE)OS_Memalloc(sizeof(*ptDevInstance));
*    OS_Memset(ptDevInstance, 0, sizeof(*ptDevInstance));
*    ptDevInstance->fPCICard          = 0;
*    ptDevInstance->pvOSDependent     = NULL;
*    ptDevInstance->pbDPM             = <insert pointer to DPM>;
*    ptDevInstance->ulDPMSize         = <insert size of DPM>;
*    OS_Strncpy(ptDevInstance->szName,
*               "cifX0",
*               sizeof(ptDevInstance->szName));
*
*    // Add the device to the toolkits handled device list
*    lRet = cifXTKitAddDevice(ptDevInstance);
*
*    // If it succeeded do device tests
*    if(CIFX_NO_ERROR == lRet)
*    {
*       // Work with the device
*    }
*  }
*  cifXTKitDeinit();
*   \endcode                                                                 */
/*****************************************************************************/


/*****************************************************************************/
/*! \defgroup CIFX_TK_DPMSTRUCTURE DPM Structure Definition
*   \ingroup CIFX_TOOLKIT
*
*   Defines dualport structures, flags
*                                                                            */
/*****************************************************************************/

/*****************************************************************************/
/*! \defgroup CIFX_TK_HARDWARE Hardware Access
* \ingroup CIFX_TOOLKIT
*
* The hardware access functions service the dualport flags,
* memory areas.
*                                                                            */
/*****************************************************************************/

/*****************************************************************************/
/*! \defgroup CIFX_DRIVER_API cifX Driver API implementation
*   \ingroup CIFX_TOOLKIT
*
*   The API implementation services the hardware access functions according
*   to the function definition
*                                                                            */
/*****************************************************************************/

/*****************************************************************************/
/*! \defgroup CIFX_TK_OS_ABSTRACTION Operating System Abstraction
*   \ingroup CIFX_TOOLKIT
*
*   Abstracts the O/S dependent functions to be used by the toolkit.
*   This part needs to be implemented by the user, according to his O/S.
*                                                                            */
/*****************************************************************************/

/*****************************************************************************/
/*! \defgroup CIFX_TK_USER User specific implementation
*   \ingroup CIFX_TOOLKIT
*
*   User specific implementation of storing/resolving card configuration
*   files, etc. These functions are called in the toolkit and MUST be
*   implemented by the user.
*                                                                            */
/*****************************************************************************/
