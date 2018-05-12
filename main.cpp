#include <iostream>

#include "map_reduce.h"
#include "my_handler_map.h"
#include "my_handler_reduce.h"

data_t get_data( const std::string& file_name )
{
    data_t data;
    std::ifstream f( file_name, std::ios::binary );
    std::string line;
    while( std::getline( f, line ) )
    {
        data.emplace_back( std::move( line ) );
    }
    return data;
}

int main( int ac, char* av[] )
{
    int ret = 0;
    try
    {
        if( ac != 4 )
            throw std::runtime_error( "incorect count param!" );

        std::string file_name = av[1];
        uint32_t count_map = std::stol( av[2] );
        uint32_t count_reduce = std::stol( av[3] );

        auto prototype_reduce = std::make_shared<my_handler_reduce>();
        map_reduce mr( file_name, count_map, count_reduce );
        mr.set_map_func( std::make_shared<my_handler_map>() );
        mr.set_reduce_func( prototype_reduce );
        mr.run();

        prototype_reduce->print_result();
    }
    catch( const std::exception& e )
    {
        std::cerr << e.what() << '\n';
        ret = 1;
    }
    
    return 0;
}
