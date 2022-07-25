// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include <pthread.h>

static void* thread_routine(void *args)
{

	sleep(3);

}

int main (int argc, char* argv[])
{
    pthread_t thread;
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "1");
    pthread_cancel(thread);
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "2");
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "3");
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "4");
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "5");
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "6");
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "7");
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "8");
    pthread_create(&thread, NULL, thread_routine, NULL);
    pthread_setname_np(thread, "Thread No " "9");
    sleep(10);
    return 0;
}
