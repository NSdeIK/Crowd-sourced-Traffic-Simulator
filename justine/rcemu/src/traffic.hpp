#ifndef ROBOCAR_TRAFFIC_HPP
#define ROBOCAR_TRAFFIC_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file smartcity.hpp
 * @author  Norbert Bátfai <nbatfai@gmail.com>
 * @version 0.0.10
 *
 * @section LICENSE
 *
 * Copyright (C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * Robocar City Emulator and Robocar World Championship
 *
 * desc
 *
 */

#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <smartcity.hpp>
#include <car.hpp>

#include <cstdlib>
#include <iterator>

#include <boost/asio.hpp>

#include <limits>

/*
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
*/

#include <memory>

#include <carlexer.hpp>

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

namespace justine
{
namespace robocar
{


enum class TrafficType: unsigned int
{
  NORMAL=0, ANT, ANT_RND, ANT_RERND, ANT_MRERND
};

enum class InitType: unsigned int
{
  NORMAL=0, DISTRIBUTION
};



class Traffic
{
public:

  Traffic ( int size,
            const char * shm_segment,
            double catchdist,
            TrafficType type = TrafficType::NORMAL,
            int minutes = 10 )
    :m_size ( size ), m_catchdist ( catchdist ), m_type ( type ), m_minutes ( minutes )
  {

#ifdef DEBUG
    std::cout << "Attaching shared memory segment called "
              << shm_segment
              << "... " << std::endl;
#endif

    segment = new boost::interprocess::managed_shared_memory (
      boost::interprocess::open_only,
      shm_segment );

    shm_map =
      segment->find<shm_map_Type> ( "JustineMap" ).first;

  }

  virtual void init ( void )
  {

#ifdef DEBUG
    std::cout << "Initializing routine cars ... " << std::endl;
#endif

    if ( m_type != TrafficType::NORMAL )
      for ( shm_map_Type::iterator iter=shm_map->begin();
            iter!=shm_map->end(); ++iter )
        {

          for ( auto noderef : iter->second.m_alist )
            {
              AntCar::alist[iter->first].push_back ( 1 );
              AntCar::alist_evaporate[iter->first].push_back ( 1 );
            }
        }

    for ( int i {0}; i < m_size; ++i )
      {

        //std::unique_ptr<Car> car(std::make_unique<Car>(*this)); //14, 4.9
        //std::unique_ptr<Car> car(new Car {*this});

        if ( m_type == TrafficType::NORMAL )
          {
            std::shared_ptr<Car> car ( new Car {*this} );

            car->init();
            cars.push_back ( car );
          }
        else
          {
            std::shared_ptr<AntCar> car ( new AntCar {*this} );

            car->init();
            cars.push_back ( car );

          }
      }

#ifdef DEBUG
    std::cout << "All routine cars initialized." <<"\n";
#endif

    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

    logfile = boost::posix_time::to_simple_string ( now );
    logFile = new std::fstream ( logfile.c_str() , std::ios_base::out );

    m_cv.notify_one();

    std::cout << "The traffic server is ready." << std::endl;

  }

  ~Traffic()
  {

    m_run = false;
    m_thread.join();
    segment->destroy<shm_map_Type> ( "JustineMap" );
    delete segment;
  }

  void processes ( )
  {
    std::unique_lock<std::mutex> lk ( m_mutex );
    m_cv.wait ( lk );

#ifdef DEBUG
    std::cout << "Traffic simul started." << std::endl;
#endif


    for ( ; m_run; )
      {

        if ( ++m_time > ( m_minutes*60*1000 ) /m_delay )
          {
            m_run = false;
            break;
          }
        else
          {
            traffic_run();
            std::this_thread::sleep_for ( std::chrono::milliseconds ( m_delay ) );
          }

      }

    std::cout << "The traffic simulation is over." << std::endl;

    for ( auto c:m_cop_cars )
      *logFile  << *c << std::endl;

    logFile->close ();

    boost::filesystem::rename (
      boost::filesystem::path ( logfile ),
      boost::filesystem::path ( get_title ( logfile ) ) );

  }

  std::string get_title ( std::string name )
  {

    std::map <std::string, int> res;
    for ( auto c:m_cop_cars )
      {
        res[c->get_name()] += c->get_num_captured_gangsters();
      }

    std::ostringstream ss;

    for ( auto r: res )
      ss << r.first << " " << res[r.first] << " ";

    ss << name << ".txt";

    return ss.str();
  }

  osmium::unsigned_object_id_type virtual node()
  {

    shm_map_Type::iterator iter=shm_map->begin();
    std::advance ( iter, std::rand() % shm_map->size() );

    return iter->first;
  }

  virtual void traffic_run()
  {

    // activities that may occur in the traffic flow

    // std::cout << *this;

    pursuit();

    steps();

  }

  void steps()
  {

    std::lock_guard<std::mutex> lock ( cars_mutex );

    *logFile <<
             m_time <<
             " " <<
             m_minutes <<
             " " <<
             cars.size()
             << std::endl;

    for ( auto car:cars )
      {
        car->step();

        *logFile << *car
                 <<  " " << std::endl;

      }
  }

  inline void pursuit ( void )
  {

    for ( auto car1:m_cop_cars )
      {

        double lon1 {0.0}, lat1 {0.0};
        toGPS ( car1->from(), car1->to() , car1->get_step(), &lon1, &lat1 );

        double lon2 {0.0}, lat2 {0.0};
        for ( auto car:m_smart_cars )
          {

            if ( car->get_type() == CarType::GANGSTER )
              {

                toGPS ( car->from(), car->to() , car->get_step(), &lon2, &lat2 );
                double d = dst ( lon1, lat1, lon2, lat2 );

                if ( d < m_catchdist )
                  {

                    car1->captured_gangster();
                    car->set_type ( CarType::CAUGHT );

                  }
              }
          }
      }
  }

  size_t nedges ( osmium::unsigned_object_id_type from ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_alist.size();
  }

  osmium::unsigned_object_id_type alist ( osmium::unsigned_object_id_type from, int to ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_alist[to];
  }

  int alist_inv ( osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );

    int ret = -1;

    for ( uint_vector::iterator noderefi = iter->second.m_alist.begin();
          noderefi!=iter->second.m_alist.end();
          ++noderefi )
      {

        if ( to == *noderefi )
          {
            ret = std::distance ( iter->second.m_alist.begin(), noderefi );
            break;
          }

      }

    return ret;
  }

  osmium::unsigned_object_id_type salist ( osmium::unsigned_object_id_type from, int to ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_salist[to];

  }
  void set_salist ( osmium::unsigned_object_id_type from, int to , osmium::unsigned_object_id_type value )
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    iter->second.m_salist[to] = value;

  }
  osmium::unsigned_object_id_type palist ( osmium::unsigned_object_id_type from, int to ) const
  {
    shm_map_Type::iterator iter=shm_map->find ( from );
    return iter->second.m_palist[to];
  }

  bool hasNode ( osmium::unsigned_object_id_type node )
  {
    shm_map_Type::iterator iter=shm_map->find ( node );
    return ! ( iter == shm_map->end() );
  }

  void start_server ( boost::asio::io_service& io_service, unsigned short port );

  void cmd_session ( boost::asio::ip::tcp::socket sock );

  friend std::ostream & operator<< ( std::ostream & os, Traffic & t )
  {

    os << t.m_time <<
       " " <<
       t.shm_map->size()
       << std::endl;

    for ( shm_map_Type::iterator iter=t.shm_map->begin();
          iter!=t.shm_map->end(); ++iter )
      {

        os  << iter->first
            << " "
            << iter->second.lon
            << " "
            << iter->second.lat
            << " "
            << iter->second.m_alist.size()
            << " ";

        for ( auto noderef : iter->second.m_alist )
          os  << noderef
              << " ";

        for ( auto noderef : iter->second.m_salist )
          os  << noderef
              << " ";

        for ( auto noderef : iter->second.m_palist )
          os  << noderef
              << " ";

        os << std::endl;

      }

    return os;

  }

  osmium::unsigned_object_id_type naive_node_for_nearest_gangster ( osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step );
  double dst ( osmium::unsigned_object_id_type n1, osmium::unsigned_object_id_type n2 ) const;
  double dst ( double lon1, double lat1, double lon2, double lat2 ) const;
  void toGPS ( osmium::unsigned_object_id_type from,
               osmium::unsigned_object_id_type to,
               osmium::unsigned_object_id_type step, double *lo, double *la ) const;
  osmium::unsigned_object_id_type naive_nearest_gangster ( osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step );

  TrafficType get_type() const
  {
    return m_type;
  }

  int get_time() const
  {
    return m_time;
  }

protected:

  boost::interprocess::managed_shared_memory *segment;
  boost::interprocess::offset_ptr<shm_map_Type> shm_map;

  int m_delay {200};
  bool m_run {true};
  double m_catchdist {15.5};

protected:

  int addCop ( CarLexer& cl );
  int addGangster ( CarLexer& cl );

  int m_size {10000};
  int m_time {0};
  int m_minutes {10};
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::thread m_thread {&Traffic::processes, this};

  std::vector<std::shared_ptr<Car>> cars;
  std::vector<std::shared_ptr<SmartCar>> m_smart_cars;
  std::vector<std::shared_ptr<CopCar>> m_cop_cars;
  std::map<int, std::shared_ptr<SmartCar>> m_smart_cars_map;

  std::mutex cars_mutex;

  TrafficType m_type {TrafficType::NORMAL};

  std::fstream* logFile;
  std::string logfile;
};


class RealTraffic : public Traffic
{
public:
  RealTraffic ( int size,
                const char * shm_segment,
                double catchdist,
                TrafficType type = TrafficType::NORMAL,
                InitType itype = InitType::NORMAL,
                int minutes = 10 )
    : Traffic ( size, shm_segment, catchdist, type, minutes ), m_itype ( itype )
  {

  }


  osmium::unsigned_object_id_type virtual node()
  {

    double no_edges = 0;
    for ( shm_map_Type::iterator iter=shm_map->begin();
          iter!=shm_map->end(); ++iter )
      {

        for ( auto nodeval : RealTraffic::alist[iter->first] )
          no_edges += nodeval;
      }

    double rand = ( double ) std::rand() / ( double ) RAND_MAX;

    double sum = 0.0;
    for ( shm_map_Type::iterator iter=shm_map->begin();
          iter!=shm_map->end(); ++iter )
      {

        for ( auto nodeval : RealTraffic::alist[iter->first] )
          {
            sum += ( double ) nodeval/no_edges;
            if ( sum >= rand )
              return iter->first;
          }
      }

    std::cout << "zavar támadt az erőben..." << std::endl;

    shm_map_Type::iterator iter=shm_map->begin();
    std::advance ( iter, std::rand() % shm_map->size() );

    return iter->first;
  }

  int measured ( osmium::unsigned_object_id_type nr, std::map<int, std::vector<osmium::unsigned_object_id_type> > &map )
  {

    for ( auto& kyvl : map )
      {
        if ( std::find ( kyvl.second.begin(), kyvl.second.end(), nr ) !=kyvl.second.end() )
          return kyvl.first;
      }

    return 0;
  }

  virtual void init ( void )
  {

#ifdef DEBUG
    std::cout << "Initializing routine cars ... " << std::endl;
#endif


    std::map<int, std::vector<osmium::unsigned_object_id_type> > realtraffic =
    {
      {
        {
          789, {

            196329009, 265509164, 196329009, 196329007, 2287951366, 1510298218, 495661984, 1399572708, 267388591,
            351133457, 312629856, 2287951408, 2287951444, 2287951465, 247994818, 1510298325, 1510298320, 196315264,
            206257909, 1510346310, 2511493169, 206260623, 1510298127, 1510298072, 206262390, 1510334903, 2969934911,
            267387939, 267388081, 2969934898, 247994913, 1399589410, 343564038, 2969934903, 343563426, 2784596654,
            2784596655, 206262391, 2784599220, 1491733445, 1135542086, 1491733449, 206262392, 1510298351, 1510298265, 265509163
          }
        }
      ,
      
        {
          317, {
            1237869120, 2527928715, 2931222522, 26754908, 1349166471, 1349166459, 267389536, 26754902, 2526560861,
            2186299187, 2186299228, 267374708, 2186299219, 2186299190, 2186299183, 309273690, 26754905, 267375676,
            2930896656, 2606021513, 267375106, 2931222537, 2527928723
          }
        }
      ,
      
        {
          559, {
            478738342, 2527928724, 1349166437, 2931222529, 1349166547, 2527928719, 26755459, 2527928727, 1402222861,
            1402223001, 392479666, 2529139455, 26755460, 392479640, 26755461, 1499371519, 2529139465, 1500772244,
            1237869120, 2527928717, 2931222526, 389054738, 2527928716, 196315269, 479304099, 1369686850, 1369686542,
            2402540090, 249781414, 1542663729, 2624233832, 2624233836, 1400881624, 2527928718, 974490993, 2527928720,
            1345915078, 1534652124, 2527928726
          }
        }
      }
    };

    if ( m_type != TrafficType::NORMAL )
      for ( shm_map_Type::iterator iter=shm_map->begin();
            iter!=shm_map->end(); ++iter )
        {

          for ( auto noderef : iter->second.m_alist )
            {
              AntCar::alist[iter->first].push_back ( 1 );
              AntCar::alist_evaporate[iter->first].push_back ( 1 );
            }
        }

    if ( m_itype == InitType::DISTRIBUTION )
      for ( shm_map_Type::iterator iter=shm_map->begin();
            iter!=shm_map->end(); ++iter )
        for ( auto noderef : iter->second.m_alist )
          RealTraffic::alist[iter->first].push_back ( measured ( iter->first, realtraffic ) );

    for ( int i {0}; i < m_size; ++i )
      {

        //std::unique_ptr<Car> car(std::make_unique<Car>(*this)); //14, 4.9
        //std::unique_ptr<Car> car(new Car {*this});

        if ( m_type == TrafficType::NORMAL )
          {
            std::shared_ptr<Car> car ( new Car {*this} );

            car->init();
            cars.push_back ( car );
          }
        else
          {
            std::shared_ptr<AntCar> car ( new AntCar {*this} );

            car->init();
            cars.push_back ( car );

          }
      }

#ifdef DEBUG
    std::cout << "All routine cars initialized." <<"\n";
#endif

    boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

    logfile = boost::posix_time::to_simple_string ( now );
    logFile = new std::fstream ( logfile.c_str() , std::ios_base::out );

    m_cv.notify_one();

    std::cout << "The traffic server is ready." << std::endl;

  }

  static AdjacencyList alist;

private:
  InitType m_itype {InitType::NORMAL};

};


}
} // justine::robocar::


#endif // ROBOCAR_TRAFFIC_HPP

