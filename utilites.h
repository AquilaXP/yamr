#pragma once

#include <thread>

using pool_thread_t = std::vector<std::thread>;

template< class Cont >
void wait_pool_thread( Cont& cont )
{
    for( auto& c : cont )
    {
        if( c.joinable() )
            c.join();
    }
}
 