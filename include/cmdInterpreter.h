#pragma once

#include <functional>
#include <map>

class CmdInterpreter
{
  public:
    
    CmdInterpreter(); // TODO: Should be overloaded with istream and ostream
    typedef std::function<void(const std::string argStr)> CmdInterpreterFnc;

    void addCmd(std::string& cmd, CmdInterpreterFnc fnc);
    void addCmd(std::string&& cmd, CmdInterpreterFnc fnc);
    void addCmd(std::string& cmd, CmdInterpreterFnc fnc, std::string& cmdDesc);
    void getCmd();

  protected:
    void runCmd(std::string& cmd);
    void printHelp();

  private:
    
    std::map<std::string, std::pair<CmdInterpreterFnc, std::string>> mRegisteredCmds;
};