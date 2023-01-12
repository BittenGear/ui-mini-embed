#pragma once

namespace UIMiniEmbed {

	class UIComponent_Sprite : public UIComponentContainer {
		private:	
			UIVar _path;
			UIVar _sx;
			UIVar _sy;
			UIVar _sw;
			UIVar _sh;
			
		protected:
			virtual void _init_VarLink() override {
				UIComponentContainer::_init_VarLink();

				_path = getVar("path");
				_sx   = getVar("sx");
				_sy   = getVar("sy");
				_sw   = getVar("sw");
				_sh   = getVar("sh");
			}
			
			virtual bool draw(SP_UIRenderDriverApi& spApi, const UIRenderContext& rCtx) override {
				const auto& innerBBox = getInnerBBoxRef();

				auto srcBBox = BBox{
					{ _sx.getFloat(), _sw.getFloat() },
					{ _sy.getFloat(), _sh.getFloat() },
				};

				spApi->drawSprite(_path.getString(), srcBBox, innerBBox, rCtx.color);
				return true;
			}
	};
	
	
	class UIComponent_SpriteFrameAnimation : public UIComponentContainer {
		private:	
			UIVar _path;

			UIVar _tw;
			UIVar _th;
			UIVar _sw;
			UIVar _sh;
			
			UIVar _startFrame;
			UIVar _endFrame;
			UIVar _fraction;
			
		protected:
			virtual void _init_VarLink() override {
				UIComponentContainer::_init_VarLink();

				_path = getVar("path");
				
				_tw   = getVar("tw");
				_th   = getVar("th");
				
				_sw   = getVar("sw");
				_sh   = getVar("sh");

				_startFrame = getVar("startFrame");
				_endFrame   = getVar("endFrame");
				_fraction   = getVar("fraction");
			}
			
			virtual bool draw(SP_UIRenderDriverApi& spApi, const UIRenderContext& rCtx) override {
				const float fraction = clamp(0, _fraction.getFloat(), 1);
				
				const int32_t startFrame = _startFrame.getI32();
				const int32_t endFrame   = _endFrame.getI32();
				const int32_t frameCount = endFrame - startFrame;
				
				const int32_t frame = ( startFrame + std::round( ((float)frameCount) * fraction ) );
				
				const float sw = max(1 , _sw.getFloat());
				const float sh = max(1 , _sh.getFloat());
				const float tw = max(sw, _tw.getFloat());
				const float th = max(sh, _th.getFloat());

				const int32_t cell_w = std::floor( tw / sw );
				const int32_t cell_h = std::floor( th / sh );

				const int32_t cell_x = frame % cell_w;
				const int32_t cell_y = std::floor( frame / cell_w );
				
				const auto srcPos  = Vec2{ ((float)cell_x) * sw, ((float)cell_y) * sh };
				const auto srcBBox = BBox{ srcPos, srcPos + Vec2{ sw, sh } };
				
				const auto& innerBBox = getInnerBBoxRef();

				spApi->drawSprite(_path.getString(), srcBBox, innerBBox, rCtx.color);
				return true;
			}
	};
	
}
