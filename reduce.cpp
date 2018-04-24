#include "reduce.h"

#include "utilites.h"

#include <thread>

void reduce_worker::run()
{
    for( auto& d : m_data )
    {
        m_handler->move_line( std::move(d) );
    }
    m_handler->flush();
}

void reduce_worker::set_handler( sp_handler_reduce handler )
{
    m_handler = handler;
}

void reduce_worker::move_line( std::string&& line )
{
    temp.emplace_back( std::move( line ) );
}

void reduce_worker::move_data( data_t&& data )
{
    m_data = std::move( data );
}

void reduce_worker::flush()
{
    data_t temp2;
    temp2.resize( temp.size() + m_data.size() );
    std::merge( temp.begin(), temp.end(), m_data.begin(), m_data.end(), temp2.begin() );
    m_data.swap( temp2 );
    data_t().swap( temp );
}

void reduce::set_handler( sp_handler_reduce prototype )
{
    m_prototype = prototype;
}

void reduce::set_param( uint32_t count_worker )
{
    m_N = count_worker;
    m_workers.swap( std::vector<reduce_worker>( m_N ) );
    m_data.swap( std::vector<data_t>( m_N ) );
}

void reduce::move_data( data_t&& data, uint32_t num )
{
    m_workers.at(num).move_data( std::move( data ) );
}

void reduce::start()
{
    uint32_t id = 1;
    for( auto& w : m_workers )
    {
        auto h = m_prototype->clone();
        h->init( id++ );
        w.set_handler( std::move(h) );
    }

    std::vector<std::thread> pool_thread;
    pool_thread.reserve( m_workers.size() );
    for( auto& w : m_workers )
    {
        pool_thread.emplace_back( &reduce_worker::run, &w );
    }

    wait_pool_thread( pool_thread );
}
