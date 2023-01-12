#pragma once


namespace UIMiniEmbed {

	class UIRenderDriverApi {
		public:
			virtual void drawSprite(const std::string& file, const BBox& srcBBox, const BBox& dstBBox, const Utils::Color color) {}
			virtual void drawText(const std::string& text, const Vec2& pos, const float scale, const Utils::Color color) {}
	};
	using SP_UIRenderDriverApi = std::shared_ptr< UIRenderDriverApi >;

}
