#include "wsocksclient.h"

int main(int argc, char* argv[]) 
{
	/*{
	std::string str = "{"
	"\"menu\":"
	"{"
	"\"foo\": true,"
	"\"bar\" : \"true\","
	"\"value\" : 102.3E+06,"
	"\"popup\" :"
	"["
	"{\"value\": \"New\", \"onclick\" : \"CreateNewDoc()\"},"
	"{\"value\": \"Open\", \"onclick\" : \"OpenDoc()\" }"
	"]"
	"}"
	"}";
	try
	{
	boost::property_tree::ptree pt;
	std::stringstream stream(str);
	read_json(stream, pt);

	test_data td;
	td.foo = pt.get_child("menu").get<decltype(td.foo)>("foo");
	td.bar = pt.get_child("menu").get<decltype(td.bar)>("bar");
	td.value = pt.get<decltype(td.value)>("menu.value");

	BOOST_FOREACH(auto & it, pt.get_child("menu").get_child("popup"))
	{
	test_item ti;

	ti.value = it.second.get<decltype(ti.value)>("value");
	ti.onclick = it.second.get<decltype(ti.onclick)>("onclick");

	td.tiv.push_back(ti);
	}
	std::string strValue = pt.get_child("menu.popup").begin()->second.get<std::string>("value");
	std::cout << strValue << std::endl;

	}
	catch (boost::property_tree::ptree_error pt)
	{
	std::cout << pt.what() << std::endl;
	return 0;
	}
	my_common_ParseJsonData(str);

	return 0;
	}*/

	wsocksclient_main(argc, argv);

	return 0;
}
