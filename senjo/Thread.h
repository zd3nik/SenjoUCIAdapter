//-----------------------------------------------------------------------------
// Copyright (c) 2015-2019 Shawn Chidester <zd3nik@gmail.com>
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
//-----------------------------------------------------------------------------

#ifndef SENJO_THREADING_H
#define SENJO_THREADING_H

#include "Platform.h"
#include <mutex>

namespace senjo {

//-----------------------------------------------------------------------------
//! \brief Base class for a single background task.
//-----------------------------------------------------------------------------
class Thread {
protected:
  std::unique_ptr<std::thread> thread;
  std::mutex mutex;

  //---------------------------------------------------------------------------
  //! \brief Constructor
  //---------------------------------------------------------------------------
  explicit Thread(int id = -1);

  //---------------------------------------------------------------------------
  //! This method is called once when the thread is run.
  //! When this method exits the thread is finished.
  //---------------------------------------------------------------------------
  virtual void doWork() = 0;

public:
  //---------------------------------------------------------------------------
  // Delete copy operations
  //---------------------------------------------------------------------------
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  //---------------------------------------------------------------------------
  //! \brief Destructor
  //---------------------------------------------------------------------------
  virtual ~Thread();

  //---------------------------------------------------------------------------
  //! \brief Get the id of this thread.
  //! \return the id of this thread
  //---------------------------------------------------------------------------
  int getId() const { return id; }

  //---------------------------------------------------------------------------
  //! \brief Run this thread.
  //! \return false if the thread is already running
  //---------------------------------------------------------------------------
  virtual bool run();

  //---------------------------------------------------------------------------
  //! \brief Tell this thread to exit the doWork() method as soon as possible.
  //! Should do nothing if the thread is already stopped.
  //---------------------------------------------------------------------------
  virtual void stop() = 0;

  //---------------------------------------------------------------------------
  //! \brief Is this thread running?
  //! \return true If this thread is running
  //---------------------------------------------------------------------------
  bool isRunning();

  //---------------------------------------------------------------------------
  //! \brief Wait for this thread to finish running.
  //! If this thread isn't running this method returns immediately.
  //---------------------------------------------------------------------------
  void waitForFinish();

private:
  static void staticRun(Thread*);
  int id;
};

} // namespace senjo

#endif // SENJO_THREADING_H
