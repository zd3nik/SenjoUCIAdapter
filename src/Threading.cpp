//----------------------------------------------------------------------------
// Copyright (c) 2015 Shawn Chidester <zd3nik@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//----------------------------------------------------------------------------

#include "Threading.h"

namespace senjo
{

//----------------------------------------------------------------------------
Mutex::Mutex()
{
#ifdef WIN32
  mutex = CreateMutex(NULL, FALSE, NULL);
  if (!mutex) {
    std::cerr << "Unable to create mutex: error code " << GetLastError()
              << std::endl;
  }
#else
  pthread_mutex_init(&mutex, NULL);
#endif
}

//----------------------------------------------------------------------------
Mutex::~Mutex()
{
#ifdef WIN32
  if (mutex) {
    CloseHandle(mutex);
    mutex= NULL;
  }
#else
  pthread_mutex_destroy(&mutex);
#endif
}

//----------------------------------------------------------------------------
void Mutex::Lock()
{
#ifdef WIN32
  if (mutex) {
    WaitForSingleObject(mutex, INFINITE);
  }
#else
  pthread_mutex_lock(&mutex);
#endif
}

//----------------------------------------------------------------------------
void Mutex::Unlock()
{
#ifdef WIN32
  if (mutex) {
    ReleaseMutex(mutex);
  }
#else
  pthread_mutex_unlock(&mutex);
#endif
}

//----------------------------------------------------------------------------
Thread::Thread()
  : threadFunction(NULL),
    threadParam(NULL)
{
#ifdef WIN32
  thread = NULL;
#else
  running = false;
#endif
}

//----------------------------------------------------------------------------
Thread::~Thread()
{
  Join();
}

//----------------------------------------------------------------------------
bool Thread::Start(void (*function)(void*), void* param)
{
  if (!function) {
    std::cerr << "NULL thread function" << std::endl;
    return false;
  }
#ifdef WIN32
  if (thread) {
    std::cerr << "Thread::Start() already active" << std::endl;
    return false;
  }
  threadFunction = function;
  threadParam = param;
  thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RunThreadFunction,
                        (LPVOID)this, 0, &threadID);
  if (!thread) {
    std::cerr << "Unable to create thread: error code " << GetLastError()
              << std::endl;
  }
#else
  if (running) {
    std::cerr << "Thread::Start() already active" << std::endl;
    return false;
  }
  threadFunction = function;
  threadParam = param;
  int err = pthread_create(&thread, NULL, RunPThreadFunction, this);
  if (err) {
    std::cerr << "Unable to create thread: error code " << err
              << std::endl;
    return false;
  }
  running = true;
#endif
  return true;
}

//----------------------------------------------------------------------------
bool Thread::Active() const
{
#ifdef WIN32
  return (thread != NULL);
#else
  return running;
#endif
}

//----------------------------------------------------------------------------
void Thread::Join()
{
#ifdef WIN32
  if (thread) {
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    thread = NULL;
  }
#else
  if (running) {
    void* ret = NULL;
    pthread_join(thread, &ret);
    running = false;
  }
#endif
}

//----------------------------------------------------------------------------
#ifdef WIN32
DWORD Thread::RunThreadFunction(LPVOID param)
{
  if (param) {
    Thread* me = static_cast<Thread*>(param);
    me->threadFunction(me->threadParam);
  }
  return 0;
}
#else
void* Thread::RunPThreadFunction(void* param)
{
  if (param) {
    Thread* me = static_cast<Thread*>(param);
    me->threadFunction(me->threadParam);
  }
  return NULL;
}
#endif

} // namespace senjo
