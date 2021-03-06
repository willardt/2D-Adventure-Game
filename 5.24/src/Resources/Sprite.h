
#include "Texture.h"

#ifndef SPRITE_H
#define SPRITE_H

struct Sprite : public Texture {

	struct Frame {
		int x;
		int y;
	};

	Sprite() :
		Texture(),
		idle		( 0 ),
		end			( 0 ),
		run			( 0 ),
		cast		( 0 ),
		special		( 0 ),
		min_idle    ( 0 ),   max_idle    ( 0 ),
		min_up		( 0 ),	 max_up		 ( 0 ),
		min_down	( 0 ),	 max_down	 ( 0 ),
		min_left	( 0 ),	 max_left	 ( 0 ),
		min_right	( 0 ),	 max_right	 ( 0 ),
		num_frames	( 0 ),
		_frames		( nullptr )
	{}

	~Sprite() {
		if (_frames != nullptr) {
			delete[] _frames;
		}
	}

	static const unsigned int SPRITE_SHEET_WIDTH = 8;

	Uint8
		idle, end,
		run, cast, special,
		min_idle, max_idle,
		min_up, max_up,
		min_down, max_down,
		min_left, max_left,
		min_right, max_right,
		num_frames;
public:
	void load_frames();
	const Frame get_frame(Uint8 index);
private:
	Frame *_frames;
};


#endif
