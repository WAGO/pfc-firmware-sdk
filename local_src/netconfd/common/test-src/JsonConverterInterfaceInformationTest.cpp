

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "InterfaceInformation.hpp"
#include "JsonConverter.hpp"
#include <nlohmann/json.hpp>

namespace netconf {

using namespace ::testing;

TEST(JsonConverterInterfaceInformationTest, ConvertBackAndForth){
  InterfaceInformation ii{"mydev", "mylabel", DeviceType::Virtual, true};

  JsonConverter jc;
  auto ii_json = jc.ToJsonString(ii);

  InterfaceInformation out_obj;
  auto status = jc.FromJsonString(ii_json, out_obj);

  EXPECT_EQ(StatusCode::OK, status.Get()) << ii_json << "\n"<< status.GetMessage();

  EXPECT_EQ("mydev", out_obj.GetInterfaceName());
  EXPECT_EQ("mylabel", out_obj.GetInterfaceLabel());
  EXPECT_EQ(DeviceType::Virtual, out_obj.GetType());
  EXPECT_EQ(true, out_obj.IsIpConfigurationReadonly());
}

TEST(JsonConverterInterfaceInformationTest, FailOnMissingKeys){
  JsonConverter jc;
  auto ii_json = R"({"name":"mydev","label":"mylabel"})";

  InterfaceInformation out_obj;
  auto status = jc.FromJsonString(ii_json, out_obj);

  EXPECT_EQ(StatusCode::JSON_CONVERT_ERROR, status.Get()) << status.GetMessage();

}

TEST(JsonConverterInterfaceInformationTest, AllTypes){
  JsonConverter jc;
  auto ii_json_ethernet =R"({"name":"mydev","label":"mylabel","type":"ethernet","ip-ro":true})";
  auto ii_json_bridge = R"({"name":"mydev","label":"mylabel","type":"bridge","ip-ro":true})";
  auto ii_json_virtual = R"({"name":"mydev","label":"mylabel","type":"virtual","ip-ro":true})";
  auto ii_json_service = R"({"name":"mydev","label":"mylabel","type":"service","ip-ro":true})";
  auto ii_json_port = R"({"name":"mydev","label":"mylabel","type":"port","ip-ro":true})";
  auto ii_json_wwan = R"({"name":"mydev","label":"mylabel","type":"wwan","ip-ro":true})";
  auto ii_json_other = R"({"name":"mydev","label":"mylabel","type":"wwan","ip-ro":true})";

  InterfaceInformation out_obj;
  auto status = jc.FromJsonString(ii_json_ethernet, out_obj);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
  status = jc.FromJsonString(ii_json_bridge, out_obj);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
  status = jc.FromJsonString(ii_json_virtual, out_obj);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
  status = jc.FromJsonString(ii_json_service, out_obj);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
  status = jc.FromJsonString(ii_json_port, out_obj);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
  status = jc.FromJsonString(ii_json_wwan, out_obj);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
  status = jc.FromJsonString(ii_json_other, out_obj);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();

}

TEST(JsonConverterInterfaceInformationTest, MultipleInformationObjects){
  JsonConverter jc;
  auto ii_json_ethernet =R"([{"name":"dev1","label":"mylabel1","type":"ethernet","ip-ro":false},)"
                         R"({"name":"dev2","label":"mylabel2","type":"bridge","ip-ro":true}])";

  InterfaceInformations iis;
  auto status = jc.FromJsonString(ii_json_ethernet, iis);
  EXPECT_EQ(StatusCode::OK, status.Get()) << status.GetMessage();
  EXPECT_EQ(2, iis.size());

  auto j_str = jc.ToJsonString(iis);
  EXPECT_EQ(nlohmann::json::parse(j_str), nlohmann::json::parse(ii_json_ethernet));

}
}  // namespace netconf

