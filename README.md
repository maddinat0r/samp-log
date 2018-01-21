# samp-log

[![sampctl](https://shields.southcla.ws/badge/sampctl-samp--log-2f2f2f.svg?style=for-the-badge)](https://github.com/SAMP-git/samp-log)

## Installation

Simply install to your project:

```bash
sampctl package install SAMP-git/samp-log
```

Include in your code and begin using the library:

```pawn
#include <log-plugin>
```

## Usage

```pawn
#include <a_samp>
#include <log-plugin>


new
    Logger:mainlog,
    Logger:playerlog,
    Logger:playermoneylog,
    Logger:systemlog;

public OnGameModeInit()
{
    mainlog = CreateLog("main");
    playerlog = CreateLog("player/main");
    playermoneylog = CreateLog("player/money");
    systemlog = CreateLog("system", DEBUG | INFO | WARNING | ERROR);
    
    Log(mainlog, INFO, "created all loggers");
    Log(playerlog, WARNING, "playerid %d is moving too %s", 1234, "fast");
    Log(playermoneylog, ERROR, "invalid amount %f", 3.14532);
    Log(systemlog, DEBUG, "update took %d seconds", 4321);
    return 1;
}


public OnGameModeExit()
{
    DestroyLog(mainlog);
    DestroyLog(playerlog);
    DestroyLog(playermoneylog);
    DestroyLog(systemlog);
    return 1;
}
```
