//===-- CommandObjectGUI.cpp ------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CommandObjectGUI.h"

#include "lldb/Core/IOHandlerCursesGUI.h"
#include "lldb/Host/Config.h"
#include "lldb/Interpreter/CommandInterpreter.h"
#include "lldb/Interpreter/CommandReturnObject.h"

using namespace lldb;
using namespace lldb_private;

// CommandObjectGUI

CommandObjectGUI::CommandObjectGUI(CommandInterpreter &interpreter)
    : CommandObjectParsed(interpreter, "gui",
                          "Switch into the curses based GUI mode.", "gui") {}

CommandObjectGUI::~CommandObjectGUI() {}

bool CommandObjectGUI::DoExecute(Args &args, CommandReturnObject &result) {
#if LLDB_ENABLE_CURSES
  if (args.GetArgumentCount() == 0) {
    Debugger &debugger = GetDebugger();

    File &input = debugger.GetInputFile();
    File &output = debugger.GetOutputFile();
    if (input.GetStream() && output.GetStream() && input.GetIsRealTerminal() &&
        input.GetIsInteractive()) {
      IOHandlerSP io_handler_sp(new IOHandlerCursesGUI(debugger));
      if (io_handler_sp)
        debugger.PushIOHandler(io_handler_sp);
      result.SetStatus(eReturnStatusSuccessFinishResult);
    } else {
      result.AppendError("the gui command requires an interactive terminal.");
      result.SetStatus(eReturnStatusFailed);
    }
  } else {
    result.AppendError("the gui command takes no arguments.");
    result.SetStatus(eReturnStatusFailed);
  }
  return true;
#else
  result.AppendError("lldb was not build with gui support");
  return false;
#endif
}
