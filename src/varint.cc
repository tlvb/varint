#include "varint.h"
varint::type varint::classify(int64_t i) { /*{{{*/
	if (i < -3) {
		return varint::type::neg_r;
	}
	else if (i < 0) {
		return varint::type::neg_2;
	}
	else if (i < 128) {
		// zero is classified as a positive number here
		return varint::type::pos_7;
	}
	else if (i & ~0x3ff == 0) {
		return varint::type::pos_14;
	}
	else if (i & ~0x1fffff == 0) {
		return varint::type::pos_21;
	}
	else if (i & ~0xfffffff == 0) {
		return varint::type::pos_28;
	}
	else if (i & ~0xffffffff == 0) {
		return varint::type::pos_32;
	}
	return varint::type::pos_64;
} /*}}}*/
unsigned int varint::size(varint::type t) { /*{{{*/
	switch (t) {
		case varint::type::neg_r:
			return 0;
		case varint::type::neg_2:
		case varint::type::pos_7:
			return 1;
		case varint::type::pos_14:
			return 2;
		case varint::type::pos_21:
			return 3;
		case varint::type::pos_28:
			return 4;
		case varint::type::pos_32:
			return 5;
		case varint::type::pos_64:
			return 9;
	}
} /*}}}*/
unsigned int varint::size(int64_t i) { /*{{{*/
	varint::type t = varint::classify(i);
	if (t == varint::type::neg_r) {
		return 1+varint::size(-i);
	}
	return varint::size(t);
} /*}}}*/
unsigned int varint::to_buffer(uint8_t *buf, int limit, int64_t integer) { /*{{{*/
	varint::type t = varint::classify(integer);
	int s = varint::size(t);
	if (s == 0) {
		s = varint::size(-integer);
	}
	if (limit < s) {
		return 0;
	}
	switch (t) {
		case varint::type::neg_r:
			buf[0] = 0b11111000;
			return 1+varint::to_buffer(buf+1, limit-1, -integer);
		case varint::type::neg_2:
			buf[0] = 0b11111100 | ((-integer) & 0b00000011);
			return 1;
		case varint::type::pos_7:
			buf[0] = 0b01111111 & integer;
			return 1;
		case varint::type::pos_14:
			buf[0] = 0b10000000 | (0b00111111 & (integer>>8));
			buf[1] = (integer & 0xff);
			return 2;
		case varint::type::pos_21:
			buf[0] = 0b11000000 | (0b00011111 & (integer>>17));
			buf[1] = (integer >> 8) & 0xff;
			buf[2] = (integer >> 0) & 0xff;
			return 3;
		case varint::type::pos_28:
			buf[0] = 0b11100000 | (0b00001111 & (integer>>26));
			buf[1] = (integer >> 16) & 0xff;
			buf[2] = (integer >>  8) & 0xff;
			buf[3] = (integer >>  0) & 0xff;
			return 4;
		case varint::type::pos_32:
			buf[0] = 0b11110000;
			buf[1] = (integer >> 24) & 0xff;
			buf[2] = (integer >> 16) & 0xff;
			buf[3] = (integer >>  8) & 0xff;
			buf[4] = (integer >>  0) & 0xff;
			return 5;
		case varint::type::pos_64:
			buf[0] = 0b11110100;
			buf[1] = (integer >> 56) & 0xff;
			buf[2] = (integer >> 48) & 0xff;
			buf[3] = (integer >> 40) & 0xff;
			buf[4] = (integer >> 32) & 0xff;
			buf[5] = (integer >> 24) & 0xff;
			buf[6] = (integer >> 16) & 0xff;
			buf[7] = (integer >>  8) & 0xff;
			buf[8] = (integer >>  0) & 0xff;
			return 9;
	}
} /*}}}*/
unsigned int varint::size(void) const { /*{{{*/
	return varint::size(this->integer);
} /*}}}*/
unsigned int varint::from_buffer(const uint8_t *buf, int limit) { /*{{{*/
	if (limit == 0) { return 0; }
	if ((buf[0] & 0b11111100) == 0b11111000) {
		int n = this->from_buffer(buf+1, limit-1);
		if (n == 0) { return 0; }
		this->integer *= -1;
		return 1+n;
	}
	else if ((buf[0] & 0b11111100) == 0b11111100) {
		this->integer = -static_cast<int64_t>(0b00000011 & buf[0]);
		return 1;
	}
	else if ((buf[0] & 0b10000000) == 0) {
		this->integer = buf[0];
		return 1;
	}
	else if ((buf[0] & 0b11000000) == 0b10000000) {
		if (limit < 2) { return 0; }
		this->integer = (static_cast<uint64_t>(buf[0]&0b00111111)<<8) | buf[1];
		return 2;
	}
	else if ((buf[0] & 0b11100000) == 0b11000000) {
		if (limit < 3) { return 0; }
		this->integer = (static_cast<uint64_t>(buf[0]&0b00011111)<<16) | (static_cast<uint64_t>(buf[1])<<8) | buf[2];
		return 3;
	}
	else if ((buf[0] & 0b11110000) == 0b11100000) {
		if (limit < 4) { return 0; }
		this->integer = (static_cast<uint64_t>(buf[0]&0b00001111)<<24) | (static_cast<uint64_t>(buf[1])<<16) | (static_cast<uint64_t>(buf[2])<<8) | buf[3];
		return 4;
	}
	else if ((buf[0] & 0b11111100) == 0b11110000) {
		if (limit < 5) { return 0; }
		this->integer = (static_cast<uint64_t>(buf[1])<<24) | (static_cast<uint64_t>(buf[2])<<16) | (static_cast<uint64_t>(buf[3])<<8) | buf[4];
		return 5;
	}
	else if ((buf[0] & 0b11111100) == 0b11110100) {
		if (limit < 9) { return 0; }
		this->integer = (static_cast<uint64_t>(buf[1])<<56) | (static_cast<uint64_t>(buf[2])<<48) | (static_cast<uint64_t>(buf[3])<<40) | (static_cast<uint64_t>(buf[4])<<32) | (static_cast<uint64_t>(buf[5])<<24) | (static_cast<uint64_t>(buf[6])<<16) | (static_cast<uint64_t>(buf[7])<<8) | buf[8];
		return 9;
	}
	return 0;
} /*}}}*/
unsigned int varint::to_buffer(uint8_t *buf, int limit) const { /*{{{*/
	return varint::to_buffer(buf, limit, this->integer);
} /*}}}*/
varint::operator int64_t(void) const { /*{{{*/
	return this->integer;
} /*}}}*/
varint& varint::operator=(const int64_t& other) { /*{{{*/
	this->integer = other;
	return *this;
} /*}}}*/
varint& varint::operator=(const int64_t&& other) { /*{{{*/
	this->integer = other;
	return *this;
} /*}}}*/

