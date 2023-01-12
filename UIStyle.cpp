#pragma once

#include "Utils.cpp"
#include "UIVar.cpp"
#include "UINodeDesc.cpp"
#include "Parser.cpp"
#include "UIRenderDriverApi.cpp"
#include "UIInputMouse.cpp"

namespace UIMiniEmbed {
	
	class  UIPosValVariant {
		public:
			enum EnumType : int32_t { 
				Pixel, 
				Percent, 
				Fraction, 
				None,
			};

		private:
			EnumType _type  = Pixel;
			float    _value = 0;

			void _set(EnumType eType, const float val) {
				_type  = eType;
				_value = normalizeFloat(val);
			}
		
		public:
			UIPosValVariant() {}
			UIPosValVariant(const EnumType type, const float val) {
				_type  = type;
				_value = val;
			}
			
			float calc(const float outer) const {
				switch( _type ) {
					case Pixel  : return _value;
					case Percent: return _value * outer;
				}
				return 0;
			}
			
			bool  isNone     () const { return _type == None; }
			bool  isFraction () const { return _type == Fraction; }
			float getFraction() const {
				if ( _value < 1 )
					return 1;
				return _value;
			}

			void setPixel   (const float value) { _set( Pixel  , value ); }
			void setPercent (const float value) { _set( Percent, value ); }
			void setFraction(const float value) { _set( Pixel  , max(1, normalizeFloat(value)) ); }
	};
	struct UIPosValVariantRect {
		UIPosValVariant left;
		UIPosValVariant right;
		UIPosValVariant top;
		UIPosValVariant bottom;
	};

	enum class UIEnumDirection {
		Row, 
		Column,
	};
	enum class UIEnumAlign {
		Start, 
		Center,
		End,
	};

}
