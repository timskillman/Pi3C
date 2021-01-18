#pragma once
#include <string>

struct Pi3Ccolours {
	//Based on HTML/CSS colour names - https://www.w3schools.com/colors/colors_names.asp
	enum {
		AliceBlue = 0xFFFFF8F0,
		AntiqueWhite = 0xFFD7EBFA,
		Aqua = 0xFFFFFF00,
		Aquamarine = 0xFFD4FF7F,
		Azure = 0xFFFFFFF0,
		Beige = 0xFFDCF5F5,
		Bisque = 0xFFC4E4FF,
		Black = 0xFF000000,
		BlanchedAlmond = 0xFFCDEBFF,
		Blue = 0xFFFF0000,
		BlueViolet = 0xFFE22B8A,
		Brown = 0xFF2A2AA5,
		BurlyWood = 0xFF87B8DE,
		CadetBlue = 0xFFA09E5F,
		Chartreuse = 0xFF00FF7F,
		Chocolate = 0xFF1E69D2,
		Coral = 0xFF507FFF,
		CornflowerBlue = 0xFFED9564,
		Cornsilk = 0xFFDCF8FF,
		Crimson = 0xFF3C14DC,
		Cyan = 0xFFFFFF00,
		DarkBlue = 0xFF8B0000,
		DarkCyan = 0xFF8B8B00,
		DarkGoldenrod = 0xFF0B86B8,
		DarkGray = 0xFFA9A9A9,
		DarkGreen = 0xFF006400,
		DarkKhaki = 0xFF6BB7BD,
		DarkMagenta = 0xFF8B008B,
		DarkOliveGreen = 0xFF2F6B55,
		DarkOrange = 0xFF008CFF,
		DarkOrchid = 0xFFCC3299,
		DarkRed = 0xFF00008B,
		DarkSalmon = 0xFF7A96E9,
		DarkSeaGreen = 0xFF8BBC8F,
		DarkSlateBlue = 0xFF8B3D48,
		DarkSlateGray = 0xFF4F4F2F,
		DarkTurquoise = 0xFFD1CE00,
		DarkViolet = 0xFFD30094,
		DeepPink = 0xFF9314FF,
		DeepSkyBlue = 0xFFFFBF00,
		DimGray = 0xFF696969,
		DodgerBlue = 0xFFFF901E,
		Firebrick = 0xFF2222B2,
		FloralWhite = 0xFFF0FAFF,
		ForestGreen = 0xFF228B22,
		Fuchsia = 0xFFFF00FF,
		Gainsboro = 0xFFDCDCDC,
		GhostWhite = 0xFFFFF8F8,
		Gold = 0xFF00D7FF,
		Goldenrod = 0xFF20A5DA,
		Gray = 0xFF808080,
		Graphite = 0xFF202020,
		Green = 0xFF008000,
		GreenYellow = 0xFF2FFFAD,
		Honeydew = 0xFFF0FFF0,
		HotPink = 0xFFB469FF,
		IndianRed = 0xFF5C5CCD,
		Indigo = 0xFF82004B,
		Ivory = 0xFFF0FFFF,
		Khaki = 0xFF8CE6F0,
		Lavender = 0xFFFAE6E6,
		LavenderBlush = 0xFFF5F0FF,
		LawnGreen = 0xFF00FC7C,
		LemonChiffon = 0xFFCDFAFF,
		LightBlue = 0xFFE6D8AD,
		LightCoral = 0xFF8080F0,
		LightCyan = 0xFFFFFFE0,
		LightGoldenrodYellow = 0xFFD2FAFA,
		LightGray = 0xFFD3D3D3,
		LightGreen = 0xFF90EE90,
		LightPink = 0xFFC1B6FF,
		LightSalmon = 0xFF7AA0FF,
		LightSeaGreen = 0xFFAAB220,
		LightSkyBlue = 0xFFFACE87,
		LightSlateGray = 0xFF998877,
		LightSteelBlue = 0xFFDEC4B0,
		LightYellow = 0xFFE0FFFF,
		Lime = 0xFF00FF00,
		LimeGreen = 0xFF32CD32,
		Linen = 0xFFE6F0FA,
		Magenta = 0xFFFF00FF,
		Maroon = 0xFF000080,
		MediumAquamarine = 0xFFAACD66,
		MediumBlue = 0xFFCD0000,
		MediumOrchid = 0xFFD355BA,
		MediumPurple = 0xFFDB7093,
		MediumSeaGreen = 0xFF71B33C,
		MediumSlateBlue = 0xFFEE687B,
		MediumSpringGreen = 0xFF9AFA00,
		MediumTurquoise = 0xFFCCD148,
		MediumVioletRed = 0xFF8515C7,
		MidnightBlue = 0xFF701919,
		MintCream = 0xFFFAFFF5,
		MistyRose = 0xFFE1E4FF,
		Moccasin = 0xFFB5E4FF,
		NavajoWhite = 0xFFADDEFF,
		Navy = 0xFF800000,
		OldLace = 0xFFE6F5FD,
		Olive = 0xFF008080,
		OliveDrab = 0xFF238E6B,
		Orange = 0xFF00A5FF,
		OrangeRed = 0xFF0045FF,
		Orchid = 0xFFD670DA,
		PaleGoldenrod = 0xFFAAE8EE,
		PaleGreen = 0xFF98FB98,
		PaleTurquoise = 0xFFEEEEAF,
		PaleVioletRed = 0xFF9370DB,
		PapayaWhip = 0xFFD5EFFF,
		PeachPuff = 0xFFB9DAFF,
		Peru = 0xFF3F85CD,
		Pink = 0xFFCBC0FF,
		Plum = 0xFFDDA0DD,
		PowderBlue = 0xFFE6E0B0,
		Purple = 0xFF800080,
		Red = 0xFF0000FF,
		Raspberry = 0xFF4A1CC3,
		RosyBrown = 0xFF8F8FBC,
		RoyalBlue = 0xFFE16941,
		SaddleBrown = 0xFF13458B,
		Salmon = 0xFF7280FA,
		SandyBrown = 0xFF60A4F4,
		SeaGreen = 0xFF578B2E,
		SeaShell = 0xFFEEF5FF,
		Sienna = 0xFF2D52A0,
		Silver = 0xFFC0C0C0,
		SkyBlue = 0xFFEBCE87,
		SlateBlue = 0xFFCD5A6A,
		SlateGray = 0xFF908070,
		Snow = 0xFFFAFAFF,
		SpringGreen = 0xFF7FFF00,
		SteelBlue = 0xFFB48246,
		Tan = 0xFF8CB4D2,
		Teal = 0xFF808000,
		Thistle = 0xFFD8BFD8,
		Tomato = 0xFF4763FF,
		Transparent = 0xFFFFFF00,
		TransparentBlack = 0xD0000000,
		Turquoise = 0xFFD0E040,
		Violet = 0xFFEE82EE,
		Wheat = 0xFFB3DEF5,
		White = 0xFFFFFFFF,
		WhiteSmoke = 0xFFF5F5F5,
		Yellow = 0xFF00FFFF,
		YellowGreen = 0xFF32CD9A
	};

	static uint32_t getPi3Ccolour(const std::string& colour)
	{
		if (colour.substr(0,2) == "0x") {
			return (uint32_t)std::stol(colour, nullptr, 16);
		}

		char firstChar = colour[0];
		switch (firstChar) {
		case 'A':
			if (colour == "AliceBlue") return AliceBlue;
			else if (colour == "AntiqueWhite") return AntiqueWhite;
			else if (colour == "Aqua") return Aqua;
			else if (colour == "Aquamarine") return Aquamarine;
			else if (colour == "Azure") return Azure;
			break;
		case 'B':
			if (colour == "Beige") return Beige;
			else if (colour == "Bisque") return Bisque;
			else if (colour == "Black") return Black;
			else if (colour == "BlanchedAlmond") return BlanchedAlmond;
			else if (colour == "Blue") return Blue;
			else if (colour == "BlueViolet") return BlueViolet;
			else if (colour == "Brown") return Brown;
			else if (colour == "BurlyWood") return BurlyWood;
			break;
		case 'C':
			if (colour == "CadetBlue") return CadetBlue;
			else if (colour == "Chartreuse") return Chartreuse;
			else if (colour == "Chocolate") return Chocolate;
			else if (colour == "Coral") return Coral;
			else if (colour == "CornflowerBlue") return CornflowerBlue;
			else if (colour == "Cornsilk") return Cornsilk;
			else if (colour == "Crimson") return Crimson;
			else if (colour == "Cyan") return Cyan;
			break;
		case 'D':
			if (colour == "DarkBlue") return DarkBlue;
			else if (colour == "DarkCyan") return DarkCyan;
			else if (colour == "DarkGoldenrod") return DarkGoldenrod;
			else if (colour == "DarkGray") return DarkGray;
			else if (colour == "DarkGreen") return DarkGreen;
			else if (colour == "DarkKhaki") return DarkKhaki;
			else if (colour == "DarkMagenta") return DarkMagenta;
			else if (colour == "DarkOliveGreen") return DarkOliveGreen;
			else if (colour == "DarkOrange") return DarkOrange;
			else if (colour == "DarkOrchid") return DarkOrchid;
			else if (colour == "DarkRed") return DarkRed;
			else if (colour == "DarkSalmon") return DarkSalmon;
			else if (colour == "DarkSeaGreen") return DarkSeaGreen;
			else if (colour == "DarkSlateBlue") return DarkSlateBlue;
			else if (colour == "DarkSlateGray") return DarkSlateGray;
			else if (colour == "DarkTurquoise") return DarkTurquoise;
			else if (colour == "DarkViolet") return DarkViolet;
			else if (colour == "DeepPink") return DeepPink;
			else if (colour == "DeepSkyBlue") return DeepSkyBlue;
			else if (colour == "DimGray") return DimGray;
			else if (colour == "DodgerBlue") return DodgerBlue;
			break;
		case 'F':
			if (colour == "Firebrick") return Firebrick;
			else if (colour == "FloralWhite") return FloralWhite;
			else if (colour == "ForestGreen") return ForestGreen;
			else if (colour == "Fuchsia") return Fuchsia;
			break;
		case 'G':
			if (colour == "Gainsboro") return Gainsboro;
			else if (colour == "GhostWhite") return GhostWhite;
			else if (colour == "Gold") return Gold;
			else if (colour == "Goldenrod") return Goldenrod;
			else if (colour == "Gray") return Gray;
			else if (colour == "Graphite") return Graphite;
			else if (colour == "Green") return Green;
			else if (colour == "GreenYellow") return GreenYellow;
			break;
		case 'H':
			if (colour == "Honeydew") return Honeydew;
			else if (colour == "HotPink") return HotPink;
			break;
		case 'I':
			if (colour == "IndianRed") return IndianRed;
			else if (colour == "Indigo") return Indigo;
			else if (colour == "Ivory") return Ivory;
			break;
		case 'K':
			if (colour == "Khaki") return Khaki;
			break;
		case 'L':
			if (colour == "Lavender") return Lavender;
			else if (colour == "LavenderBlush") return LavenderBlush;
			else if (colour == "LawnGreen") return LawnGreen;
			else if (colour == "LemonChiffon") return LemonChiffon;
			else if (colour == "LightBlue") return LightBlue;
			else if (colour == "LightCoral") return LightCoral;
			else if (colour == "LightCyan") return LightCyan;
			else if (colour == "LightGoldenrodYellow") return LightGoldenrodYellow;
			else if (colour == "LightGray") return LightGray;
			else if (colour == "LightGreen") return LightGreen;
			else if (colour == "LightPink") return LightPink;
			else if (colour == "LightSalmon") return LightSalmon;
			else if (colour == "LightSeaGreen") return LightSeaGreen;
			else if (colour == "LightSkyBlue") return LightSkyBlue;
			else if (colour == "LightSlateGray") return LightSlateGray;
			else if (colour == "LightSteelBlue") return LightSteelBlue;
			else if (colour == "LightYellow") return LightYellow;
			else if (colour == "Lime") return Lime;
			else if (colour == "LimeGreen") return LimeGreen;
			else if (colour == "Linen") return Linen;
			break;
		case 'M':
			if (colour == "Magenta") return Magenta;
			else if (colour == "Maroon") return Maroon;
			else if (colour == "MediumAquamarine") return MediumAquamarine;
			else if (colour == "MediumBlue") return MediumBlue;
			else if (colour == "MediumOrchid") return MediumOrchid;
			else if (colour == "MediumPurple") return MediumPurple;
			else if (colour == "MediumSeaGreen") return MediumSeaGreen;
			else if (colour == "MediumSlateBlue") return MediumSlateBlue;
			else if (colour == "MediumSpringGreen") return MediumSpringGreen;
			else if (colour == "MediumTurquoise") return MediumTurquoise;
			else if (colour == "MediumVioletRed") return MediumVioletRed;
			else if (colour == "MidnightBlue") return MidnightBlue;
			else if (colour == "MintCream") return MintCream;
			else if (colour == "MistyRose") return MistyRose;
			else if (colour == "Moccasin") return Moccasin;
			break;
		case 'N':
			if (colour == "NavajoWhite") return NavajoWhite;
			else if (colour == "Navy") return Navy;
			break;
		case 'O':
			if (colour == "OldLace") return OldLace;
			else if (colour == "Olive") return Olive;
			else if (colour == "OliveDrab") return OliveDrab;
			else if (colour == "Orange") return Orange;
			else if (colour == "OrangeRed") return OrangeRed;
			else if (colour == "Orchid") return Orchid;
			break;
		case 'P':
			if (colour == "PaleGoldenrod") return PaleGoldenrod;
			else if (colour == "PaleGreen") return PaleGreen;
			else if (colour == "PaleTurquoise") return PaleTurquoise;
			else if (colour == "PaleVioletRed") return PaleVioletRed;
			else if (colour == "PapayaWhip") return PapayaWhip;
			else if (colour == "PeachPuff") return PeachPuff;
			else if (colour == "Peru") return Peru;
			else if (colour == "Pink") return Pink;
			else if (colour == "Plum") return Plum;
			else if (colour == "PowderBlue") return PowderBlue;
			else if (colour == "Purple") return Purple;
			break;
		case 'R':
			if (colour == "Red") return Red;
			else if (colour == "Raspberry") return Raspberry;
			else if (colour == "RosyBrown") return RosyBrown;
			else if (colour == "RoyalBlue") return RoyalBlue;
			break;
		case 'S':
			if (colour == "SaddleBrown") return SaddleBrown;
			else if (colour == "Salmon") return Salmon;
			else if (colour == "SandyBrown") return SandyBrown;
			else if (colour == "SeaGreen") return SeaGreen;
			else if (colour == "SeaShell") return SeaShell;
			else if (colour == "Sienna") return Sienna;
			else if (colour == "Silver") return Silver;
			else if (colour == "SkyBlue") return SkyBlue;
			else if (colour == "SlateBlue") return SlateBlue;
			else if (colour == "SlateGray") return SlateGray;
			else if (colour == "Snow") return Snow;
			else if (colour == "SpringGreen") return SpringGreen;
			else if (colour == "SteelBlue") return SteelBlue;
			break;
		case 'T': 
			if (colour == "Tan") return Tan;
			else if (colour == "Teal") return Teal;
			else if (colour == "Thistle") return Thistle;
			else if (colour == "Tomato") return Tomato;
			else if (colour == "Transparent") return Transparent;
			else if (colour == "TransparentBlack") return TransparentBlack;
			else if (colour == "Turquoise") return Turquoise;
			break;
		case 'V':
			if (colour == "Violet") return Violet;
			break;
		case 'W':
			if (colour == "Wheat") return Wheat;
			else if (colour == "White") return White;
			else if (colour == "WhiteSmoke") return WhiteSmoke;
			break;
		case 'Y':
			if (colour == "Yellow") return Yellow;
			else if (colour == "YellowGreen") return YellowGreen;
			break;
		}
		return Black;
	}
};
