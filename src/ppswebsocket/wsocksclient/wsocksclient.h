#ifndef __BOOSTUTILS_H__
#define __BOOSTUTILS_H__

/*
* Copyright (c) 2014, Peter Thorson. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the WebSocket++ Project nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/** ====== WARNING ========
* This example is presently used as a scratch space. It may or may not be broken
* at any given time.
*/

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>


#include <iostream>

#include <boost/locale.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <map>
#include <string>

#include "gziputils.h"

std::map<std::string, int> g_mapURI;

template<typename datatype>
__inline static bool common_ParseJsonData(std::map<std::string, typename datatype> & odata, std::string &str)
{
	try
	{
		boost::property_tree::ptree pt;
		std::stringstream stream(str);
		read_json(stream, pt);
		BOOST_FOREACH(auto &v, odata)
		{
			try
			{
				v.second = pt.get<typename datatype>(v.first);
			}
			catch (boost::property_tree::ptree_error pt)
			{
				;
			}
		}
	}
	catch (boost::property_tree::ptree_error pt)
	{
		pt.what();
		return false;
	}
	return true;
}
__inline static void print(boost::property_tree::ptree const& pt)
{
	using boost::property_tree::ptree;
	ptree::const_iterator end = pt.end();
	for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
		std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
		print(it->second);
	}
}
__inline static int tttt_main()
{
	try
	{
		std::stringstream ss;
		// send your JSON above to the parser below, but populate ss first

		ss << "{ \"particles\": [ { \"electron\": { \"pos\": [ 0, 0, 0 ], \"vel\": [ 0, 0, 0 ] }, \"proton\": { \"pos\": [ -1, 0, 0 ], \"vel\": [ 0, -0.1, -0.1 ] } } ]}";


		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("particles"))
		{
			assert(v.first.empty()); // array elements have no names
			print(v.second);
		}
		return EXIT_SUCCESS;
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return EXIT_FAILURE;
}
__inline static bool common_ParseJsonList(std::map<std::string, std::string> & odata, std::map<std::string, double> & ddmap, std::string strSymbol, std::string &str)
{
	{
		//tttt_main();
		//return false;
	}
	/*{
		std::string str = "{\"data\":[{\"high\":1.421E-7,\"low\":1.517E-7},{\"high\":1.121E-7,\"low\":1.717E-7}]}";
		boost::property_tree::ptree pt;
		std::stringstream stream(str);
		read_json(stream, pt);
		boost::property_tree::ptree pt_sub;
		pt_sub = pt.get_child("data");// odata.begin()->first);
		BOOST_FOREACH(auto &v, ddmap)
		{
			try
			{
				v.second = pt_sub.begin()->second.get<double>(v.first);
			}
			catch (boost::property_tree::ptree_error pt)
			{
				pt.what();
				return false;
			}
		}
		return true;
	}*/
	try
	{
		boost::property_tree::ptree pt;
		std::stringstream stream(str);
		read_json(stream, pt);
		BOOST_FOREACH(auto &v, odata)
		{
			try
			{
				v.second = pt.get<std::string>(v.first);
				if (v.second.length() <= 0)
				{
					boost::property_tree::ptree pt_sub = pt.get_child(v.first);
					BOOST_FOREACH(const boost::property_tree::ptree::value_type &sv, pt_sub)
					{
						if (!sv.second.get<std::string>("symbol").compare(strSymbol))
						//if (!sv.second.get<std::string>("symbol").compare("manbtc"))
						//if (!sv.second.get<std::string>("symbol").compare("hotbtc"))
						{
							BOOST_FOREACH(auto &vv, ddmap)
							{
								try
								{
									vv.second = sv.second.get<double>(vv.first);
								}
								catch (boost::property_tree::ptree_error pt)
								{
									pt.what();
									return false;
								}
							}
							return true;
						}
						
					}
				}
			}
			catch (boost::property_tree::ptree_error pt)
			{
				pt.what();
				return false;
			}
		}
	}
	catch (boost::property_tree::ptree_error pt)
	{
		pt.what();
		return false;
	}
	return true;
}

class class_asio_tls_client : public websocketpp::client<websocketpp::config::asio_tls_client> {};
class class_asio_no_tls_client : public websocketpp::client<websocketpp::config::asio_client> { public: void set_tls_init_handler(websocketpp::lib::function<websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>(websocketpp::connection_hdl)> h) {} };

typedef class_asio_tls_client asio_tls_client;
typedef class_asio_no_tls_client asio_no_tls_client;

template<typename EndpointType>
class CWsocksClient {

	// See https://wiki.mozilla.org/Security/Server_Side_TLS for more details about
	// the TLS modes. The code below demonstrates how to implement both the modern
	typedef enum tls_mode {
		MOZILLA_INTERMEDIATE = 1,//1024bits
		MOZILLA_MODERN = 2,//2048bits
	};
public:

	CWsocksClient() {
		// Clear access and error log
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.set_access_channels(websocketpp::log::alevel::all);
		m_endpoint.set_error_channels(websocketpp::log::elevel::all);

		// Initialize ASIO
		m_endpoint.init_asio();

		// Register our handlers
		if (typeid(typename EndpointType) != typeid(asio_no_tls_client))
		{
			m_endpoint.set_tls_init_handler(websocketpp::lib::bind(&CWsocksClient::on_tls_init_handler, this, websocketpp::lib::placeholders::_1));
		}
		m_endpoint.set_socket_init_handler(websocketpp::lib::bind(&CWsocksClient::on_socket_init_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_fail_handler(websocketpp::lib::bind(&CWsocksClient::on_fail_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_open_handler(websocketpp::lib::bind(&CWsocksClient::on_open_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_close_handler(websocketpp::lib::bind(&CWsocksClient::on_close_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_http_handler(websocketpp::lib::bind(&CWsocksClient::on_http_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_ping_handler(websocketpp::lib::bind(&CWsocksClient::on_ping_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_handler(websocketpp::lib::bind(&CWsocksClient::on_pong_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_timeout_handler(websocketpp::lib::bind(&CWsocksClient::on_pong_timeout_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_interrupt_handler(websocketpp::lib::bind(&CWsocksClient::on_interrupt_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_validate_handler(websocketpp::lib::bind(&CWsocksClient::on_validate_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_message_handler(websocketpp::lib::bind(&CWsocksClient::on_message_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

	}

	CWsocksClient(boost::asio::io_service & ios)
	{
		// Clear access and error log
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.set_access_channels(websocketpp::log::alevel::all);
		m_endpoint.set_error_channels(websocketpp::log::elevel::all);

		// Initialize ASIO
		m_endpoint.init_asio(&ios);

		// Register our handlers
		if (typeid(typename EndpointType) != typeid(asio_no_tls_client))
		{
			m_endpoint.set_tls_init_handler(websocketpp::lib::bind(&CWsocksClient::on_tls_init_handler, this, websocketpp::lib::placeholders::_1));
		}
		m_endpoint.set_socket_init_handler(websocketpp::lib::bind(&CWsocksClient::on_socket_init_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_fail_handler(websocketpp::lib::bind(&CWsocksClient::on_fail_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_open_handler(websocketpp::lib::bind(&CWsocksClient::on_open_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_close_handler(websocketpp::lib::bind(&CWsocksClient::on_close_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_http_handler(websocketpp::lib::bind(&CWsocksClient::on_http_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_ping_handler(websocketpp::lib::bind(&CWsocksClient::on_ping_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_handler(websocketpp::lib::bind(&CWsocksClient::on_pong_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_timeout_handler(websocketpp::lib::bind(&CWsocksClient::on_pong_timeout_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_interrupt_handler(websocketpp::lib::bind(&CWsocksClient::on_interrupt_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_validate_handler(websocketpp::lib::bind(&CWsocksClient::on_validate_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_message_handler(websocketpp::lib::bind(&CWsocksClient::on_message_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		
	}

	void start(std::string uri) {
		websocketpp::lib::error_code ec;
		typename EndpointType::connection_ptr con = m_endpoint.get_connection(uri, ec);

		if (ec)
		{
			m_endpoint.get_alog().write(websocketpp::log::alevel::app, ec.message());
		}
		else
		{
			//con->set_proxy("http://humupdates.uchicago.edu:8443/");
			//con->set_proxy("https://192.168.31.124:1088/");
			if (g_mapURI.begin()->first.compare("wss://api.zb.cn:9999/websocket") != 0)
			{
				//con->set_proxy("https://192.168.31.124:1088/");
			}
			// Specify the HTTP subprotocol:
			//con->add_subprotocol("Sec-WebSocket-Protocol");

			m_endpoint.connect(con);

			// Start the ASIO io_service run loop
			//m_endpoint.run();
		}
	}

	websocketpp::lib::shared_ptr<boost::asio::ssl::context> on_tls_init_handler(websocketpp::connection_hdl hdl) {
		std::cout << "on_tls_init" << std::endl;
		tls_mode mode = tls_mode::MOZILLA_INTERMEDIATE;//tls_mode::MOZILLA_MODERN;//tls_mode::MOZILLA_INTERMEDIATE
		//websocketpp::lib::shared_ptr<boost::asio::ssl::context> ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
		//websocketpp::lib::shared_ptr<boost::asio::ssl::context> ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
		websocketpp::lib::shared_ptr<boost::asio::ssl::context> ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tls);

		typename EndpointType::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
		std::string strProtocol = con->get_subprotocol();
		try {
			switch (mode)
			{
			case tls_mode::MOZILLA_MODERN:
			{
				// Modern disables TLSv1
				ctx->set_options(boost::asio::ssl::context::default_workarounds
					| boost::asio::ssl::context::no_sslv2
					| boost::asio::ssl::context::no_sslv3
					| boost::asio::ssl::context::no_tlsv1
					| boost::asio::ssl::context::single_dh_use
				);
			}
			break;
			case tls_mode::MOZILLA_INTERMEDIATE:
			{
				ctx->set_options(boost::asio::ssl::context::default_workarounds
					| boost::asio::ssl::context::no_sslv2
					| boost::asio::ssl::context::no_sslv3
					| boost::asio::ssl::context::single_dh_use
				);
			}
			break;
			}
			
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		return ctx;
	}

	void on_socket_init_handler(websocketpp::connection_hdl) {
		std::cout << "on_socket_init" << std::endl;
	}
	void on_fail_handler(websocketpp::connection_hdl hdl) {
		typename EndpointType::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);

		std::cout << "Fail handler" << std::endl;
		std::cout << con->get_state() << std::endl;
		std::cout << con->get_local_close_code() << std::endl;
		std::cout << con->get_local_close_reason() << std::endl;
		std::cout << con->get_remote_close_code() << std::endl;
		std::cout << con->get_remote_close_reason() << std::endl;
		std::cout << con->get_ec() << " - " << con->get_ec().message() << std::endl;
	}
	void on_open_handler(websocketpp::connection_hdl hdl) {
		std::cout << "on_open" << std::endl;
		//m_endpoint.send(hdl, "{\"ping\": 18212553000}", websocketpp::frame::opcode::text);
		//m_endpoint.send(hdl, "{\"id\": \"id generate by client\", \"sub\" : \"topic to sub\", \"freq-ms\" : 1000}", websocketpp::frame::opcode::text);
		//m_endpoint.send(hdl, "{\"id\": \"id generate by client\", \"sub\" : \"topic to sub\"}", websocketpp::frame::opcode::text);
		if (g_mapURI.begin()->first.compare("wss://api.hadax.com/ws") == 0)
		{
			m_endpoint.send(hdl, boost::locale::conv::between("{\"id\": \"id1\", \"sub\" : \"market.tickers\"}", "UTF-8", "GBK"), websocketpp::frame::opcode::text);
		}
		else if (g_mapURI.begin()->first.compare("wss://www.bitmex.com/realtime") == 0)
		{
			//m_endpoint.send(hdl, boost::locale::conv::between("{\"op\": \"subscribe\", \"args\": [<SubscriptionTopic>]}", "UTF-8", "GBK"), websocketpp::frame::opcode::text);
		}
		else if (g_mapURI.begin()->first.compare("wss://api.zb.cn:9999/websocket") == 0)
		{
			m_endpoint.send(hdl, boost::locale::conv::between("{\"event\":\"addChannel\",\"channel\":\"btcusdt_ticker\"}", "UTF-8", "GBK"), websocketpp::frame::opcode::text);
		}
		else if (g_mapURI.begin()->first.compare("wss://stream.binance.com:9443/ws/btcusdt@kline_1m.b10") == 0)
		{
			//m_endpoint.send(hdl, boost::locale::conv::between("btsusdt@miniticker", "UTF-8", "GBK"), websocketpp::frame::opcode::text);
		}
		//m_endpoint.send(hdl, boost::locale::conv::between("{\"id\": \"id1\", \"sub\" : \"market.ethbtc.detail\"}", "UTF-8", "GBK"), websocketpp::frame::opcode::text);

		//m_endpoint.send(hdl, boost::locale::conv::between("ETHBTC@ticker", "UTF-8", "GBK"), websocketpp::frame::opcode::text);
	}
	void on_close_handler(websocketpp::connection_hdl) {
		std::cout << "on_close" << std::endl;
	}
	void on_http_handler(websocketpp::connection_hdl) {
		std::cout << "on_http_handler" << std::endl;
	}
	bool on_ping_handler(websocketpp::connection_hdl, std::string) {
		std::cout << "on_ping_handler" << std::endl;
		return false;
	}
	void on_pong_handler(websocketpp::connection_hdl, std::string) {
		std::cout << "on_pong_handler" << std::endl;
	}
	void on_pong_timeout_handler(websocketpp::connection_hdl, std::string) {
		std::cout << "on_pong_timeout_handler" << std::endl;
	}
	void on_interrupt_handler(websocketpp::connection_hdl) {
		std::cout << "on_interrupt_handler" << std::endl;
	}
	bool on_validate_handler(websocketpp::connection_hdl) {
		std::cout << "on_validate_handler" << std::endl;
		return false;
	}
	void on_message_handler(websocketpp::connection_hdl hdl, typename EndpointType::message_ptr msgptr) {
		std::cout << "on_message" << std::endl;
		std::string strData = msgptr->get_payload();
		
		if (g_mapURI.begin()->first.compare("wss://api.hadax.com/ws") == 0)
		{
			std::string strDecompressData = CGZip::decompress(strData);
			if (std::string("{\"ping\":").compare(strDecompressData.substr(0, std::string("{\"ping\":").length())) == 0)
			{
				std::cout << strDecompressData << std::endl;
				strDecompressData.at(3) = 'o';
				m_endpoint.send(hdl, strDecompressData, websocketpp::frame::opcode::text);
				std::cout << "Send pong message:" << strDecompressData << std::endl;
			}
			else
			{
				//std::cout << strDecompressData << std::endl;
				std::map<std::string, std::string> ssmap = { { "data", "" }, };
				std::map<std::string, INT64> dtmap = { { "ts", 0LL }, };
				std::map<std::string, double> ddmap = { { "high", 0.0f },{ "low", 0.0f },{ "vol", 0.0f },{ "close", 0.0f }, };
#define		BASE_FACTOR	100000
				if (common_ParseJsonList(ssmap, ddmap, "rccceth", strDecompressData) && common_ParseJsonData<INT64>(dtmap, strDecompressData))
				{
					if (ddmap.at("high") && ddmap.at("low") && ddmap.at("close"))
					{
						std::cout << ssmap.at("data") << std::endl;
						std::cout << dtmap.at("ts") << std::endl;
						std::cout << ddmap.at("high") * BASE_FACTOR << std::endl;
						std::cout << ddmap.at("low") * BASE_FACTOR << std::endl;
						std::cout << ddmap.at("vol") * BASE_FACTOR << std::endl;
						std::cout << "===========true===========" << std::endl;
						printf("%ws,%ws,%.8f,%.8f,%.8f,%.8f,%lld\r\n", L"BTCUST", L"TestDemo", ddmap.at("high") * BASE_FACTOR, ddmap.at("low") * BASE_FACTOR, ddmap.at("close") * BASE_FACTOR, ddmap.at("vol") * BASE_FACTOR, dtmap.at("ts"));

						double dHigh = ddmap.at("high");
					}
				}
			}
		}
		else if (g_mapURI.begin()->first.compare("wss://www.bitmex.com/realtime") == 0)
		{
			std::cout << strData << std::endl;
		}
		else if (g_mapURI.begin()->first.compare("wss://api.zb.cn:9999/websocket") == 0)
		{
			std::cout << strData << std::endl;
		}
		else if (g_mapURI.begin()->first.compare("wss://stream.binance.com:9443/ws/btcusdt@kline_1m.b10") == 0)
		{
			std::cout << strData << std::endl;
		}
		//m_endpoint.close(hdl, websocketpp::close::status::going_away, "");
	}
private:

	typename EndpointType m_endpoint;

};

__inline static int wsocksclient_main(int argc, char* argv[]) {

	std::string uri = "";
	//uri = "wss://api.hadax.com/ws";
	//uri = "wss://beta.bittrex.com/signalr";
	//uri = "wss://api.huobi.pro/ws";
	//uri = "wss://stream.binance.com:9443";
	//uri = "wss://www.bitmex.com/realtime";
	//uri = "wss://www.bitmex.com/realtime?subscribe=instrument:BTCUSDT";
	//uri = "wss://api.zb.cn:9999/websocket";
	uri = boost::locale::conv::between("wss://stream.binance.com:9443/ws/btcusdt@kline_1m.b10", "UTF-8", "GBK");
	g_mapURI.insert(std::map<std::string, int>::value_type(uri, g_mapURI.size()));

	if (argc == 2) {
		uri = argv[1];
	}

	try {
		boost::asio::io_service ios;
		CWsocksClient<asio_tls_client> endpoint(ios);
		endpoint.start(uri);
		ios.run();
	}
	catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	catch (websocketpp::lib::error_code e) {
		std::cout << e.message() << std::endl;
	}
	catch (...) {
		std::cout << "other exception" << std::endl;
	}

	return 0;
}


#endif // __BOOSTUTILS_H__