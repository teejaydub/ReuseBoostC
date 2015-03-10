/* queue.h
    Copyright (c) 2007 by Timothy J. Weber, tw@timothyweber.org.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
	A FIFO queue of user-definable records.
	Items are added at the tail, and removed from the head.
	
	The record type is declared in queue-consts.h (customized per application 
	from queue-consts-template.h).
	
	Accordingly, only one queue can be used per application.
	It's defined internal to this module, and accessed only through this API.
	But, this gives substantial efficiency over a queue whose elements are sized
	at runtime, without any syntactic bloat.
	
	To facilitate efficient memory use with minimal copying,
	elements are added by first pushing a new element on the tail, then modifying it.
	
	Similarly, the head element is typically accessed and modified in place.
	So, when pushing a new element onto a full queue, you can choose whether to
	discard the oldest item (the head) or to clear the entire queue EXCEPT for the head
	(essentially, discarding the newest items but keeping the 'current' item).
	
	Sample code for adding items:
	
		PrePushQueue();
		QueueTail()->b = 5;
		PushQueue();
		
	Sample code for accessing the head and removing it:
	
		putc(QueueHead()->b);
		PopQueue();
		
*/

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "types-tjw.h"

#include "queue-consts.h"

#ifdef IN_QUEUE
 #define QUEUE_EXTERN
#else
 #define QUEUE_EXTERN  extern
#endif


// These are only intended for use within the Queue module,
// but the inline function definitions need them to be visible here.
QUEUE_EXTERN QueueEntry queue[QUEUE_LENGTH];

// The tail index stored is one past the last element.
// So, it points to the next "free" element, unless the queue is full,
// in which case it points to the head.
// It also points to the head when the queue is empty,
// so we also keep a counter to distinguish between those two states.
QUEUE_EXTERN QueueEntry* queueHead;
QUEUE_EXTERN QueueEntry* queueTail;
QUEUE_EXTERN byte queueCount;

// Returns a pointer to the next queue element after the given one,
// wrapping around.
QueueEntry* QueueIncrement(QueueEntry* queueIndex);

inline void ClearQueue(void)
{
	queueHead = &queue[0];
	queueTail = queueHead;
	queueCount = 0;
}

// Clears everything from the queue but the head.
// Does nothing if the queue is already empty.
void ClearQueueTail(void);

// These would be nice to kee
#define IsQueueEmpty()  queueCount == 0

#define IsQueueFull()  queueCount == QUEUE_LENGTH

inline QueueEntry* QueueHead()
{
	return queueHead;
}

inline QueueEntry* QueueTail()
{
	return queueTail;
}

inline QueueEntry* QueueNextHead(void)
{
	return QueueIncrement(queueHead);
}

inline QueueEntry* QueueNextTail(void)
{
	return QueueIncrement(queueTail);
}

// Makes space for a new item on the queue.
// On return, the tail is ready to be modified with the new value.
// If the queue is full, discards the head.
void PrePushQueue(void);

// Same, but follows the policy where the head is most valuable.
// So, if there is no room, clears out all items except the head.
void PrePushQueueKeepHead(void);

// Accepts the new item, prepared at the "past-the-end" position,
// into the queue.
inline void PushQueue(void)
{
	queueTail = QueueNextTail();
	++queueCount;
}

// Removes the head from the queue.
inline void PopQueue(void)
{
	queueHead = QueueNextHead();
	--queueCount;
}


#endif