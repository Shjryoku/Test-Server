#pragma once

#include <string>

struct ServerStats
{
    size_t total_uniq_clients = 0;
    size_t curr_clients = 0;
};

enum class CommandResult{
    OK,
    SHUTDOWN
};

CommandResult handle(
                            const std::string&,
                            const ServerStats&,
                            std::string&
                            );