/*---------------------------------------------------------------------------
 * Copyright (c) 2006 connectBlue AB, Sweden.
 * Any reproduction without written permission is prohibited by law.
 *
 * Component: Operating System
 * File     : cb_os.h
 *
 * Description: Abstraction of the actual operating system.
 *-------------------------------------------------------------------------*/
#ifndef _CB_OS_H_
#define _CB_OS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "cb_comdefs.h"

/*===========================================================================
 * DEFINES
 *=========================================================================*/

/* Resources */
#ifndef cbOS_MAX_NO_OF_PROCS
    #define cbOS_MAX_NO_OF_PROCS            ((cb_int32)10)
#endif

#define cbOS_INVALID_PROCESS_ID         0x0000FFFF

/* Creation of a process */
#define cbOS_PROCESS_FUNCTION(procFcn) void procFcn(void* cb_UNUSED(dummy))

/* Accessing a process */
#define cbOS_PROCESS_ID(process) (process##Id)

/* External process definition. */
#define cbOS_EXTERN_PROCESS(process) extern cbOS_ProcessId process##Id

#define cbOS_RTSL_DEFAULT_PRIO 30
/*===========================================================================
 * TYPES
 *=========================================================================*/
typedef void (*cbOS_ProcFcn)(void *pParameter);

typedef cb_int32 cbOS_ProcessId;

typedef cb_int32 cbOS_MsgId;

typedef void (*cbOS_IdleHandler)(void);

typedef void (*cbOS_SendHandler)(
    cbOS_MsgId          msgId,
    cbOS_ProcessId      toProcess,
    cbOS_ProcessId      fromProcess,
    cb_int64            time,
    cb_int8             data[]);

typedef void (*cbOS_ErrorHandler)(
    cb_int32            errorCode,
    cbOS_ProcessId      processId,
    cb_uint32           stackPtr,
    const cb_char*      filename,
    cb_uint32           line);

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

/*---------------------------------------------------------------------------
 * Initializes the operating system module. Typically called from start 
 * handler.
 *-------------------------------------------------------------------------*/
void cbOS_init(void);

/*---------------------------------------------------------------------------
 * cbOS main event handling loop.
 *-------------------------------------------------------------------------*/
void cbOS_main(void * pParameters);
void cbOS_runOnce();

/*---------------------------------------------------------------------------
 * Registers the idle handler. The idle handler is called from the idle 
 * process. Does not need to be installed.
 *-------------------------------------------------------------------------*/
void cbOS_registerIdleHandler(cbOS_IdleHandler idleHandler);

/*---------------------------------------------------------------------------
 * Registers the send handler. The send handler is called every time a 
 * message is sent. Does not need to be installed.
 *-------------------------------------------------------------------------*/
void cbOS_registerSendHandler(cbOS_SendHandler sendHandler);

/*---------------------------------------------------------------------------
 * Registers the error handler. The error handler is called when the EXIT 
 * and ASSERT macros are called (see cb_assert.h).
 *-------------------------------------------------------------------------*/
void cbOS_registerErrorHandler(cbOS_ErrorHandler errorHandler);

/**
 * Send a message to a specified process.
 * @param processId Identifier of receiving process. Use @ref cbOS_PROCESS_ID macro.
 *              E.g. cbOS_PROCESS_ID(SAMPLE_process)
 * @param pMsg Pointer to message allocated by the @ref cbOS_allocMessage function.
 *
 * @see cbOS_allocMessage
 * @see cbOS_receive
 */
void cbOS_send(cbOS_ProcessId processId, void* pMsg);

/**
 * Receive a message.
 * @param msgId Specifies what kind of messages to receive.
 * @param nMsg specifies how many identifiers the msgId vector contains.
 * If all messages are to be received, the msgId parameter is set
 * to NULL and the nMsg parameter is set to 0. 
 * 
 * @return @ref NULL if no message is available, otherwise a pointer to a message.
 *
 * @see cbOS_send
 * @see cbOS_allocMessage
 * @see cbOS_getMessageId
 */
void* cbOS_receive(cbOS_MsgId msgId[], cb_int32 nMsg);

/*---------------------------------------------------------------------------
 * Update the event mask.
 * - processId: Identifier of receiving process. Use cbOS_PROCESS_ID macro.
 *              E.g. cbOS_PROCESS_ID(SAMPLE_process)
 * - events: Bitmask of the events to set.
 *-------------------------------------------------------------------------*/
void cbOS_setEvents(cbOS_ProcessId processId, cb_uint32 events);

/*---------------------------------------------------------------------------
 * Update the event mask from an interrupt service routing.
 * - processId: Identifier of receiving process. Use cbOS_PROCESS_ID macro.
 *              E.g. cbOS_PROCESS_ID(SAMPLE_process)
 * - events: Bitmask of the events to set.
 *-------------------------------------------------------------------------*/
void cbOS_setEventsIsr(cbOS_ProcessId processId, cb_uint32 events);

/*---------------------------------------------------------------------------
 * Read the event mask for the current process. 
 * Calling this operation will reset the mask. 
 * Receives a bit mask of the set events.
 *-------------------------------------------------------------------------*/
cb_uint32 cbOS_getEvents(void);

/*---------------------------------------------------------------------------
 * Returns the process identifier of the calling process.
 *-------------------------------------------------------------------------*/
cbOS_ProcessId cbOS_getCurrentProcess(void);

/**
 * Gets the sender of a received message.
 * @param pMsg Pointer to a message received by the @ref cbOS_receive call.
 *
 * @return ProcessId of the originating process.
 */
cbOS_ProcessId cbOS_getSender(void* pMsg);

/**
 * Gets the message id of a received message.
 * @param pMsg Pointer to a message received by the @ref cbOS_receive call.
 *
 * @return Message Id of the message.
 * 
 * @see cbOS_receive
 * @see cbOS_freeMessage
 */
cbOS_MsgId cbOS_getMessageId(void* pMsg);

/*---------------------------------------------------------------------------
 * Check if a buffer with nBytes is available for allocation.
 * - nBytes: Number of bytes.
 * Returns TRUE if buffer available, otherwise FALSE.
 *-------------------------------------------------------------------------*/
//TODO remove
cb_boolean cbOS_bufferAvailable(cb_int32 nBytes);

/**
 * Allocates dynamical memory for messages. 
 * @param nBytes Number of bytes to allocate.
 * @param messageID Message ID.
 * 
 * @returns Pointer to the allocated memory.
 * 
 * @see cbOS_freeMessage
 * @see cbOS_send
 * @see cbOS_receive
 */
void* cbOS_allocMessage(cb_int32 nBytes, cbOS_MsgId messageID);

/*---------------------------------------------------------------------------
 * Gets the maximum size that can be allocated using cbOS_allocMessage.
 *-------------------------------------------------------------------------*/
 //TODO remove
cb_uint32 cbOS_getAllocMaxSize(void);

/**
 * Deallocates memory previously allocated by @ref cbOS_allocMessage.
 * @param pBuf Pointer to memory.
 * @see cbOS_allocMessage
 */
void cbOS_freeMessage(void* pBuf);

/**
 * Enter critical section. 
 * This function must handle nested calls.
 * @returns State to be passed to @ref cbOS_exitCritical
 */
cb_uint32 cbOS_enterCritical(void);
/**
 * Exit critical section. 
 * @param prevState The previos state returned by @ref cbOS_enterCritical()
 */
void cbOS_exitCritical(cb_uint32 prevState);
/**
 * Busy wait for ~us microseconds.
 * @param us  Delay time in microseconds
 */
void cbOS_delay(cb_uint32 us);

#if defined(__cplusplus)
}
#endif

#endif /* _CB_OS_H_ */
