#pragma once

#include "map.h"

class my_handler_map : public handler_map
{
public:
    void init( uint32_t id ) override {}
    void push( std::string&& v ) override
    {
        if( !v.empty() )
        {
            for( size_t i = 1; i < v.size(); ++i )
            {
                m_data.emplace_back( v.substr( 0, i ) );
            }
        }
        m_data.emplace_back( std::move( v ) );
    }
    std::shared_ptr<handler_map> clone() override
    {
        return std::make_shared<my_handler_map>();
    }
};
