#pragma once

#include <algorithm>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>

#include "reduce.h"

class my_handler_reduce : public handler_reduce
{
public:
    void init( uint32_t id ) override
    {
        std::stringstream ss;
        ss << "file_" << id << ".txt";
        f.open( ss.str(), std::ios::binary );
    }
    void move_line( std::string&& line ) override
    {
        if( !m_last.empty() )
        {
            auto max_len = std::min( line.size(), m_last.size() );
            for( uint32_t i = 0; i < max_len; ++i )
            {
                if( m_last[i] != line[i] )
                {
                    min_len = std::max( i, min_len );
                    break;
                }
            }
        }

        m_last = std::move( line );
        f << m_last << '\n';
    }
    void flush() override
    {
        f.flush();
        std::stringstream ss;
        ss << min_len << '\n';
        std::cout << ss.str();
    }
    sp_handler_reduce clone() override
    {
        return std::make_shared<my_handler_reduce>();
    }
private:
    uint32_t min_len = 1;
    std::ofstream f;
    std::string m_last;
};
