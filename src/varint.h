#include <cstdint>
class varint { /*{{{*/
	public:
		enum class type {
			pos_7,
			pos_14,
			pos_21,
			pos_28,
			pos_32,
			pos_64,
			neg_2,
			neg_r
		};

		static varint::type classify(int64_t i);
		static unsigned int size(int64_t i);
		static unsigned int size(varint::type t);
	private:
		int64_t integer = 0;
		static unsigned int to_buffer(uint8_t *buf, int limit, int64_t integer);
	public:
		unsigned int size(void) const;
		unsigned int from_buffer(const uint8_t *buf, int limit);
		unsigned int to_buffer(uint8_t *buf, int limit) const;
		operator int64_t(void) const;
		varint& operator=(const int64_t& other);
		varint& operator=(const int64_t&& other);
}; /*}}}*/

