#include "shuffle.h"

#include <algorithm>

#include "utilites.h"


class shuffle_worker
{
public:
    void run()
    {
        m_N_data.swap( std::vector<data_t>() );
        m_N_data.resize( m_N );
        std::hash<std::string> hf;
        for( auto& d : m_data )
        {
            auto h = hf( d );
            m_N_data[h%m_N].emplace_back( std::move( d ) );
        }
    }
    void move_data( data_t&& data )
    {
        m_data = std::move( data );
    }
    void set_reduce( uint32_t count_reduce )
    {
        m_N = count_reduce;
    }
    data_t& get_result( uint32_t num )
    {
        return m_N_data.at( num );
    }
private:
    uint32_t m_N;
    std::vector<data_t> m_N_data;
    data_t m_data;
};

class shuffle_merge_worker
{
public:
    void push( data_t&& data )
    {
        data_t temp;
        std::swap( temp, m_data );
        m_data.resize( data.size() + temp.size() );
        std::merge( data.begin(), data.end(), temp.begin(), temp.end(), m_data.begin() );
        data.swap( data_t() );
    }
    data_t& get_data()
    {
        return m_data;
    }
private:
    data_t m_data;
};


void shuffle::set_param( uint32_t count_map, uint32_t count_reduce )
{
    m_count_map = count_map;
    m_count_reduce = count_reduce;

    m_datas.swap( std::vector<data_t>( m_count_map ) );
}

void shuffle::move_data( data_t&& data, uint32_t num )
{
    m_datas.at( num ) = std::move( data );
}

void shuffle::start()
{
    std::vector<shuffle_worker> m_workers( m_datas.size() );
    for( uint32_t i = 0; i < m_datas.size(); ++i )
    {
        m_workers[i].set_reduce( m_count_reduce );
        m_workers[i].move_data( std::move( m_datas[i] ) );
    }
    m_datas.swap( std::vector<data_t>() );

    pool_thread_t pool_thread;
    pool_thread.reserve( m_workers.size() );
    for( auto& w : m_workers )
    {
        pool_thread.emplace_back( &shuffle_worker::run, &w );
    }
    wait_pool_thread( pool_thread );

    pool_thread.swap( pool_thread_t() );
    pool_thread.reserve( m_count_reduce );
    std::vector<shuffle_merge_worker> workers_merge( m_count_reduce );
    for( uint32_t i = 0; i < workers_merge.size(); ++i )
    {
        pool_thread.emplace_back(
            [i, &m_workers, &workers_merge, this]()
        {
            for( auto& w : m_workers )
            {
                workers_merge[i].push( std::move( w.get_result( i ) ) );
            }
        } );
    }
    wait_pool_thread( pool_thread );

    m_data_by_reduce.swap( std::vector<data_t>() );
    m_data_by_reduce.reserve( m_count_reduce );
    for( auto& w : workers_merge )
    {
        m_data_by_reduce.emplace_back( std::move( w.get_data() ) );
    }
}

data_t& shuffle::get_data( uint32_t num )
{
    return m_data_by_reduce.at( num );
}
