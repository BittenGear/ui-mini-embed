#pragma once

namespace UIMiniEmbed {

	constexpr uint32_t UIInputMousePreventDefault  = 1;
	constexpr uint32_t UIInputMouseStopPropagation = 2;
	
	enum class UIEnumMouseButton : int32_t {
		Left   = 1,
		Middle = 2,
		Right  = 3,
	};

	enum class UIInputMouseKeyState : uint8_t {
		Null = 0,
		Free,
		Pressed,
	};
	
	class UIInputMouseKeyStateList : public std::array< UIInputMouseKeyState, 4 > {
		public:
			UIInputMouseKeyState& get(const UIEnumMouseButton eBtn) {
				switch( eBtn ) {
					case UIEnumMouseButton::Left  : return (*this)[1];
					case UIEnumMouseButton::Middle: return (*this)[2];
					case UIEnumMouseButton::Right : return (*this)[3];
				}
				return (*this)[0];
			}
			
			template< class TFun, class TFun2 >
			void eachDelta(UIInputMouseKeyStateList& other, TFun down, TFun2 up) {
				constexpr std::array< UIEnumMouseButton, 3 > bl = { UIEnumMouseButton::Left, UIEnumMouseButton::Middle, UIEnumMouseButton::Right };
				for(const auto eBtn : bl) {
					const auto a = get(eBtn);
					const auto b = other.get(eBtn);
					if ( a != b && a != UIInputMouseKeyState::Null && b != UIInputMouseKeyState::Null ) {
						if ( a == UIInputMouseKeyState::Pressed ) down(eBtn);
						if ( a == UIInputMouseKeyState::Free    ) up  (eBtn);
					}
				}
			}
	};
	static_assert( sizeof(UIInputMouseKeyStateList) == 4 );

	struct UIInputMousePressState {
		bool left   = false;
		bool middle = false;
		bool right  = false;
		
		UIInputMouseKeyStateList getKeyStateList() const {
			return { 
				UIInputMouseKeyState::Null,
				left   ? UIInputMouseKeyState::Pressed : UIInputMouseKeyState::Free,
				middle ? UIInputMouseKeyState::Pressed : UIInputMouseKeyState::Free,
				right  ? UIInputMouseKeyState::Pressed : UIInputMouseKeyState::Free,
			};
		}
	};

	struct UIInputMouseState {
		Vec2                   screen;	/// screen pos
		Vec2                   client;	/// window pos
		UIInputMousePressState pressState;
	};
	
}
