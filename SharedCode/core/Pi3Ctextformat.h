#pragma once



struct Pi3CtextFormat {

	enum FT_Justify { LEFT_JUSTIFY, CENTER_JUSTIFY, RIGHT_JUSTIFY, FULL_JUSTIFY };

	bool italic = false;
	bool bold = false;
	bool tiny = false;
	bool deleted = false;
	bool subscript = false;
	bool superscript = false;
	bool insert = false;
	bool marked = false;
	//HTML entity
	bool nobreak = false;
	FT_Justify justify = LEFT_JUSTIFY;
	bool endJustify = false;

	float italicSlant = 0.f;
	float scale = 1.f;
	float scaleYoffset = 0;

	void recalc()
	{
		italicSlant = (italic) ? 4.f : 0.f;
		//scale = (superscript) ? 0.6f : (subscript) ? 0.6f : scale;
	}
};