#pragma once

namespace UIMiniEmbed {
	
	class UIComponent_TextLine : public UIComponentContainer {
		private:	
			UIVar _text;
			UIVar _scale;
			UIVar _charWidth;
			UIVar _charHeight;
			
			static constexpr float CharWidth  = 7;
			static constexpr float CharHeight = 11;
			
			static float _getFloatOrDef(const UIVar& var, const float def) {
				if ( var.isNull() )
					return def;
				return max(0, var.getFloat());
			}
			
		protected:
			virtual void _init_VarLink() override {
				UIComponentContainer::_init_VarLink();
				
				_text       = getVar("text");
				_scale      = getVar("scale");
				_charWidth  = getVar("charWidth");
				_charHeight = getVar("charHeight");
			}

			virtual UIPosValVariant     getStyleWidth () override {
				return UIPosValVariant{ UIPosValVariant::Pixel, 
					_getFloatOrDef(_charWidth , CharWidth ) * _getFloatOrDef(_scale, 1) * _text.getStringRef().length() };
			}
			virtual UIPosValVariant     getStyleHeight() override {
				return UIPosValVariant{ UIPosValVariant::Pixel, 
					_getFloatOrDef(_charHeight, CharHeight) * _getFloatOrDef(_scale, 1) };
			}

			virtual bool draw(SP_UIRenderDriverApi& spApi, const UIRenderContext& rCtx) override {
				const auto bbox = getInnerBBox();
				
				spApi->drawText( _text.getString(), bbox.min, _getFloatOrDef(_scale, 1), rCtx.color );
				return true;
			}
	};
	
	class UIComponent_TextLineDumpVar : public UIComponent_TextLine {
		private:	
			UIVar _text;
			UIVar _in;
			
		protected:
			virtual void _init_VarLink() override {
				UIComponent_TextLine::_init_VarLink();
				
				_in   = getVar("in");
				_text = getVar("text");
			}
			virtual void _update_State() override {
				_text.set( _in.dump() );
			}
	};

}