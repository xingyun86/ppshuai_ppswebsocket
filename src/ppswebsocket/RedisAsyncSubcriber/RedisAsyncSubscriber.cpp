#include <string>
#include <map>
#include <iostream>
#include <boost/asio/ip/address.hpp>
#include <boost/format.hpp>
#include <boost/locale.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <redisclient/redisasyncclient.h>

using namespace redisclient;

class RedisAsyncSubscriber
{
public:
	RedisAsyncSubscriber(boost::asio::io_service &ioService,
		const boost::asio::ip::address &address,
		unsigned short port,
		const std::string &channelName,
		const boost::posix_time::microseconds timeout)
		: subscriber(ioService),
		connectSubscriberTimer(ioService),
		address(address), port(port), channelName(channelName), timeout(timeout)
	{
		subscriber.installErrorHandler(std::bind(&RedisAsyncSubscriber::connectSubscriber, this));
	}

	void start()
	{
		connectSubscriber();
	}

protected:

	void errorSubProxy(const std::string &)
	{
		connectSubscriber();
	}

	void connectSubscriber()
	{
		std::cerr << "connectSubscriber\n";

		if (subscriber.state() == RedisAsyncClient::State::Connected ||
		subscriber.state() == RedisAsyncClient::State::Subscribed)
		{
		std::cerr << "disconnectSubscriber\n";
		subscriber.disconnect();
		}

		boost::asio::ip::tcp::endpoint endpoint(address, port);
		subscriber.connect(endpoint,
		std::bind(&RedisAsyncSubscriber::onSubscriberConnected, this, std::placeholders::_1));
	}

	void callLater(boost::asio::deadline_timer &timer,
		void(RedisAsyncSubscriber::*callback)())
	{
		std::cerr << "callLater\n";
		timer.expires_from_now(timeout);
		timer.async_wait([callback, this](const boost::system::error_code &ec) {
			if (!ec)
			{
				(this->*callback)();
			}
		});
	}

	void onSubscriberConnected(boost::system::error_code ec)
	{
		if (ec)
		{
			std::cerr << "onSubscriberConnected: can't connect to redis: " << ec.message() << "\n";
			callLater(connectSubscriberTimer, &RedisAsyncSubscriber::connectSubscriber);
		}
		else
		{
			std::cerr << "onSubscriberConnected ok\n";
			subscriber.subscribe(channelName,
				std::bind(&RedisAsyncSubscriber::onMessage, this, std::placeholders::_1));
			subscriber.psubscribe("*",
				std::bind(&RedisAsyncSubscriber::onMessage, this, std::placeholders::_1));
		}
	}

	void onMessage(const std::vector<char> &buf)
	{
		std::string s(buf.begin(), buf.end());
		std::string sGBK = boost::locale::conv::between(s, "GBK", "UTF-8");
		std::cout << "onMessage: " << sGBK << "\n";
	}

private:
	boost::asio::deadline_timer connectSubscriberTimer;
	const boost::asio::ip::address address;
	const unsigned short port;
	const std::string &channelName;
	const boost::posix_time::microseconds timeout;

	RedisAsyncClient subscriber;
};

#include <redisclient\redissyncclient.h>
BOOL sync_test(std::map<std::string, std::string> & ssmap)
{
	BOOL bResult = EXIT_FAILURE;
	//putenv("REDIS_HOST=192.168.31.86");
	putenv("REDIS_HOST=47.52.199.85");
	const char * host = getenv("REDIS_HOST");
	boost::asio::ip::address address = boost::asio::ip::address::from_string(host ? host : "127.0.0.1");
	//boost::asio::ip::address address = boost::asio::ip::address::from_string("127.0.0.1");
	const unsigned short port = 6379;
	boost::asio::ip::tcp::endpoint endpoint(address, port);

	boost::asio::io_service ioService;
	redisclient::RedisSyncClient redis(ioService);
	boost::system::error_code ec;

	redis.connect(endpoint, ec);

	if (ec)
	{
		std::cerr << "Can't connect to redis: " << ec.message() << std::endl;
		return EXIT_FAILURE;
	}

	redisclient::RedisValue result;

	//result = redis.command("SET", { "key1", "value" });

	//if (result.isError())
	//{
	//	std::cerr << "SET error: " << result.toString() << "\n";
	//	return EXIT_FAILURE;
	//}
	for (auto it =ssmap.begin(); it!=ssmap.end(); it++)
	{
		result = redis.command("GET", { it->first.c_str() });

		if (result.isOk())
		{
			if (it->second.compare(result.toString()))
			{
				std::cout << "GET: " << it->first.c_str() << "=" << result.toString() << "\n";
				bResult = EXIT_SUCCESS;
				it->second = result.toString();
			}
		}
		else
		{
			std::cerr << "GET error: " << it->first.c_str() << "=" << result.toString() << "\n";
			bResult = EXIT_FAILURE;
		}
	}	
	return bResult;
}

__inline static void string_split_to_vector(std::vector<std::string> & sv, std::string strData, std::string strSplitter, std::string::size_type stPos = 0)
{
	std::string strTmp = ("");
	std::string::size_type stIdx = 0;
	std::string::size_type stSize = strData.length();

	while ((stPos = strData.find(strSplitter, stIdx)) != std::string::npos)
	{
		strTmp = strData.substr(stIdx, stPos - stIdx);
		if (!strTmp.length())
		{
			strTmp = ("");
		}
		sv.push_back(strTmp);

		stIdx = stPos + strSplitter.length();
	}

	if (stIdx < stSize)
	{
		strTmp = strData.substr(stIdx, stSize - stIdx);
		if (!strTmp.length())
		{
			strTmp = ("");
		}
		sv.push_back(strTmp);
	}
}

std::string GetProgramDir()
{
	char szFullPath[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, szFullPath, MAX_PATH);
	*(strrchr(szFullPath, ('\\')) + sizeof(char)) = '\x00';
	return szFullPath;  // Return the directory without the file name 
}

int main(int argc, char ** argv)
{
	/*{
		CHAR cAction[MAXBYTE] = { 0 };
		CHAR cFlags[MAXBYTE] = { 0 };
		CHAR cType[MAXBYTE] = { 0 };
		CHAR cLogin[MAXBYTE] = { 0 };
		CHAR cSymbol[MAXBYTE] = { 0 };
		CHAR cVolume[MAXBYTE] = { 0 };
		CHAR cPriceOrder[MAXBYTE] = { 0 };
		CHAR cTypeFill[MAXBYTE] = { 0 };
		CHAR cPriceDeviation[MAXBYTE] = { 0 };
		CHAR cAppName[MAX_PATH] = "OPEN";
		CHAR cOpenFile[MAX_PATH] = "C:\\open.ini";

		WCHAR wAction[MAXBYTE] = { 0 };
		WCHAR wFlags[MAXBYTE] = { 0 };
		WCHAR wType[MAXBYTE] = { 0 };
		WCHAR wLogin[MAXBYTE] = { 0 };
		WCHAR wSymbol[MAXBYTE] = { 0 };
		WCHAR wVolume[MAXBYTE] = { 0 };
		WCHAR wPriceOrder[MAXBYTE] = { 0 };
		WCHAR wTypeFill[MAXBYTE] = { 0 };
		WCHAR wPriceDeviation[MAXBYTE] = { 0 };
		WCHAR wAppName[MAX_PATH] = L"OPEN";
		WCHAR wOpenFile[MAX_PATH] = L"C:\\open.ini";

		::GetPrivateProfileStringA(cAppName, "Action", "1", cAction, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "Flags", "2", cFlags, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "Type", "0", cType, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "Login", "1004", cLogin, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "Symbol", "AUDJPY", cSymbol, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "Volume", "1", cVolume, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "PriceOrder", "82.342", cPriceOrder, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "TypeFill", "0", cTypeFill, MAXBYTE, cOpenFile);
		::GetPrivateProfileStringA(cAppName, "PriceDeviation", "1000000", cPriceDeviation, MAXBYTE, cOpenFile);

		::GetPrivateProfileStringW(wAppName, L"Action", L"1", wAction, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"Flags", L"2", wFlags, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"Type", L"0", wType, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"Login", L"1004", wLogin, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"Symbol", L"AUDJPY", wSymbol, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"Volume", L"1", wVolume, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"PriceOrder", L"82.342", wPriceOrder, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"TypeFill", L"0", wTypeFill, MAXBYTE, wOpenFile);
		::GetPrivateProfileStringW(wAppName, L"PriceDeviation", L"1000000", wPriceDeviation, MAXBYTE, wOpenFile);


		UINT nAction = std::stoul(wAction);//IMTRequest::EnTradeActions::TA_REQUEST //开单
		UINT nFlags = std::stoul(wFlags);//IMTRequest::EnTradeActionFlags::TA_FLAG_MARKET //市价
		UINT nType = std::stoul(wType);//IMTOrder::EnOrderType::OP_BUY //开仓方向
		UINT64 nLogin = std::stoull(wLogin);//交易账号
		wSymbol;//交易品种
		UINT64 nVolume = std::stoull(wVolume);//开仓量(手数)
		double dPriceOrder = std::stod(wPriceOrder);//开仓价格
		UINT64 nTypeFill = std::stoul(wTypeFill);//IMTOrder::EnOrderFilling::ORDER_FILL_FOK
		UINT64 nPriceDeviation = std::stoull(wPriceDeviation);

		return 0;
	}*/
	
	/*std::vector<std::string> sv;
	std::map<std::string, std::string> ssmap;
	CHAR szContractList[USHRT_MAX] = { 0 };
	::GetPrivateProfileStringA(("PUBLIC"), ("CONTRACTS"), (""), szContractList, sizeof(szContractList) / sizeof(*szContractList), (GetProgramDir() + "common.conf").c_str());
	string_split_to_vector(sv, szContractList, ("|"));
	std::for_each(sv.begin(), sv.end(), [&](const std::vector<std::string>::value_type & it) {ssmap.insert(std::map<std::string, std::string>::value_type(it, {""})); });
	while (true)
	{
		sync_test(ssmap);
		Sleep(100);
	}
	getchar();*/
	putenv("REDIS_HOST=127.0.0.1");
	const char * host = getenv("REDIS_HOST");
	boost::asio::ip::address address = boost::asio::ip::address::from_string(host ? host : "127.0.0.1");
	const unsigned short port = 6379;

	boost::asio::io_service ioService;
	std::string channelName = "unique-channel-0";
	boost::posix_time::microseconds timeout(1000);
	RedisAsyncSubscriber subscriber(ioService, address, port, channelName, timeout);
	
	subscriber.start();
	
	ioService.run();

	std::cerr << "done\n";

	return 0;
}
