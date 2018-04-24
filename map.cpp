#include "map.h"

#include <thread>
#include <stdexcept>

#include "utilites.h"

uint64_t get_size_file( const std::string& file_name )
{
    std::ifstream f( file_name, std::ios::binary );
    if( !f )
        throw std::runtime_error( "fail open file:" + file_name );

    f.seekg( 0, std::ios::end );
    return static_cast<uint64_t>( f.tellg() );;
}

/************************************************************************/
/*                        IHandlerMap                                   */
/************************************************************************/

data_t& handler_map::get_data()
{
    return m_data;
}

/************************************************************************/
/*                        map_worker                                    */
/************************************************************************/

map_worker::map_worker( sp_handler_map handler, const std::string& file_name, int64_t begin, int64_t end, uint64_t size_file )
{
    set_param( handler, file_name, begin, end, size_file );
}

void map_worker::set_param( sp_handler_map handler, const std::string& file_name, int64_t begin, int64_t end, uint64_t size_file )
{
    m_handler = handler;
    m_file_name = file_name;
    m_begin_file_pos = begin;
    m_end_file_pos = end;
    m_size_file = size_file;
}

void map_worker::run()
{
    std::ifstream f( m_file_name, std::ios::binary );
    // корректируем позиции, так чтобы не были посредине строки
    correct_begin_end( f );
    f.seekg( m_begin_file_pos );
    std::string str;
    while( std::getline( f, str ) )
    {
        // вышли за пределы, это не наша строка
        if( m_end_file_pos < f.tellg() )
            break;
        m_handler->push( std::move( str ) );
    }
    // говорим завершить все
    m_handler->flush();
}

void map_worker::correct_begin_end( std::ifstream& f )
{
    // Движемся вправо если по середине строки
    if( m_begin_file_pos != 0 )
    {
        f.seekg( m_begin_file_pos - 1, std::ios::beg );
        f.ignore( ( std::numeric_limits<std::streamsize>::max )( ), '\n' );
        m_begin_file_pos = f.tellg();
    }
    if( m_end_file_pos < ( m_size_file ) )
    {
        f.seekg( m_end_file_pos, std::ios::beg );
        f.ignore( ( std::numeric_limits<std::streamsize>::max )( ), '\n' );
        m_end_file_pos = f.tellg();
    }
}

/************************************************************************/
/*                               map                                    */
/************************************************************************/
void map::set_param( const std::string& file_name, uint32_t count_map )
{
    m_file_name = file_name;
    m_count_map = count_map;
    m_size_file = get_size_file( m_file_name );
}

void map::set_handler_prototype( sp_handler_map handler )
{
    m_prototype = handler;
}

void map::start()
{
    // Разделяем файл на равные куски, не заботимся о выравнивании
    auto pos_file_begin_end = split( m_size_file, m_count_map );
    // Очищаем всех рабов
    m_workers.swap( std::vector<map_worker>() );
    
    // Создаем обработчики на основе прототипа
    m_handlers_map.resize( pos_file_begin_end.size() );
    std::generate( m_handlers_map.begin(), m_handlers_map.end(),
                   [this](){ return m_prototype->clone(); 
    });

    // Подготавливаем рабочий объект
    auto h = m_handlers_map.begin();
    uint32_t id = 1;
    for( auto& pos : pos_file_begin_end )
    {
        (*h)->init( id++ );
        m_workers.emplace_back( *h, m_file_name, pos.first, pos.second, m_size_file );
        ++h;
    }

    // запускаем потоки
    std::vector<std::thread> t;
    for( auto& w : m_workers )
    {
        t.emplace_back( &map_worker::run, &w );
    }
    // ожидаем завершения
    wait_pool_thread( t );
}

data_t& map::get_data( uint32_t num )
{
    return m_handlers_map.at( num )->get_data();
}

// Разделяет файл на равные части, возращает массив пар чисел: начало и конец в байтах
std::vector<std::pair<uint64_t, uint64_t>> map::split( uint64_t size_file, uint64_t count )
{
    std::vector<std::pair<uint64_t, uint64_t>> parts;
    auto size_part = size_file / count;
    auto divv = size_file % count;
    if( divv > 0 )
    {
        // Пытаемся хоть не много подравнять
        auto v = divv / count;
        if( v > 0 )
        {
            size_part += v;
        }
    }
    uint64_t b = 0;
    uint64_t e = size_part;
    for( int i = 0; i < count - 1; ++i )
    {
        parts.push_back( std::make_pair( b, b + size_part - 1 ) );
        b += size_part;
    }
    // В последню вставляем все что осталось, максимум size_part*2-1
    parts.push_back( std::make_pair( b, size_file - 1 ) );

    return parts;
}

