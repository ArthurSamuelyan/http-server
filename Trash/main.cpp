#include <boost\extended\serialization\json.hpp>
#include <boost\extended\serialization\opaque_ptr.hpp>
#include <boost\extended\application\service.hpp>

#include <boost/predef.h> // Tools to identify the OS.

#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0601

#if _WIN32_WINNT <= 0x0502 // Windows Server 2003 or earlier.
#define BOOST_ASIO_ENABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO	
#endif
#endif

#include <boost\asio.hpp>
#include <boost\thread.hpp>

#include <boost/move/utility_core.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <iostream>
#include <thread>
#include <future>
#include <memory>
#include <atomic>
#include <string>
#include <chrono>
#include <locale>
#include <fstream>
#include <locale>

using namespace boost::asio;
using namespace boost::extended::serialization;

//namespace logging = boost::log;
//namespace sinks = boost::log::sinks;
//namespace src = boost::log::sources;
//namespace expr = boost::log::expressions;
//namespace attrs = boost::log::attributes;
//namespace keywords = boost::log::keywords;

std::atomic < bool > flag;

void resolver()
{
	std::string host = "olymptrade.com";
	std::string port = "2001";

	io_service service;

	ip::tcp::resolver::query resolver_query(host, port, ip::tcp::resolver::query::numeric_service);

	ip::tcp::resolver resolver(service);

	ip::tcp::resolver::iterator iterator = resolver.resolve(resolver_query);

	ip::tcp::resolver::iterator iterator_end;

	for (; iterator != iterator_end; ++iterator) 
	{
		ip::tcp::endpoint endpoint = iterator->endpoint();
		std::cout << endpoint.address().to_string() << ' ' << endpoint.port() << ' ' << endpoint.protocol().family() << std::endl;
	}
}

struct Session 
{
	std::shared_ptr < ip::tcp::socket > socket;
	std::string buf;
	//std::size_t total_bytes_written; // write_some
};

void callback_free(const boost::system::error_code & ec, std::size_t bytes_transferred/*, std::shared_ptr < Session > s*/)
{
	if (ec != 0)
	{
		std::cout << "Error occured! Error code = "
			<< ec.value()
			<< ". Message: "
			<< ec.message()
			<< "\n";

		return;
	}


	std::cout << "callback exec ... \n";
	// Here we know that all the data has
	// been written to the socket.
}

//void callback(const boost::system::error_code & ec, std::size_t bytes_transferred, std::shared_ptr < Session > s)
//{
//	if (ec != 0) 
//	{
//		std::cout << "Error occured! Error code = "
//				  << ec.value()
//				  << ". Message: " 
//				  << ec.message()
//				  << "\n";
//
//		return;
//	}
//
//	s->total_bytes_written += bytes_transferred;
//
//	if (s->total_bytes_written == s->buf.length()) 
//		return;
//	
//	s->socket->async_write_some(
//		buffer(s->buf.c_str() +	s->total_bytes_written,	s->buf.length() - s->total_bytes_written),
//		std::bind(callback, std::placeholders::_1, std::placeholders::_2, s));
//}

struct Server_Session 
{
	std::shared_ptr < ip::tcp::socket > socket;
	std::unique_ptr < char[] > buf;
	//std::size_t total_bytes_read;
	std::size_t buf_size;
};

void server_callback(const boost::system::error_code & ec, std::size_t bytes_transferred/*, std::shared_ptr < Server_Session > s*/)
{
	if (ec != 0)
	{
		std::cout << "Error occured! Error code = "
			<< ec.value()
			<< ". Message: "
			<< ec.message()
			<< "\n";

		return;
	}

	//s->total_bytes_read += bytes_transferred;

	//if (s->total_bytes_read == s->buf_size) 
	//{
	//	std::cout << "server callback exec ... \n";
	//	return;
	//}
	//
	//s->socket->async_read_some(buffer(s->buf.get() + s->total_bytes_read, s->buf_size - s->total_bytes_read),
	//	std::bind(server_callback, std::placeholders::_1, std::placeholders::_2, s));
}

void read_from_socket(std::shared_ptr < Server_Session > s)
{
	async_read(*(s->socket), buffer(s->buf.get(), s->buf_size),
		std::bind(server_callback,	std::placeholders::_1, std::placeholders::_2/*, s*/));
}

//std::string read_from_socket(ip::tcp::socket & socket) 
//{
//	//const unsigned char MESSAGE_SIZE = 7;
//	//char buf[MESSAGE_SIZE];
//	//std::size_t total_bytes_read = 0;
//
//	//while (total_bytes_read != MESSAGE_SIZE) 
//	//{
//	//	total_bytes_read += socket.read_some(buffer(buf + total_bytes_read, MESSAGE_SIZE - total_bytes_read));
//	//}
//	//return std::string(buf, total_bytes_read);
//
//	//const unsigned char MESSAGE_SIZE = 5;
//	//char buf[MESSAGE_SIZE];
//	//
//	//read(socket, buffer(buf, MESSAGE_SIZE));
//	//
//	//return std::string(buf, MESSAGE_SIZE);
//
//	streambuf buf;
//	read_until(socket, buf, '\n');
//
//	std::string message;
//
//	std::istream input_stream(&buf);
//	std::getline(input_stream, message);
//
//	return message;
//}

void process_request(ip::tcp::socket & socket) 
{
	// We use extensibel buffer because we don't
	// know the size of the request message.
	streambuf request_buf;

	boost::system::error_code ec;

	// Receiving the request.
	read(socket, request_buf, ec);

	if (ec != error::eof)
		throw boost::system::system_error(ec);
	else
	{
		std::cout << "server request!\n";
	}

	// Request received. Sending response.
	// Allocating and filling the buffer with
	// binary data.
	const char response_buf[] = { 0x48, 0x69, 0x21 };

	// Sending the request data.
	write(socket, buffer(response_buf));

	// Shutting down the socket to let the
	// client know that we've sent the whole
	// response.
	socket.shutdown(socket_base::shutdown_send);
}

void server()
{
	const int BACKLOG_SIZE = 30;
	
	unsigned short port = 3333;
	ip::address ip_address = ip::address_v4::any();
	ip::tcp::endpoint endpoint(ip_address, port);

	io_service service;
	ip::tcp::acceptor acceptor(service, endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen(BACKLOG_SIZE);

	std::shared_ptr < ip::tcp::socket > socket(new ip::tcp::socket(service));

	//ip::tcp::socket socket(service);
	acceptor.accept(*socket);

	std::cout << "acception successful" << std::endl;

	//std::string message;
	//message = read_from_socket(socket);
	//std::cout << message << std::endl;

	std::shared_ptr < Server_Session > s(new Server_Session);

	const unsigned int MESSAGE_SIZE = 17;

	s->buf.reset(new char[MESSAGE_SIZE]);

	//s->total_bytes_read = 0;
	s->socket = socket;
	s->buf_size = MESSAGE_SIZE;

	read_from_socket(s);

	service.run();

	std::string str((s->buf).get(), MESSAGE_SIZE);
	std::cout << str << std::endl;

	process_request(*socket);

	while (!flag.load());
}

void write_to_socket(std::shared_ptr < Session > s) // ip::tcp::socket & socket
{
	//std::string buf = "Hello Boost.asio!\n";

	//std::size_t total_bytes_written = 0;

	//while (total_bytes_written != buf.length()) 
	//{
	//	total_bytes_written += socket.write_some(buffer(buf.c_str() + total_bytes_written, buf.length() - total_bytes_written)); // socket.send(buffer(buf));
	//}
	// write(socket, buffer(buf)); // целиком
	//std::cout << "written!\n";

	//s->socket->async_write_some(buffer(s->buf), std::bind(callback, std::placeholders::_1, std::placeholders::_2, s));
	async_write(*(s->socket), buffer(s->buf), std::bind(callback_free, std::placeholders::_1, std::placeholders::_2/*, s*/));
	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void communicate(ip::tcp::socket & socket) 
{
	// Allocating and filling the buffer with
	// binary data.
	const char request_buf[] = { 0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f };

	// Sending the request data.
	write(socket, buffer(request_buf));

	// Shutting down the socket to let the
	// server know that we've sent the whole
	// request.
	socket.shutdown(socket_base::shutdown_send);

	// We use extensible buffer for response
	// because we don't know the size of the
	// response message.
	streambuf response_buf;

	boost::system::error_code ec;
	read(socket, response_buf, ec);

	if (ec == error::eof) 
	{
		// Whole response message has been received.
		// Here we can handle it.
		std::cout << "client correct!\n";
	}
	else 
	{
		throw boost::system::system_error(ec);
	}
}

//class Widget 
//{
//public:
//	Widget();
//	~Widget();
//	void foo() const; // <- константный метод
//private:
//	struct Impl;
//	opaque_ptr<Impl> m_ptr;
//};
//
//struct Widget::Impl { int i = 0; };
//Widget::Widget() : m_ptr(nullptr) {}
//Widget::~Widget() {}
//
//void Widget::foo() const {
//	m_ptr->i = 42; // <- изменение данных внутри константного метода
//}

int main(int argc, char ** argv)
{
	std::setlocale(LC_ALL, "Russian");

	flag.store(false);

	std::future < void > result = std::async(std::launch::async, server);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// =============================================================================

	std::string raw_ip_address = "127.0.0.1";
	unsigned short port = 3333;
	boost::system::error_code ec;
	ip::address ip_address = ip::address::from_string(raw_ip_address, ec);
	if (ec.value() != 0) 
	{
		std::cout << "Failed to parse the IP address. Error code = " << ec.value() << ". Message: " << ec.message();

		return ec.value();
	}
	ip::tcp::endpoint endpoint(ip_address, port);
	
	io_service service;

	// =============================================================================
	/*
	ip::tcp::resolver::query resolver_query("mail.yandex.ru", "3333", ip::tcp::resolver::query::numeric_service);
	ip::tcp::resolver resolver(service);

	try 
	{
		ip::tcp::resolver::iterator iterator = resolver.resolve(resolver_query);

		ip::tcp::socket socket(service);

		connect(socket, iterator);

		std::cout << "success" << std::endl;
	}
	catch (boost::system::system_error & e) 
	{
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		// return e.code().value();
	}
	*/
	// =============================================================================

	try 
	{

		std::shared_ptr < ip::tcp::socket > socket(new ip::tcp::socket(service, endpoint.protocol()));

		socket->connect(endpoint);

		std::shared_ptr < Session > s(new Session);

		s->buf = std::string("Hello Boost.asio!\n");
		//s->total_bytes_written = 0; // write_some
		s->socket = socket;

		//ip::tcp::socket socket(service, endpoint.protocol());
		//socket.connect(endpoint);
		std::cout << "connection successful" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "waiting ... " << std::endl;
		//for (int i = 0; i < 100; ++i)
		{
			write_to_socket(s); // socket->cancel();
		}

		communicate(*socket);
		
		service.run();
		// std::cout << read_from_socket(socket) << std::endl;
	}
	catch (boost::system::system_error & e) 
	{
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;
	}

	// =============================================================================

	std::shared_ptr < ip::tcp::socket > socket(new ip::tcp::socket(service, endpoint.protocol()));

	socket->async_connect(endpoint, [socket](const boost::system::error_code & ec)
	{
		if (ec != 0) 
		{
			if (ec == error::operation_aborted) 
			{
				std::cout << "Operation cancelled!";
			}
			else 
			{
				std::cout << "Error occured!"
					<< " Error code = "
					<< ec.value()
					<< ". Message: "
					<< ec.message();
			}

			return;
		}
	});

	// Starting a thread, which will be used
	// to call the callback when asynchronous 
	// operation completes.
	std::thread worker_thread([&service]() 
	{
		try 
		{
			service.run();
		}
		catch (boost::system::system_error & e) 
		{
			std::cout << "Error occured!"
				<< " Error code = " << e.code()
				<< ". Message: " << e.what();
		}
	});

	std::this_thread::sleep_for(std::chrono::seconds(2));

	socket->cancel();

	worker_thread.join();

	// =============================================================================

	flag.store(true);
	result.wait();

	system("pause");

	return EXIT_SUCCESS;
}