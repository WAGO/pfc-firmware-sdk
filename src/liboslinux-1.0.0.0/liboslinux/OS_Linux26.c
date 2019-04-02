/**************************************************************************************

 Copyright (c) Hilscher GmbH. All Rights Reserved.

 Adaptations for Linux 2.6
 Copyright (c) 2008 Kontron Modular Computers

 **************************************************************************************

 Filename:
 $Workfile: OS_Linux26.c $
 Last Modification:
 $Author: mathdu-cvs $
 $Modtime: 19.10.06 9:10 $
 $Revision: 4056 $

 Targets:
 Win32/ANSI   : yes
 Win32/Unicode: yes (define _UNICODE)
 WinCE        : no

 Description:
 Win32 OS Abstraction Layer implementation. This MUST not be used in kernel mode
 drivers, but can be used in Userland, or Windows CE.

 Changes:

 Version   Date        Author   Description
 ----------------------------------------------------------------------------------
 1        07.08.2006  MT       initial version
 2		   06.12.2007  MathDu	Changed for Linux 2.6.X

 $Log: OS_Linux26.c,v $
 Revision 1.8  2008/06/09 07:10:26  mathdu-cvs
 Changed copyright in source files

 Revision 1.7  2008/05/19 08:46:18  mathdu-cvs
 Some modifications of demo program and interrupt driven mode.

 Revision 1.6  2008/05/06 14:43:20  mathdu-cvs
 Added current firmwares from Hilscher

 Revision 1.5  2008/04/30 09:47:30  mathdu-cvs
 Beta Version


 **************************************************************************************/

/* Kontron Version Identifier */
char id_OS_Linux26_c[] =
		"KOMVERSION $Id: OS_Linux26.c 4056 2012-08-14 14:20:19Z jan.sondhauss@wago.com $";

/*****************************************************************************/
/*! \file OS_Linux26.c
 *    Linux 2.6.X OS Abstraction Layer implementation.                        */
/*****************************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "OS_Dependent.h"
#include "common/UserTrace.h"
#include "common/common.h"
//#include "os_dependent/netXToolkit.h"
#include "NetxDriver/NetxDriver.h"
#include "NetxDevice/NetxDevice.h"

#include <assert.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>

#define __need_timespec
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <pci/pci.h>

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_OS_ABSTRACTION Operating System Abstraction
 *    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Memory allocation function
 *   \param ulSize    Length of memory to allocate
 *   \return Pointer to allocated memory                                      */
/*****************************************************************************/
void* OS_Memalloc(u32 ulSize)
{
	return malloc(ulSize);
}

/*****************************************************************************/
/*! Memory freeing function
 *   \param pvMem Memory block to free                                        */
/*****************************************************************************/
void OS_Memfree(void* pvMem)
{
	free(pvMem);
}

/*****************************************************************************/
/*! Memory reallocating function (used for resizing dynamic toolkit arrays)
 *   \param pvMem     Memory block to resize
 *   \param ulNewSize new size of the memory block
 *   \return pointer to the resized memory block                              */
/*****************************************************************************/
void* OS_Memrealloc(void* pvMem,
						  u32 ulNewSize)
{
	return  (void*)realloc(pvMem, ulNewSize);
}

/*****************************************************************************/
/*! Memory setting
 *   \param pvMem     Memory block
 *   \param bFill     Byte to use for memory initialization
 *   \param ulSize    Memory size for initialization)                         */
/*****************************************************************************/
void OS_Memset(void* pvMem,
					u8 bFill,
					u32 ulSize)
{
	memset(pvMem, bFill, ulSize);
}

/*****************************************************************************/
/*! Copy memory from one block to another
 *   \param pvDest    Destination memory block
 *   \param pvSrc     Source memory block
 *   \param ulSize    Copy size in bytes                                      */
/*****************************************************************************/
void OS_Memcpy(void* pvDest,
					const void* pvSrc,
					u32 ulSize)
{
	//USER_Trace(NULL, TRACE_LEVEL_INFO, "OS_Memcpy(pvDest=%x, pvSrc=%x, ulSize=%d)", pvDest, pvSrc, ulSize);
	memcpy(pvDest, pvSrc, ulSize);

	/* [JSO] xxx: commented in memcpy and commented out the custom memcpy */
//	{
//		char *s, *d;
//		i32 i = (i32) ulSize;
//		s = pvSrc;
//		d = pvDest;
//
//		// normally both pointers are word-aligned when OS_Memcpy is called
//		// therefore we do a little optimization in this case
//		if (!(((i32) s & 0x3) || ((i32) d & 0x3)))
//		{
//			while (i > 4)
//			{
//				*((i32 *) d) = *((i32 *) s);
//				s += 4;
//				d += 4;
//				i -= 4;
//			}
//		}
//
//		// the remaining bytes and for not word-aligned pointers it is copied byte by byte
//		while (i > 0)
//		{
//			*d++ = *s++;
//			i--;
//		}
//	}
}

/*****************************************************************************/
/*! Compare two memory blocks
 *   \param pvBuf1    First memory block
 *   \param pvBuf2    Second memory block
 *   \param ulSize    Compare size in bytes
 *   \return 0 if both buffers are equal                                      */
/*****************************************************************************/
i32 OS_Memcmp(const void* pvBuf1,
						const void* pvBuf2,
						u32 ulSize)
{
	return memcmp(pvBuf1, pvBuf2, ulSize);
}

/*****************************************************************************/
/*! Move memory
 *   \param pvDest    Destination memory
 *   \param pvSrc     Source memory
 *   \param ulSize    Size in byte to move                                    */
/*****************************************************************************/
void OS_Memmove(void* pvDest,
					 void* pvSrc,
					 u32 ulSize)
{
	memmove(pvDest, pvSrc, ulSize);
}

/*****************************************************************************/
/*! Sleep for a specific time
 *   \param ulSleepTimeMs  Time in ms to sleep for                            */
/*****************************************************************************/
void OS_Sleep(u32 ulSleepTimeMs)
{
	OS_Sleep_us(ulSleepTimeMs * 1000);
}

/*****************************************************************************/
/*! Retrieve a counter based on millisecond used for timeout monitoring
 *   \return Current counter value (resolution of this value will influence
 *           timeout monitoring in driver/toolkit functions)                  */
/*****************************************************************************/
u32 OS_GetMilliSecCounter(void)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	return (u32)( SEC_TO_MSEC(now.tv_sec) + NSEC_TO_MSEC(now.tv_nsec));
}

/*****************************************************************************/
/*! Retrieve a counter based on millisecond used for timeout monitoring
 *   \return Current counter value (resolution of this value will influence
 *           timeout monitoring in driver/toolkit functions)                  */
/*****************************************************************************/
u64 OS_GetMicroSecCounter(void)
{
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	return (u64)( SEC_TO_USEC(now.tv_sec) + NSEC_TO_USEC(now.tv_nsec));
}

/*****************************************************************************/
/*! Create an auto reset event
 *   \return handle to the created event                                      */
/*****************************************************************************/
tOS_Event* OS_CreateEvent(void)
{
	/*return (void*)CreateEvent(NULL, FALSE, FALSE, NULL);*/
	sem_t *pSem;
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_CreateEvent");

	pSem = malloc(sizeof(sem_t));
	if (pSem)
	{
		if (sem_init(pSem, 0, 0) == -1)
		{
			free(pSem);
			pSem = 0;
		}
		else
		{
			// done
		}
	}
	return pSem;
}

/*****************************************************************************/
/*! Set an event
 *   \param pvEvent Handle to event being signalled                           */
/*****************************************************************************/
void OS_SetEvent(tOS_Event* pvEvent)
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "%s (%p)", __func__, pvEvent);

	/*SetEvent((HANDLE)pvEvent);*/
	i32 value = 0;
	sem_getvalue(pvEvent, &value);
	if (value > 0)
	{
		USER_Trace(NULL, TRACE_LEVEL_INFO, "OS_SetEvent %p already set value=%d", pvEvent, value);
	}
	else
		sem_post(pvEvent);
}

/*****************************************************************************/
/*! Reset an event
 *   \param pvEvent Handle to event being reset                               */
/*****************************************************************************/
void OS_ResetEvent(tOS_Event* pvEvent)
{
	/*ResetEvent((HANDLE)pvEvent);*/
	UNUSED( pvEvent);
	/* this is only needed for manual-reset-events which are not available in linux */
	/* -> do nothing */
}

/*****************************************************************************/
/*! Delete an event
 *   \param pvEvent Handle to event being deleted                             */
/*****************************************************************************/
void OS_DeleteEvent(tOS_Event* pvEvent)
{
	/*CloseHandle((HANDLE)pvEvent);*/
	sem_destroy(pvEvent);
}

/*****************************************************************************/
/*! Wait for the signalling of an event
 *   \param pvEvent   Handle to event being wait for
 *   \param ulTimeout Timeout in ms to wait for event
 *   \return 0 if event was signalled                                         */
/*****************************************************************************/
i32 OS_WaitEvent(tOS_Event* pEvent,
							 u32 ulTimeout)
{
	struct timespec ts;
	i32 s;

	UserTraceDebug(TraceFacility_Osal,"%s=%p, %s=%d",_NV(pEvent),_NV(ulTimeout));

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		return CIFX_EVENT_TIMEOUT;
	}
	ts.tv_nsec += ((ulTimeout % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_nsec -= 1000000000;
		ts.tv_sec += 1;
	}
	ts.tv_sec += ulTimeout / 1000;

	while ((s = sem_timedwait(pEvent, &ts)) == -1 && errno == EINTR)
		continue; /* Restart if interrupted by handler */

	/* Check what happened */
	if (s == -1)
	{
		if (errno == ETIMEDOUT)
			return CIFX_EVENT_TIMEOUT; /* Timeout */
		else
			return 2; //CIFX_EVENT_TIMEOUT;	/* Error, but there is no other return value */
	}
	else
	{
		return CIFX_EVENT_SIGNALLED; /* Success */
	}
	return 3; //CIFX_EVENT_TIMEOUT;	/* Error, but there is no other return value */
}

/*****************************************************************************/
/*! Compare two ASCII string
 *   \param pszBuf1   First buffer
 *   \param pszBuf2   Second buffer
 *   \return 0 if strings are equal                                           */
/*****************************************************************************/
i32 OS_Strcmp(const char* pszBuf1,
						const char* pszBuf2)
{
	return strcmp(pszBuf1, pszBuf2);
}

/*****************************************************************************/
/*! Compare characters of two strings without regard to case
 *   \param pszBuf1   First buffer
 *   \param pszBuf2   Second buffer
 *   \param ulLen     Number of characters to compare
 *   \return 0 if strings are equal                                           */
/*****************************************************************************/
i32 OS_Strnicmp(const char* pszBuf1,
						  const char* pszBuf2,
						  u32 ulLen)
{
	return strncasecmp(pszBuf1, pszBuf2, ulLen);
}

/*****************************************************************************/
/*! Query the length of an ASCII string
 *   \param szText    ASCII string
 *   \return character count of szText                                        */
/*****************************************************************************/
i32 OS_Strlen(const char* szText)
{
	return (i32) strlen(szText);
}

/*****************************************************************************/
/*! Copies one string to another monitoring the maximum length of the target
 *   buffer.
 *   \param szDest    Destination buffer
 *   \param szSource  Source buffer
 *   \param ulLength  Maximum length to copy
 *   \return pointer to szDest                                                */
/*****************************************************************************/
char* OS_Strncpy(char* szDest,
					  const char* szSource,
					  u32 ulLength)
{
	char *r;
	r = strncpy(szDest, szSource, ulLength);
/*     USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_Strncpy(szDest=%s, szSource=%s, ulLength=%u)",*/
/*         szDest, szSource, ulLength);*/
	return r;
}

/*****************************************************************************/
/*! Create an interrupt safe locking mechanism (Spinlock/critical section)
 *   \return handle to the locking object                                     */
/*****************************************************************************/
tOS_Lock* OS_CreateLock(void)
{
   /* we use priority inheritance by default,
    * just to make high prio task don't have to wait
    * long than needed. */
   pthread_mutexattr_t attr;
	pthread_mutex_t *pLock = malloc(sizeof(pthread_mutex_t));

	UserTraceDebug(TraceFacility_Osal, "%s", "OS_CreateLock");

	if (pLock == 0)
		return 0;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

#ifndef NDEBUG
//	pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST_NP);
	pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#endif

	pthread_mutex_init(pLock, &attr);
	pthread_mutexattr_destroy(&attr);

	return pLock;
}

/*****************************************************************************/
/*! Enter a critical section/spinlock
 *   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_EnterLock(tOS_Lock* pLock)
{
   i32 error;
   if ((error =  pthread_mutex_lock(pLock)))
      UserTraceError("mutex lock failed with %s", strerror(error));
}

/*****************************************************************************/
/*! Leave a critical section/spinlock
 *   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_LeaveLock(tOS_Lock* pLock)
{
	i32 error;

	if (( error = pthread_mutex_unlock(pLock)))
	   UserTraceError("mutex unlock failed with %s", strerror(error));

}

/*****************************************************************************/
/*! Delete a critical section/spinlock object
 *   \param pvLock Handle to the locking object being deleted                 */
/*****************************************************************************/
void OS_DeleteLock(tOS_Lock* pLock)
{
	assert(pLock != NULL);
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_DeleteLock");

	pthread_mutex_destroy(pLock);
	free(pLock);
}

/*****************************************************************************/
/*! Create an Mutex object for locking code sections
 *   \return handle to the mutex object                                       */
/*****************************************************************************/
tOS_Mutex* OS_CreateMutex(void)
{

   pthread_mutexattr_t attr;
   pthread_mutex_t *pMutex = malloc(sizeof(pthread_mutex_t));

   UserTraceDebug(TraceFacility_Osal, "%s", "OS_CreateLock");

   if (pMutex == 0)
      return 0;

   pthread_mutexattr_init(&attr);
   /* we use prio inheritance by default */
   pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

#ifndef NDEBUG
//   pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST_NP);
   pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#endif

   pthread_mutex_init(pMutex, &attr);
   pthread_mutexattr_destroy(&attr);

   return pMutex;
}

/*****************************************************************************/
/*! Wait for mutex
 *   \param pvMutex    Handle to the Mutex locking object
 *   \param ulTimeout  Wait timeout in milliseconds
 *   \return !=0 on succes                                                    */
/*****************************************************************************/
i32 OS_WaitMutex(tOS_Mutex* pMutex,
							u32 ulTimeout)
{
   assert(pMutex != NULL);
	/*return (WaitForSingleObject((HANDLE)pvMutex, ulTimeout) == WAIT_OBJECT_0) ? 1 : 0;*/
	struct timespec abs_timeout;

	UserTraceDebug(TraceFacility_Osal, "OS_WaitMutex(timeout=%dms)", ulTimeout);

	abs_timeout.tv_sec = ulTimeout / 1000;	// convert to seconds
	abs_timeout.tv_nsec = (ulTimeout % 1000) * 1000000;	// convert to nanoseconds

	i32 error;
	if ((error = pthread_mutex_timedlock(pMutex, &abs_timeout)))
	{
	   if ( error != ETIMEDOUT)
	      UserTraceError("pthread_mutex_timedlock failed with %s", strerror(error));
		return 0;
	}

	return 1;
}

/*****************************************************************************/
/*! Release a mutex section section
 *   \param pvMutex Handle to the locking object                              */
/*****************************************************************************/
void OS_ReleaseMutex(tOS_Mutex* pMutex)
{
   assert(pMutex != NULL);

	UserTraceDebug(TraceFacility_Osal, "(%s=%p)",_NV(pMutex));
	i32 error;
	error = pthread_mutex_unlock(pMutex);
	if ( error )
	   UserTraceError("pthread_mutex_unlock failed with %s", strerror(error));
}

/*****************************************************************************/
/*! Delete a Mutex object
 *   \param pvMutex Handle to the mutex object being deleted                  */
/*****************************************************************************/
void OS_DeleteMutex(tOS_Mutex* pMutex)
{
   assert(pMutex != NULL);

	UserTraceDebug(TraceFacility_Osal, "(%s=%p)",_NV(pMutex));
	pthread_mutex_destroy(pMutex);
	free(pMutex);
}

/*****************************************************************************/
/*! Opens a file in binary mode
 *   \param szFile     Full file name of the file to open
 *   \param pulFileLen Returned length of the opened file
 *   \return handle to the file, NULL mean file could not be opened           */
/*****************************************************************************/
void* OS_FileOpen(char* szFile,
						u32* pulFileLen)
{
   assert(szFile != NULL);
   assert(pulFileLen != NULL);

	char pwd[200];
	getcwd(pwd, sizeof(pwd));
	i32 iFile;
	void* pvRet = NULL;

	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_FileOpen(%s)", szFile);

	iFile = open(szFile, O_RDONLY);
	if (iFile != -1)
	{
		struct stat tBuffer;
		ZERO_MEMORY(&tBuffer);

		if (fstat(iFile, &tBuffer) != -1)
		{
			*pulFileLen = (u32) tBuffer.st_size;
			pvRet = (void*) iFile;
		}
	}

	return pvRet;
}

/*****************************************************************************/
/*! Closes a previously opened file
 *   \param pvFile Handle to the file being closed                            */
/*****************************************************************************/
void OS_FileClose(void* pvFile)
{
   assert(pvFile);

	i32 iFile = (i32) pvFile;
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_FileClose");

	close(iFile);
}

/*****************************************************************************/
/*! Read a specific amount of bytes from the file
 *   \param pvFile   Handle to the file being read from
 *   \param ulOffset Offset inside the file, where read starts at
 *   \param ulSize   Size in bytes to be read
 *   \param pvBuffer Buffer to place read bytes in
 *   \return number of bytes actually read from file                          */
/*****************************************************************************/
u32 OS_FileRead(void* pvFile,
							u32 ulOffset,
							u32 ulSize,
							void* pvBuffer)
{
   assert(pvFile != NULL);
   assert(pvBuffer != NULL);

	i32 iFile = (i32) pvFile;
	u32 ulRet = 0;

	UserTraceDebugF(TraceFacility_Osal, "");

	if (ulOffset == (u32) lseek(iFile, (i32) ulOffset, SEEK_SET))
	{
		ulRet = (u32) read(iFile, pvBuffer, ulSize);
	}

	return ulRet;
}

/*****************************************************************************/
/*! O/S specific initialization (if needed), called during cifXTKitInit()    */
/*****************************************************************************/
i32 OS_Init()
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_Init");

	return 0;
}

/*****************************************************************************/
/*! O/S specific de-initialization (if needed), called during cifXTKitInit() */
/*****************************************************************************/
void OS_Deinit()
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_Deinit");

}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
 * write back all BAR's (Base address registers), Interrupt and Command
 * Register. These registers are invalidated during cifX Reset and need to be
 * re-written after the reset has succeeded
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice
 *   \param pvPCIConfig   Configuration returned by OS_ReadPCIConfig
 *                        (implementation dependent)                          */
/*****************************************************************************/
void OS_WritePCIConfig(void* pvOSDependent,
							  void* pvPCIConfig)
{
	UserTraceDebug(TraceFacility_Osal, "(%s=%p, %s=%p)",_NV(pvOSDependent),_NV(pvPCIConfig));

	NetxDriver driver = (NetxDriver) pvOSDependent;
	NetxDevice device = driver->GetNetxDevice(driver);

	u32 *v;
	struct pci_access *pacc;
	struct pci_dev *dev;

	v = pvPCIConfig;

	pacc = pci_alloc();
	if (pacc == 0)
	{
		UserTraceErrorF("error at pci_alloc()");
	}
	else
	{
		pci_init(pacc); /* Initialize the PCI library */
		pci_scan_bus(pacc); /* We want to get the list of devices */
		for (dev = pacc->devices; dev; dev = dev->next) /* Iterate over all devices */
		{
			pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES); /* Fill in header info we need */
			pci_read_word(dev, PCI_CLASS_DEVICE); /* Read config register directly */
			USER_Trace(NULL, TRACE_LEVEL_DEBUG, "%02x:%02x.%d vendor=%04x device=%04x irq=%d base0=%lx",
						  dev->bus,
						  dev->dev, dev->func, dev->vendor_id, dev->device_id,
						  dev->irq,
						  dev->base_addr[0]);
			if (dev->base_addr[0] == device->GetPhysicalAddress(device))
			{
				USER_Trace(NULL, TRACE_LEVEL_DEBUG, "found\n");
				pci_write_word(dev, PCI_COMMAND,(u16) v[0]);
				pci_write_long(dev, PCI_BASE_ADDRESS_0, v[1]);
				pci_write_long(dev, PCI_BASE_ADDRESS_1, v[2]);
				pci_write_long(dev, PCI_BASE_ADDRESS_2, v[3]);
				pci_write_byte(dev, PCI_INTERRUPT_LINE, (u8)v[4]);
				break;
			}
		}
		pci_cleanup(pacc); /* Close everything */
	}
	free(v);


	return;
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
 * read all BAR's (Base address registers), Interrupt and Command Register.
 * These registers are invalidated during cifX Reset and need to be
 * re-written after the reset has succeeded
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice
 *   \return pointer to stored register copies (implementation dependent)     */
/*****************************************************************************/
void* OS_ReadPCIConfig(void* pvOSDependent)
{
	NetxDriver netxDriver = (NetxDriver) pvOSDependent;
	NetxDevice netxDevice = (NetxDevice) netxDriver->GetNetxDevice(netxDriver);

	u32 *v;
	struct pci_access *pacc;
	struct pci_dev *dev;

	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig(%x)", pvOSDependent);

	v = malloc(5 * sizeof(u32));
	memset(v, 0, 5 * sizeof(u32));

	pacc = pci_alloc();
	if (pacc == 0)
	{
		USER_Trace(NULL, TRACE_LEVEL_ERROR, "error at pci_alloc()\n");
	}
	else
	{
		pci_init(pacc); /* Initialize the PCI library */
		pci_scan_bus(pacc); /* We want to get the list of devices */
		for (dev = pacc->devices; dev; dev = dev->next) /* Iterate over all devices */
		{
			pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES); /* Fill in header info we need */
			pci_read_word(dev, PCI_CLASS_DEVICE); /* Read config register directly */
			USER_Trace(NULL, TRACE_LEVEL_DEBUG, "%02x:%02x.%d vendor=%04x device=%04x irq=%d base0=%lx",
						  dev->bus,
						  dev->dev, dev->func, dev->vendor_id, dev->device_id,
						  dev->irq,
						  dev->base_addr[0]);
			if (dev->base_addr[0] == netxDevice->GetPhysicalAddress(netxDevice))
			{
				USER_Trace(NULL, TRACE_LEVEL_DEBUG, "found\n");
				v[0] = pci_read_word(dev, PCI_COMMAND);
				v[1] = pci_read_long(dev, PCI_BASE_ADDRESS_0);
				v[2] = pci_read_long(dev, PCI_BASE_ADDRESS_1);
				v[3] = pci_read_long(dev, PCI_BASE_ADDRESS_2);
				v[4] = pci_read_byte(dev, PCI_INTERRUPT_LINE);
				break;
			}
		}
		pci_cleanup(pacc); /* Close everything */
		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: command %08x", v[0]);
		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: bar0 %08x", v[1]);
		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: bar1 %08x", v[2]);
		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: bar2 %08x", v[3]);
		USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_ReadPCIConfig: irql %08x", v[4]);
	}
	return v;
}

/*****************************************************************************/
/*! This function Maps a DPM pointer to a user application if needed.
 *   This example just returns the pointer valid inside the driver.
 *   In Win32 Kernel Mode this must be replaced by calls to
 *   \param pvDriverMem  Pointer to be mapped
 *   \param ulMemSize    Size to be mapped
 *   \param ppvMappedMem Returned mapped pointer (usable by application)
 *   \return Handle that is needed for unmapping (can not be NULL)                */
/*****************************************************************************/
void* OS_MapUserPointer(void* pvDriverMem,
								u32 ulMemSize,
								void** ppvMappedMem,
								void* pvOSDependent)
{
	/* TODO: Check if we can Map the pointer here instead inside the netxToolkitInit Function*/
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_MapUserPointer");
	/* in linux the DPM is already mapped to user space, so just return this pointer */
	UNUSED(ulMemSize);
	UNUSED(pvOSDependent);
	*ppvMappedMem = pvDriverMem;

	return pvDriverMem;
}

/*****************************************************************************/
/*! This function unmaps a previously mapped user application pointer
 *   In Win32 Kernel Mode this function must call
 *   \param phMapping  Handle that was returned by OS_MapUserPointer
 *   \return !=0 on success                                                   */
/*****************************************************************************/
i32 OS_UnmapUserPointer(void* phMapping,
									 void* pvOSDependent)
{
	USER_Trace(NULL, TRACE_LEVEL_DEBUG, "OS_UnmapUserPointer");
	/* nothing to do for linux */
	UNUSED(phMapping);
	UNUSED(pvOSDependent);
	return 1;
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_EnableInterrupts(void* pvOSDependent)
{
	USER_Trace(NULL, TRACE_LEVEL_INFO, "OS_EnableInterrupts");
	UNUSED(pvOSDependent);
	/*printf("We do not support IRQs in Win32 User mode!!");*/
	/*UNREFERENCED_PARAMETER(pvOSDependent);*/
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
 *   \param pvOSDependent O/S Dependent Variable passed during call to
 *                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_DisableInterrupts(void* pvOSDependent)
{
	USER_Trace(NULL, TRACE_LEVEL_INFO, "OS_DisableInterrupts");
	UNUSED(pvOSDependent);
	/*printf("We do not support IRQs in Win32 User mode!!");*/
	/*UNREFERENCED_PARAMETER(pvOSDependent);*/
}

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
