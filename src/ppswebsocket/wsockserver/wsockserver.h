#pragma once

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio.hpp>

#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include <string>
#include <functional>
#include <iostream>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/locale.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

class class_asio_tls_server : public websocketpp::server<websocketpp::config::asio_tls> {};
class class_asio_no_tls_server : public websocketpp::server<websocketpp::config::asio> { public: void set_tls_init_handler(websocketpp::lib::function<websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>(websocketpp::connection_hdl)> h) {} };

typedef class_asio_tls_server asio_tls_server;
typedef class_asio_no_tls_server asio_no_tls_server;

template <typename EndpointType>
class CWsockServer {

	// See https://wiki.mozilla.org/Security/Server_Side_TLS for more details about
	// the TLS modes. The code below demonstrates how to implement both the modern
	typedef enum tls_mode {
		MOZILLA_INTERMEDIATE = 1,//1024bits
		MOZILLA_MODERN = 2,//2048bits
	};

public:

	CWsockServer() {
		// Reset the log channels
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.set_access_channels(websocketpp::log::alevel::all);
		m_endpoint.set_error_channels(websocketpp::log::elevel::all);

		// Initialize Asio Transport
		m_endpoint.init_asio();
		m_endpoint.set_reuse_addr(true);

		// Register handler callbacks
		if (typeid(typename EndpointType) != typeid(asio_no_tls_server))
		{
			m_endpoint.set_tls_init_handler(websocketpp::lib::bind(&CWsockServer::on_tls_init_sslv23, this, websocketpp::lib::placeholders::_1));
		}
		m_endpoint.set_socket_init_handler(websocketpp::lib::bind(&CWsockServer::on_socket_init_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_fail_handler(websocketpp::lib::bind(&CWsockServer::on_fail_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_open_handler(websocketpp::lib::bind(&CWsockServer::on_open_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_close_handler(websocketpp::lib::bind(&CWsockServer::on_close_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_http_handler(websocketpp::lib::bind(&CWsockServer::on_http_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_ping_handler(websocketpp::lib::bind(&CWsockServer::on_ping_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_handler(websocketpp::lib::bind(&CWsockServer::on_pong_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_timeout_handler(websocketpp::lib::bind(&CWsockServer::on_pong_timeout_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_interrupt_handler(websocketpp::lib::bind(&CWsockServer::on_interrupt_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_validate_handler(websocketpp::lib::bind(&CWsockServer::on_validate_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_message_handler(websocketpp::lib::bind(&CWsockServer::on_message_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

	}
	CWsockServer(boost::asio::io_service & ios) {
		// Reset the log channels
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.set_access_channels(websocketpp::log::alevel::all);
		m_endpoint.set_error_channels(websocketpp::log::elevel::all);

		// initialize asio with our external io_service rather than an internal one

		// Initialize Asio Transport
		m_endpoint.init_asio(&ios);
		m_endpoint.set_reuse_addr(true);

		// Register handler callbacks
		if (typeid(typename EndpointType) != typeid(asio_no_tls_server))
		{
			m_endpoint.set_tls_init_handler(websocketpp::lib::bind(&CWsockServer::on_tls_init_handler, this, websocketpp::lib::placeholders::_1));
		}
		m_endpoint.set_socket_init_handler(websocketpp::lib::bind(&CWsockServer::on_socket_init_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_fail_handler(websocketpp::lib::bind(&CWsockServer::on_fail_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_open_handler(websocketpp::lib::bind(&CWsockServer::on_open_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_close_handler(websocketpp::lib::bind(&CWsockServer::on_close_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_http_handler(websocketpp::lib::bind(&CWsockServer::on_http_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_ping_handler(websocketpp::lib::bind(&CWsockServer::on_ping_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_handler(websocketpp::lib::bind(&CWsockServer::on_pong_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_pong_timeout_handler(websocketpp::lib::bind(&CWsockServer::on_pong_timeout_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		m_endpoint.set_interrupt_handler(websocketpp::lib::bind(&CWsockServer::on_interrupt_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_validate_handler(websocketpp::lib::bind(&CWsockServer::on_validate_handler, this, websocketpp::lib::placeholders::_1));
		m_endpoint.set_message_handler(websocketpp::lib::bind(&CWsockServer::on_message_handler, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

	}

	void start(uint16_t port) {

		// listen on specified port
		m_endpoint.listen(port);

		// Start the server accept loop
		m_endpoint.start_accept();
	}

	static std::string get_password(
		std::size_t max_length,  // The maximum size for a password.
		websocketpp::lib::asio::ssl::context::password_purpose purpose // Whether password is for reading or writing.
	) {
		return "test";//return "123456";
	}

	static std::string get_password_callback() {
		return "test";//return "123456";
	}
	
	websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> on_tls_init_handler(websocketpp::connection_hdl hdl) {
		//tls_mode mode = MOZILLA_MODERN; 
		tls_mode mode = MOZILLA_INTERMEDIATE;
		//std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;
		//std::cout << "using TLS mode: " << (mode == MOZILLA_MODERN ? "Mozilla Modern" : "Mozilla Intermediate") << std::endl;

		websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> ctx = websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(websocketpp::lib::asio::ssl::context::sslv23);

		try {
			if (mode == MOZILLA_MODERN) {
				// Modern disables TLSv1
				ctx->set_options(websocketpp::lib::asio::ssl::context::default_workarounds |
					websocketpp::lib::asio::ssl::context::no_sslv2 |
					websocketpp::lib::asio::ssl::context::no_sslv3 |
					websocketpp::lib::asio::ssl::context::no_tlsv1 |
					websocketpp::lib::asio::ssl::context::single_dh_use);
			}
			else {
				ctx->set_options(websocketpp::lib::asio::ssl::context::default_workarounds |
					websocketpp::lib::asio::ssl::context::no_sslv2 |
					websocketpp::lib::asio::ssl::context::no_sslv3 |
					websocketpp::lib::asio::ssl::context::single_dh_use);
			}
			ctx->set_password_callback(bind(&get_password, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
			//ctx->set_password_callback(bind(&get_password_callback));
			ctx->use_certificate_chain_file("server.pem");
			ctx->use_private_key_file("server.pem", websocketpp::lib::asio::ssl::context::pem);

			// Example method of generating this file:
			// `openssl dhparam -out dh.pem 2048`
			// Mozilla Intermediate suggests 1024 as the minimum size to use
			// Mozilla Modern suggests 2048 as the minimum size to use.
			ctx->use_tmp_dh_file("dh.pem");

			std::string ciphers = "";

			if (mode == MOZILLA_MODERN) {
				ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!3DES:!MD5:!PSK";
			}
			else {
				ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
			}

			if (SSL_CTX_set_cipher_list(ctx->native_handle(), ciphers.c_str()) != 1) {
				std::cout << "Error setting cipher list" << std::endl;
			}
		}
		catch (std::exception& e) {
			std::cout << "Exception: " << e.what() << std::endl;
		}
		return ctx;
	}
	void on_socket_init_handler(websocketpp::connection_hdl) {
		std::cout << "on_socket_init" << std::endl;
	}
	void on_open_handler(websocketpp::connection_hdl hdl) {
		std::cout << "on_open_handler" << std::endl;
	}
	void on_close_handler(websocketpp::connection_hdl hdl) {
		std::cout << "on_close_handler" << std::endl;
	}
	void on_http_handler(websocketpp::connection_hdl hdl) {
		typename EndpointType::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);
		std::string strResource = con->get_resource();
		websocketpp::http::parser::request req = con->get_request();
		websocketpp::http::parser::response resp = con->get_response();

		std::string uri = req.get_uri();
		std::string raw = req.raw();
		std::string strHeader = req.get_header("host");
		std::string strBody = req.get_body();
		std::string strMethod = boost::to_lower_copy(req.get_method());
		std::string strVersion = req.get_version();
		
		con->replace_header("Server", "PPSHUAI Server 2018");
		// Enable cors
		con->append_header("Access-Control-Allow-Credentials", "true");
		con->append_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE, PUT");
		con->append_header("Access-Control-Allow-Origin", "*");

		con->set_body("This is a test!");
		con->set_status(websocketpp::http::status_code::ok);
	}
	void on_fail_handler(websocketpp::connection_hdl hdl) {
		typename EndpointType::connection_ptr con = m_endpoint.get_con_from_hdl(hdl);

		// Print as much information as possible
		m_endpoint.get_elog().write(websocketpp::log::elevel::warn,
			"Fail handler: " + std::to_string(con->get_state()) + " " +
			std::to_string(con->get_local_close_code()) + " " + con->get_local_close_reason() + " " + std::to_string(con->get_remote_close_code()) + " " +
			con->get_remote_close_reason() + " " + std::to_string(con->get_ec().value()) + " - " + con->get_ec().message() + "\n");
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
	void on_message_handler(websocketpp::connection_hdl hdl, typename EndpointType::message_ptr msg) {
		// queue message up for sending by processing thread
		std::string strPayLoad = msg->get_payload();
		m_endpoint.send(hdl, strPayLoad, websocketpp::frame::opcode::text);
	}

private:

	typename EndpointType m_endpoint;
};

__inline static int wsockserver_main() {
	try {
		// set up an external io_service to run both endpoints on. This is not
		// strictly necessary, but simplifies thread management a bit.
		boost::asio::io_service ios;

		CWsockServer<asio_tls_server> server_tls_instance(ios);
		CWsockServer<asio_no_tls_server> server_no_tls_instance(ios);

		// Run the asio loop with the main thread
		server_tls_instance.start(9002);
		server_no_tls_instance.start(9003);
		
		// Start the ASIO io_service run loop
		ios.run();
	}
	catch (websocketpp::exception const & e) {
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