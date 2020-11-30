#include <gtest/gtest.h>

#include "OutputFactory.hpp"
#include <string>
#include <typeinfo>

namespace std {
inline void PrintTo(const type_info &bar, std::ostream *os) {
  *os << bar.name();
}
}

namespace network_config {

using namespace ::std::string_literals;
using ::boost::program_options::variables_map;
using ::boost::program_options::variable_value;

class OutputFactoryTest : public ::testing::Test {
 public:
  variables_map map;

  template<typename T>
  void PutToVM(::std::string key, T &&arg) {
    map.emplace(key, variable_value { std::forward < T > (arg), false });
  }

  void PutToVM(::std::string key) {
    map.emplace(key, variable_value { });
  }

};

TEST_F(OutputFactoryTest, NullOutput) {

  PutToVM("quiet"s);

  auto output = OutputFactory::Create(map);
  EXPECT_EQ(typeid(NullOutput), typeid(*output));
}

TEST_F(OutputFactoryTest, FilenameOutput) {

  PutToVM("error-msg-dst"s, "bla"s);

  auto output = OutputFactory::Create(map);
  EXPECT_EQ(typeid(FileOutput), typeid(*output));
}

TEST_F(OutputFactoryTest, StderrOutput) {

  auto output = OutputFactory::Create(map);
  EXPECT_EQ(typeid(StderrOutput), typeid(*output));
}

TEST_F(OutputFactoryTest, QuietIsPrefered) {

  PutToVM("error-msg-dst"s, "bla"s);
  PutToVM("quiet"s);

  auto output = OutputFactory::Create(map);
  EXPECT_EQ(typeid(NullOutput), typeid(*output));
}

}  // namespace network_config

