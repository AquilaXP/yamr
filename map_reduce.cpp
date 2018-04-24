#include "map_reduce.h"

map_reduce::map_reduce( const std::string& file_name, uint32_t count_map, uint32_t count_reduce ) : m_count_map( count_map ), m_count_reduce( count_reduce )
{
    m_map.set_param( file_name, count_map );
    m_shuffle.set_param( count_map, count_reduce );
    m_reduce.set_param( count_reduce );
}

void map_reduce::set_map_func( sp_handler_map prototype )
{
    m_map.set_handler_prototype( prototype );
}

void map_reduce::set_reduce_func( sp_handler_reduce prototype )
{
    m_reduce.set_handler( prototype );
}

void map_reduce::run()
{
    // отображаем
    m_map.start();

    // переносим данные из отобржаение в смешивание
    for( uint32_t i = 0; i < m_count_map; ++i )
    {
        m_shuffle.move_data( std::move( m_map.get_data( i ) ), i );
    }
    // смешиваем, подготавливаем для R контейнеров для свертки 
    m_shuffle.start();

    // переносим данные из смешивания в свертку
    for( uint32_t i = 0; i < m_count_reduce; ++i )
    {
        m_reduce.move_data( std::move( m_shuffle.get_data( i ) ), i );
    }

    // свертываем
    m_reduce.start();
}
