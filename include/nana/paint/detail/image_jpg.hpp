#ifndef NANA_PAINT_DETAIL_IMAGE_JPG_HPP
#define NANA_PAINT_DETAIL_IMAGE_JPG_HPP

#include "image_impl_interface.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "nana\extrlib\stb_image.h"

#include <stdio.h>
#include "../pixel_buffer.hpp"

namespace nana
{
	namespace paint{	namespace detail{

		class image_jpg
			: public image::image_impl_interface
		{
		public:
			image_jpg()
			{
			}

			bool open(const nana::char_t* jpg_file)
			{
				int x, y, n;
#ifdef NANA_UNICODE
				unsigned char *data = stbi_load(static_cast<std::string>(nana::charset(jpg_file)).c_str(), &x, &y, &n, 0);
#else
				unsigned char *data = stbi_load(jpg_file, &x, &y, &n, 0);
#endif
				if (nullptr == data) return false;

				std::size_t bytes_per_line = (x * (n));

				pixbuf_.open(x, y);
				//auto d = pixbuf_.raw_ptr(0);
				const auto len = (x * y * n);
				unsigned char *start, *end;
				printf("JPG DATA: %d, %d, %d, %d, %d, %d\n", x, y, n, bytes_per_line, len, 0);
				for (start = (unsigned char *)data, end = start + len - 1; start < end; ++start, --end)
				{
					unsigned char swap = *start;
					*start = *end;
					*end = swap;
				}
				for (int h = 0; h < y; ++h) {
					byte* row = data + h * x * n;
					for (int w = 0; w < x / 2; ++w) {
						byte* left = row + w * n;
						byte* right = row + (x - w - 1) * n;
						for (int b = 0; b < n; ++b) {
							std::swap(left[b], right[b]);
						}
					}
				}
				pixbuf_.put(data, x, y, 24, bytes_per_line, false);
				stbi_image_free(data);
				return true;
			}

			virtual bool empty() const
			{
				return pixbuf_.empty();
			}

			virtual void close()
			{
				pixbuf_.close();
			}

			bool alpha_channel() const
			{
				return false;
			}

			virtual nana::size size() const
			{
				return pixbuf_.size();
			}

			void paste(const nana::rectangle& src_r, graph_reference graph, int x, int y) const
			{
				pixbuf_.paste(src_r, graph.handle(), x, y);
			}

			void stretch(const nana::rectangle& src_r, graph_reference dst, const nana::rectangle& r) const
			{
				pixbuf_.stretch(src_r, dst.handle(), r);
			}
		private:
			nana::paint::pixel_buffer pixbuf_;

		};
	}//end namespace detail
	}//end namespace paint
}//end namespace nana

#endif
