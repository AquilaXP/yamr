#pragma once

#include <algorithm>
#include <fstream>
#include <sstream>
#include <mutex>

#include "reduce.h"

class my_handler_reduce : public handler_reduce
{
public:
    my_handler_reduce() = default;
    my_handler_reduce( my_handler_reduce* parent )
        : m_parent(parent) {}

    void init( uint32_t id ) override
    {
        m_id = id;
        std::stringstream ss;
        ss << "file_" << id << ".txt";
        f.open( ss.str(), std::ios::binary );
    }
    void move_line( std::string&& line ) override
    {
        if( !m_last.empty() )
        {
            if( m_last == line )
            {
                min_len = ( std::max )( static_cast<decltype( min_len )>( m_last.size() + 1 ) , min_len );
            }
        }

        m_last = std::move( line );
        f << m_last << '\n';
    }
    void flush() override
    {
        f.flush();
        std::stringstream ss;
        ss << "reduce[" << m_id << "] = " << min_len << '\n';
        std::cout << ss.str();
        if( m_parent )
            m_parent->add_result( min_len );
    }
    sp_handler_reduce clone() override
    {
        return std::make_shared<my_handler_reduce>(this);
    }
    void print_result()
    {
        std::cout << "Min length unique string:" << min_len << '\n';
    }
private:
    void add_result( uint32_t min_res )
    {
        std::lock_guard<std::mutex> lk( mut );
        min_len = ( std::max )( min_res, min_len );
    }

    uint32_t m_id = 0;
    std::mutex mut;
    my_handler_reduce* m_parent = nullptr;
    uint32_t min_len = 1;
    std::ofstream f;
    std::string m_last;
};
