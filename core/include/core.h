/*
 * Copyright (c) 2015, Thomas Keh
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <functional>
#include <string>
#include <vector>
#include <thread>
#include <atomic>

#include "nmt.h"
#include "sdo.h"
#include "pdo.h"
#include "message.h"

//-------------------------------------------//
// Types used by the CAN driver.             //
// Global scope for linking!                 //
// TODO: Maybe encapsulate in a driver class //
//-------------------------------------------//

/// This struct contains C-strings for
/// busname and baudrate and is passed
/// to a CAN driver
typedef struct {
  /// Bus name
  const char* busname;

  /// Baudrate
  const char* baudrate;

} CANBoard;

/// This type is returned by the CAN driver
/// to identify the driver instance.
typedef void* CANHandle;

namespace kaco {

/// \class Core
///
/// This class implements the Core of KaCanOpen
/// It communicates with the CAN driver, sends
/// CAN messages and listens for incoming
/// CAN messages. You can access CanOpen sub-
/// protocols using public members nmt, sdo and pdo.
class Core {
 public:
  /// type of a message receiver function
  typedef std::function<void(const Message&)> MessageReceivedCallback;

  /// Constructor
  Core();

  /// Destructor
  ~Core();

  /// Opens CAN driver and starts CAN message receive loop.
  ///	\param busname Name of the bus which will be passed to the CAN driver, e.g. slcan0
  ///	\param baudrate Baudrate in 1/s, will be passed to the CAN driver, e.g. 500000
  /// \returns true if successful
  bool start(const std::string busname, unsigned baudrate);

  /// Stops the receive loop and closes the driver.
  void stop();

  /// Sends a message
  void send(const Message& message);

  /// Registers a callback function which is called when a message has been received.
  void register_receive_callback(const MessageReceivedCallback& callback);

  /// The NMT sub-protocol
  NMT nmt;

  /// The SDO sub-protocol
  SDO sdo;

  /// The PDO sub-protocol
  PDO pdo;

 private:
  static const bool debug = true;

  std::atomic<bool> m_running{false};
  std::vector<MessageReceivedCallback> m_receive_callbacks;
  std::thread m_loop_thread;
  CANHandle m_handle;

  void receive_loop(std::atomic<bool>& running);
  void received_message(const Message& m);
};

}  // end namespace kaco