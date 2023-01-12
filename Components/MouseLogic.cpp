#pragma once

namespace UIMiniEmbed {
	
	class UIComponent_BaseMouseEventResult : public UIComponentContainerTransparent {
		private:
			UIVar _preventDefault;
			UIVar _stopPropagation;
		
		protected:
			virtual void _init_VarLink() override {
				_preventDefault  = getVar("preventDefault");
				_stopPropagation = getVar("stopPropagation");
			}
			
			uint32_t getResultEvent() {
				return 
					(_preventDefault .getBool() ? UIInputMousePreventDefault  : 0) |
					(_stopPropagation.getBool() ? UIInputMouseStopPropagation : 0) ;
			}
	};

	class UIComponent_Hover : public UIComponent_BaseMouseEventResult {
		private:
			UIVar _out;
			
		protected:
			virtual void _init_VarLink() override {
				UIComponent_BaseMouseEventResult::_init_VarLink();
				_out = getVar("out");
			}
			
			virtual uint32_t input_OnMouseEnter(const UIInputMouseState& mouseState) override {
				_out.setBool(true);
				return getResultEvent();
			}
			virtual void     input_OnMouseLeave(const UIInputMouseState& mouseState) override {
				_out.setBool(false);
			}
	};

	template< const UIEnumMouseButton MouseBtn >
	class UIComponent_Active : public UIComponent_BaseMouseEventResult {
		private:
			UIVar _out;
		
		protected:
			virtual void _init_VarLink() override {
				UIComponent_BaseMouseEventResult::_init_VarLink();
				_out = getVar("out");
			}
			
			virtual uint32_t input_OnMouseDown(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) override {
				if ( eBtn != MouseBtn )
					return 0;
				
				_out.setBool(true);
				return getResultEvent();
			}

			virtual uint32_t input_OnMouseUp(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) override {
				if ( eBtn != MouseBtn )
					return 0;
				
				_out.setBool(false);
				return getResultEvent();
			}
			
			virtual void input_OnGlobalMouseUp  (const UIInputMouseState&  mouseState, const UIEnumMouseButton eBtn) override {
				if ( eBtn != MouseBtn )
					return;
				
				_out.setBool(false);
			}
	};
	using UIComponent_ActiveL = UIComponent_Active< UIEnumMouseButton::Left >;


	template< const UIEnumMouseButton MouseBtn >
	class UIComponent_Click : public UIComponent_BaseMouseEventResult {
		private:
			UIVar _out;
		
		protected:
			virtual void _init_VarLink() override {
				UIComponent_BaseMouseEventResult::_init_VarLink();
				_out = getVar("out");
			}
			
			virtual uint32_t input_OnMouseClick(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) override {
				if ( eBtn != MouseBtn )
					return 0;
				
				_out.setBool(true);
				return getResultEvent();
			}
	};
	using UIComponent_ClickL = UIComponent_Click< UIEnumMouseButton::Left >;

}
