/* queue.c
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

#define IN_QUEUE

#include <system.h>

#include "queue.h"

QueueEntry* QueueIncrement(QueueEntry* queueIndex)
{
	if (queueIndex == &queue[QUEUE_LENGTH - 1])
		return &queue;
	else
		return ++queueIndex;
}

void PrePushQueue(void)
{
	if (IsQueueFull()) {
		PopQueue();
	}
}

void PrePushQueueKeepHead(void)
{
	if (IsQueueFull()) {
		queueTail = QueueNextHead();
		queueCount = 1;
	}
}

void ClearQueueTail(void)
{
	if (!IsQueueEmpty()) {
		queueTail = QueueNextHead();
		queueCount = 1;
	}
}
