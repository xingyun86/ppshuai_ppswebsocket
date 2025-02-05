#ifndef __GZIP_H__
#define __GZIP_H__

#include <sstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

class CGZip {
public:
	static std::string compress(const std::string& data)
	{
		std::stringstream compressed;
		std::stringstream decompressed(data);

		boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
		out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
		out.push(decompressed);
		boost::iostreams::copy(out, compressed);

		return compressed.str();
	}

	static std::string decompress(const std::string& data)
	{
		std::stringstream compressed(data);
		std::stringstream decompressed;

		boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
		out.push(boost::iostreams::gzip_decompressor());
		out.push(compressed);
		boost::iostreams::copy(out, decompressed);

		return decompressed.str();
	}
};

#endif // __GZIP_H__