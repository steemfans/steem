
#include <steem/utilities/storage_configuration_manager.hpp>

#include <boost/test/unit_test.hpp>
#include <sys/stat.h>
#include <stdio.h>

#include <iostream>
#include <fstream>

using namespace steem::utilities;

BOOST_AUTO_TEST_SUITE(storage_configuration_tests)

BOOST_AUTO_TEST_CASE(storage_configuration_basic_tests)
{
   const std::string directory_name = "storage_configuration_tests_directory";
   const std::string config_ini_name = directory_name + "/config.ini";

   std::remove( directory_name.c_str() );
   mkdir( directory_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   struct stat sb;
   BOOST_REQUIRE( stat(directory_name.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode) );

   //Create content of config.ini
   std::vector< std::string > content;
   content.emplace_back( "follow-configuration-file = follow_config.ini" );
   content.emplace_back( "p2p-configuration-file = p2p_config.ini" );

   //remove file
   remove( config_ini_name.c_str() );

   std::ofstream f( config_ini_name );

   for( auto& item : content )
      f<<item<<"\n";

   f.flush();
   f.close();

   {
      storage_configuration_manager scm;

      const char* argv[] = { "path", "-d", "storage_configuration_tests_directory", "--storage-root-path", "any_path" };
      scm.add_plugin( "follow" );
      scm.add_plugin( "p2p" );
      scm.initialize( 5, (char**)argv );

      const bfs::path& path = scm.get_storage_root_path();
      std::string s_path = path.string();

      bfs::path storage_root_path = bfs::current_path() / "any_path";

      BOOST_REQUIRE( scm.get_storage_root_path().string() == storage_root_path );

      BOOST_REQUIRE( scm.get_storage_path( "follow" ) == storage_root_path / "follow-data-dir" );
      BOOST_REQUIRE( scm.get_storage_path( "p2p" ) == storage_root_path / "p2p-data-dir" );

      BOOST_REQUIRE( scm.get_config_file( "follow" ) == storage_root_path / "follow_config.ini" );
      BOOST_REQUIRE( scm.get_config_file( "p2p" ) == storage_root_path / "p2p_config.ini" );
   }

}

BOOST_AUTO_TEST_SUITE_END()
