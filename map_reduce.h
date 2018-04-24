#pragma once

#include "map.h"
#include "reduce.h"
#include "shuffle.h"

class map_reduce
{
public:
    map_reduce( const std::string& file_name, uint32_t count_map, uint32_t count_reduce );

    void set_map_func( sp_handler_map prototype );
    void set_reduce_func( sp_handler_reduce prototype );

    void run();

private:

    uint32_t m_count_map = 1;
    uint32_t m_count_reduce = 1;

    map m_map;
    shuffle m_shuffle;
    reduce m_reduce;
};