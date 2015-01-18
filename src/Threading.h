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

#ifndef SENJO_THREADING_H
#define SENJO_THREADING_H

#include "Platform.h"

namespace senjo
{

//----------------------------------------------------------------------------
//! \brief Mutual exclusion locking object
//! Generally used to synchronize access to shared data.
//----------------------------------------------------------------------------
class Mutex
{
public:
  //--------------------------------------------------------------------------
  //! \brief Constructor
  //--------------------------------------------------------------------------
  Mutex();

  //--------------------------------------------------------------------------
  //! \brief Destructor
  //--------------------------------------------------------------------------
  virtual ~Mutex();

  //--------------------------------------------------------------------------
  //! \brief Obtain exclusive lock on the mutex
  //! This will block (wait) until the lock is obtained.
  //--------------------------------------------------------------------------
  void Lock();

  //--------------------------------------------------------------------------
  //! \brief Relinquish exclusive lock on the mutex
  //! Behavior is undefined if this method is called without first obtaining
  //! a lock on the mutex with the Lock() method.
  //--------------------------------------------------------------------------
  void Unlock();

private:
#ifdef WIN32
  HANDLE mutex;
#else
  pthread_mutex_t mutex;
#endif
};

//----------------------------------------------------------------------------
//! \brief Represents a single background thread
//! Runs a single function at a time on the background thread.
//----------------------------------------------------------------------------
class Thread
{
public:
  //--------------------------------------------------------------------------
  //! \brief Constructor
  //--------------------------------------------------------------------------
  Thread();

  //--------------------------------------------------------------------------
  //! \brief Destructor
  //--------------------------------------------------------------------------
  virtual ~Thread();

  //--------------------------------------------------------------------------
  //! \brief Execute the given function on the background thread
  //! \param[in] function The function to execute
  //! \param[in] param Will be passed to \p function when it is executed
  //! \retur true If \p function was started successfully
  //--------------------------------------------------------------------------
  bool Start(void (*function)(void*), void* param);

  //--------------------------------------------------------------------------
  //! \brief Is this thread running a function?
  //! \return true If a function is running a function
  //--------------------------------------------------------------------------
  bool Active() const;

  //--------------------------------------------------------------------------
  //! \brief Wait for the function running on this thread to finish
  //! If this thread isn't running a function the method returns immediately.
  //--------------------------------------------------------------------------
  void Join();

private:
  void (*threadFunction)(void*);
  void* threadParam;

#ifdef WIN32
  static DWORD RunThreadFunction(LPVOID);
  HANDLE thread;
  DWORD threadID;
#else
  static void* RunPThreadFunction(void*);
  pthread_t thread;
  bool running;
#endif
};

} // namespace senjo

#endif // SENJO_THREADING_H
