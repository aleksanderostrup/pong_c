#include "../include/cmdInterpreter.h"
#include <utility>
#include <iostream>

CmdInterpreter::CmdInterpreter()
{
}

void CmdInterpreter::addCmd(std::string& cmd, CmdInterpreterFnc fnc)
{
  std::string desc = "No description available";
  addCmd(cmd, fnc, desc);
}

void CmdInterpreter::addCmd(std::string&& cmd, CmdInterpreterFnc fnc)
{
  std::string desc = "No description available";
  addCmd(cmd, fnc, desc);
}

void CmdInterpreter::addCmd(std::string& cmd, CmdInterpreterFnc fnc, std::string& cmdDesc)
{
  mRegisteredCmds.insert(std::make_pair(cmd, std::make_pair(fnc, cmdDesc)));
}

void CmdInterpreter::getCmd()
{ 
  std::string cmd;
  std::cout << "Cmd:";
  // TODO: make cin selectable, so we can have input from other streams than the keyboard
  std::cin >> cmd;
  runCmd(cmd);
}

void CmdInterpreter::runCmd(std::string& cmd)
{
  const auto it = mRegisteredCmds.find(cmd);
  if (mRegisteredCmds.end() != it)
  {
    it->second.first("TEST STRING");
  }
  else
  {
    printHelp();
  }
}

void CmdInterpreter::printHelp()
{
  // TODO: make cout selectable, so we can have output to other than the shell
  std::cout << "Invalid command.\nAvailable commands:\n\n";
  for (auto& item : mRegisteredCmds)
  {
    std::cout << item.first << "\t" << item.second.second << "\n";
  }
}