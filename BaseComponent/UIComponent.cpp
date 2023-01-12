#pragma once

namespace UIMiniEmbed {

	template< class T >
	struct StdVectorWithoutCopy : public std::vector< T > {
		private:
			std::mutex ___mutex;
	};

	struct UIRenderContext {
		float        opacity = 1;
		Utils::Color color;
	};

	class UIComponent;
	using SP_UIComponent  = std::shared_ptr< UIComponent >;
	using WP_UIComponent  = std::weak_ptr  < UIComponent >;
	using UIComponentList = StdVectorWithoutCopy< SP_UIComponent >;

	SP_UIComponent createUINode(SP_UINodeDesc spNodeDesc, SP_UIVarEnv spVarEnv = nullptr, SP_UIComponent spParentNode = nullptr);
	
	class UIComponent : public std::enable_shared_from_this< UIComponent > {
		public:
			virtual ~UIComponent() {}

		private:
			SP_UINodeDesc   _spNodeDesc = nullptr;
			SP_UIVarEnv     _spVarEnv   = nullptr;
			WP_UIComponent  _wpParentNode;

			UIComponentList _childNodeList;
			UIComponentList _childRenderNodeList;

		protected:
			SP_UIComponent   getParentNodeOrNull      () { return _wpParentNode.lock(); }
			UIComponentList& getChildNodeListRef      () { return _childNodeList;       }
			UIComponentList& getChildRenderNodeListRef() { return _childRenderNodeList; }
			
			SP_UIVarEnv      createChildVarEnv        () { return UIVarEnv::create(_spVarEnv); }
			
			SP_UIComponent   createChildNode(SP_UINodeDesc spNodeDesc, SP_UIVarEnv spVarEnv = nullptr) {
				if ( !spVarEnv )
					spVarEnv = createChildVarEnv();

				return createUINode( spNodeDesc, spVarEnv, shared_from_this() );
			}
			
		public:
			UIVar getVar(const std::string& name, const bool searchParent = false) {
				return _spVarEnv->getVar(name, searchParent);
			}
			void  setVar(const std::string& name, UIVar var) {
				_spVarEnv->setVar(name, var);
			}

		private:
			void _init_Var(SP_UINodeDesc& spNodeDesc) {
				for(const auto prop : spNodeDesc->getProps()) {
					switch( prop.getType() ) {
						case UINodePropDesc::ConstBool          : getVar( prop.getName() ).setBool( prop.getValue() == "true" ? true : false ); break;
						case UINodePropDesc::ConstString        : getVar( prop.getName() ).setString( prop.getValue() ); break;
						
						case UINodePropDesc::ConstNumber        : getVar( prop.getName() ).setFloat   ( stringToFloat( prop.getValue() )          ); break;
						case UINodePropDesc::ConstNumberPixel   : getVar( prop.getName() ).setPixel   ( stringToFloat( prop.getValue() )          ); break;
						case UINodePropDesc::ConstNumberPercent : getVar( prop.getName() ).setPercent ( stringToFloat( prop.getValue() ) / 100.0f ); break;
						case UINodePropDesc::ConstNumberFraction: getVar( prop.getName() ).setFraction( stringToFloat( prop.getValue() )          ); break;	
						
						case UINodePropDesc::VarExternal: {
							auto var = getVar( prop.getValue(), true );
							for(auto path : prop.getFetchPath())
								var = var.map_Get(path);
							setVar( prop.getName(), var );
						};
						break;
					}
				}
			}
		
		public:
			void create(SP_UINodeDesc spNodeDesc, SP_UIVarEnv spVarEnv, SP_UIComponent spParentNode = nullptr) {
				_spNodeDesc   = spNodeDesc;
				_spVarEnv     = spVarEnv;
				_wpParentNode = spParentNode;
				
				_init_Var(_spNodeDesc);
				_init_VarLink();
			}
			
		
		

		/////////////////////////////////////////////// Style
		/////////////////////////////////////////////// Style
		/////////////////////////////////////////////// Style
		private:
			UIVar _left  ;
			UIVar _right ;
			UIVar _top   ;
			UIVar _bottom;
					
			UIVar _padding;
			UIVar _paddingLeft;
			UIVar _paddingRight;
			UIVar _paddingTop;
			UIVar _paddingBottom;
				
			UIVar _widthHeight;
			UIVar _width;
			UIVar _height;
					
			UIVar _gap;
					
			UIVar _relative;
			UIVar _absolute;
					
			UIVar _dirColumn;
			
			UIVar _alignX;
			UIVar _alignY;
			UIEnumAlign _cacheAlignX = UIEnumAlign::Start;
			UIEnumAlign _cacheAlignY = UIEnumAlign::Start;
			
			UIVar _opacity;
			UIVar _color;
			
		protected:
			static UIPosValVariant _getPosVariant(const UIVar& var) {
				switch( var.getType() ) {
					case UIVar::Null         : return UIPosValVariant{ UIPosValVariant::None, 0 };
					case UIVar::StylePixel   : return UIPosValVariant{ UIPosValVariant::Pixel   , var.getFraction() };
					case UIVar::StylePercent : return UIPosValVariant{ UIPosValVariant::Percent , var.getFraction() };
					case UIVar::StyleFraction: return UIPosValVariant{ UIPosValVariant::Fraction, var.getFraction() };

					default:
						return UIPosValVariant{ UIPosValVariant::Pixel, var.getFloat() };
				}
				
				return UIPosValVariant{ UIPosValVariant::Pixel, var.getFloat() };
			}
				
			static UIEnumAlign _getAlign(const UIVar& var) {
				if ( var.getString() == "center" ) return UIEnumAlign::Center;
				if ( var.getString() == "end"    ) return UIEnumAlign::End;				
				return UIEnumAlign::Start;
			}

		public:
		
			virtual void _init_VarLink() {
				_left   = getVar("left");
				_right  = getVar("right");
				_top    = getVar("top");
				_bottom = getVar("bottom");
						
				_padding       = getVar("pad");
				_paddingLeft   = getVar("padl");
				_paddingRight  = getVar("padr");
				_paddingTop    = getVar("padt");
				_paddingBottom = getVar("padb");
						
				_widthHeight = getVar("wh");
				_width       = getVar("w");
				_height      = getVar("h");
						
				_relative  = getVar("rel");
				_absolute  = getVar("abs");
						
				_dirColumn = getVar("column");
						
				_alignX = getVar("alignx");
				_alignY = getVar("aligny");
						
				_gap = getVar("gap");
				
				_opacity = getVar("opacity");
				_color   = getVar("color");
			}
		
		
			virtual UIPosValVariant     getStyleWidth        () { return _getPosVariant( _width .isNull() ? _widthHeight : _width  ); }
			virtual UIPosValVariant     getStyleHeight       () { return _getPosVariant( _height.isNull() ? _widthHeight : _height ); }
					
			virtual UIPosValVariantRect getStylePadding () {
				return {
					_getPosVariant( _paddingLeft  .isNull() ? _padding : _paddingLeft   ),
					_getPosVariant( _paddingRight .isNull() ? _padding : _paddingRight  ),
					_getPosVariant( _paddingTop   .isNull() ? _padding : _paddingTop    ),
					_getPosVariant( _paddingBottom.isNull() ? _padding : _paddingBottom ),
				};
			}
			virtual UIPosValVariantRect getStylePosition() {
				return {
					_getPosVariant( _left   ),
					_getPosVariant( _right  ),
					_getPosVariant( _top    ),
					_getPosVariant( _bottom ),
				};
			}
					
			virtual UIPosValVariant getStyleGap          () { return _getPosVariant( _gap ); }
					
			virtual bool            getStyleRelative     () { return _relative.getBool(); }
			virtual bool            getStyleAbsolute     () { return _absolute.getBool(); }
			virtual UIEnumDirection getStyleDirection    () { return _dirColumn.getBool() ? UIEnumDirection::Column : UIEnumDirection::Row; }
					
			virtual UIEnumAlign     getStyleAlignX() {
				if ( _alignX.readInvalidate() )
					_cacheAlignX = _getAlign(_alignX);
				return _cacheAlignX;
			}
			virtual UIEnumAlign     getStyleAlignY() {
				if ( _alignY.readInvalidate() )
					_cacheAlignY = _getAlign(_alignY);
				return _cacheAlignY;
			}
	
			virtual float           getStyleOpacity() {
				if ( _opacity.isNull() )
					return 1;
				return clamp(0, _opacity.getFloat(), 1);
			}
			
			Utils::Color _cacheColor = Utils::Color{ 0xFF, 0xFF, 0xFF, 0xFF };
			virtual Utils::Color    getStyleColor() {
				if ( _color.readInvalidate() )
					if ( !_color.isNull() )
						_cacheColor = Utils::Color::fromStringRGBA( _color.getString() );
				
				return _cacheColor;
			}
	
		/////////////////////////////////////////////// FinalPosition
		/////////////////////////////////////////////// FinalPosition
		/////////////////////////////////////////////// FinalPosition
		private:
			BBox _outerBBox;
			BBox _innerBBox;

		protected:
			void setOuterBBox(const BBox& bbox) { 
				_outerBBox = bbox;
				
				const auto padding = getStylePadding();
				_innerBBox = {
					_outerBBox.min + Vec2{ padding.left .calc( _outerBBox.getWidth() ), padding.top   .calc( _outerBBox.getHeight() ), },
					_outerBBox.max - Vec2{ padding.right.calc( _outerBBox.getWidth() ), padding.bottom.calc( _outerBBox.getHeight() ), },
				};
			}
			const BBox& getOuterBBoxRef() const { return _outerBBox; }
			const BBox& getInnerBBoxRef() const { return _innerBBox; }
		
		public:
			BBox getOuterBBox() const { return _outerBBox; }
			BBox getInnerBBox() const { return _innerBBox; }
			
			void setRootBBox(const BBox& bbox) {
				setOuterBBox(bbox);
			}

			std::string dumpInnerBBox(const int32_t dp = 0) {
				std::string dump = "";
				for(int32_t i = 0; i < dp; i++)
					dump += "    ";

				dump += getInnerBBoxRef().dump() + "\n";
				for( auto& childNode : getChildRenderNodeListRef() )
					dump += childNode->dumpInnerBBox(dp + 1);
				return dump;
			}
		
		/////////////////////////////////////////////// CalcPosition
		/////////////////////////////////////////////// CalcPosition
		/////////////////////////////////////////////// CalcPosition
		
		private:
			void _calcSizeAbsolute(const BBox& containerBBox, const UIEnumAlign containerAlignX, const UIEnumAlign containerAlignY) {
				const auto size = Vec2{
					getStyleWidth ().calc( containerBBox.getWidth () ),
					getStyleHeight().calc( containerBBox.getHeight() ),
				};
				
				const auto position = getStylePosition();
				Vec2 pos = containerBBox.min;
				
				bool has_x = false;
				bool has_y = false;
				if ( !position.left  .isNone() ) { pos.x = containerBBox.min.x +          position.left  .calc( containerBBox.getWidth () ); has_x = true; }
				if ( !position.right .isNone() ) { pos.x = containerBBox.max.x - size.x - position.right .calc( containerBBox.getWidth () ); has_x = true; }
				if ( !position.top   .isNone() ) { pos.y = containerBBox.min.y +          position.top   .calc( containerBBox.getHeight() ); has_y = true; }
				if ( !position.bottom.isNone() ) { pos.y = containerBBox.max.y - size.y - position.bottom.calc( containerBBox.getHeight() ); has_y = true; }
				
				if ( !has_x ) pos.x += _calcAlignCorrect( containerAlignX, containerBBox.getWidth (), size.x );
				if ( !has_y ) pos.y += _calcAlignCorrect( containerAlignY, containerBBox.getHeight(), size.y );
				
				setOuterBBox( BBox{ pos, pos + size } );
			}

			static BBox _swapSideBBox(const BBox& bbox) {
				return BBox{
					Vec2{ bbox.min.y, bbox.min.x },
					Vec2{ bbox.max.y, bbox.max.x },
				};
			}
			
			struct TFlowData {
				BBox        bbox;
				float       gap;
				UIEnumAlign firstAlign;
				UIEnumAlign secondAlign;
			};
			TFlowData       _getFlowData(const UIEnumDirection eDir) {
				const auto  gap = getStyleGap();
				const auto& innerBBox  = getInnerBBoxRef();

				if ( eDir == UIEnumDirection::Row )
					return { innerBBox, gap.calc( innerBBox.getWidth() ), getStyleAlignX(), getStyleAlignY() };

				return { _swapSideBBox(innerBBox), gap.calc( innerBBox.getHeight() ), getStyleAlignY(), getStyleAlignX() };
			}
			UIPosValVariant _getFlowSize(const UIEnumDirection eDir) {
				if ( eDir == UIEnumDirection::Row )
					return getStyleWidth();

				return getStyleHeight();
			}

			float _tmpFirstSize = 0;
			float _tmpSecondSize = 0;

			static float _calcAlignCorrect(const UIEnumAlign eAlign, const float containerSize, const float contentSize) {
				switch( eAlign ) {
					case UIEnumAlign::Start : return 0;
					case UIEnumAlign::End   : return containerSize - contentSize;
					case UIEnumAlign::Center: return ( containerSize - contentSize ) / 2;
				}
				
				return 0;
			}

			void update_PositionWalk(const BBox& parentRelBBox) {
				const auto  eDir      = getStyleDirection();
				const auto  eDirInv   = eDir == UIEnumDirection::Row ? UIEnumDirection::Column : UIEnumDirection::Row;
				const auto& innerBBox = getInnerBBoxRef();
				const auto  flowData  = _getFlowData( eDir );
				
				float  firstSummarySize = 0;
				float  firstSummaryFr   = 0;
				size_t flowNodeIndex    = 0;
				
				auto& childRenderNodeListRef = getChildRenderNodeListRef();
				
				for(auto& childNode : childRenderNodeListRef) {
					if ( childNode->getStyleAbsolute() )
						continue;
					
					if ( flowNodeIndex != 0 )
						firstSummarySize += flowData.gap;
					flowNodeIndex++;
					
					///////////////////
					childNode->_tmpSecondSize = childNode->_getFlowSize(eDirInv).calc( flowData.bbox.getHeight() );
					
					const auto& firstSizeVrnt = childNode->_getFlowSize(eDir);
					if ( firstSizeVrnt.isFraction() ) {
						firstSummaryFr += firstSizeVrnt.getFraction();
						continue;
					}

					childNode->_tmpFirstSize = firstSizeVrnt.calc( flowData.bbox.getWidth() );
					
					firstSummarySize += childNode->_tmpFirstSize;
				}
				
				firstSummaryFr = max(1, firstSummaryFr);
				float firstFreeSize = max(0, flowData.bbox.getWidth() - firstSummarySize);
				for(auto& childNode : childRenderNodeListRef) {
					if ( childNode->getStyleAbsolute() )
						continue;
					
					const auto& flowSizeVrnt = childNode->_getFlowSize(eDir);
					if ( !flowSizeVrnt.isFraction() )
						continue;
					
					childNode->_tmpFirstSize = flowSizeVrnt.getFraction() / firstSummaryFr * firstFreeSize;
				}
				
				firstSummarySize = 0;
				float secondMaxSize = 0;
				for(auto& childNode : childRenderNodeListRef) {
					if ( childNode->getStyleAbsolute() )
						continue;
					
					firstSummarySize += childNode->_tmpFirstSize;
					secondMaxSize = max(secondMaxSize, childNode->_tmpSecondSize);
				}
				
				float firstPos = _calcAlignCorrect(flowData.firstAlign, flowData.bbox.getWidth(), firstSummarySize);
				flowNodeIndex = 0;
				for(auto& childNode : childRenderNodeListRef) {
					if ( childNode->getStyleAbsolute() )
						continue;
					
					float x = firstPos;
					float secondPos = _calcAlignCorrect(flowData.secondAlign, flowData.bbox.getHeight(), childNode->_tmpSecondSize);
					
					auto outerBBox = BBox{
						Vec2{ firstPos , secondPos  },
						Vec2{ firstPos + childNode->_tmpFirstSize, secondPos + childNode->_tmpSecondSize },
					};
					
					if ( eDir == UIEnumDirection::Column )
						outerBBox = _swapSideBBox(outerBBox);
					
					childNode->setOuterBBox( BBox{ 
						innerBBox.min + outerBBox.min,
						innerBBox.min + outerBBox.max,
					} );
					
					firstPos += childNode->_tmpFirstSize + flowData.gap;
				}
				
				/////////////////////
				const auto& nextRelBBox = getStyleRelative() ? innerBBox : parentRelBBox;

				const auto alignX = getStyleAlignX();
				const auto alignY = getStyleAlignY();
				for(auto& childNode : childRenderNodeListRef) {
					if ( !childNode->getStyleAbsolute() )
						continue;
					
					childNode->_calcSizeAbsolute(nextRelBBox, alignX, alignY);
				}
				
				for(auto& childNode : childRenderNodeListRef)
					childNode->update_PositionWalk( nextRelBBox ); 
			}




		/////////////////////////////////////////////// InputMouse
		/////////////////////////////////////////////// InputMouse
		/////////////////////////////////////////////// InputMouse
		private:

			SP_UIComponent input_MouseSearchHoverFinalWalk(const UIInputMouseState& mouseState) {
				SP_UIComponent spNode = nullptr;
				
				if ( getOuterBBoxRef().isPointOnBBox(mouseState.client) )
					spNode = shared_from_this();

				for( auto& renderChildNode : getChildRenderNodeListRef() ) {
					auto spNode2 = renderChildNode->input_MouseSearchHoverFinalWalk(mouseState);
					if ( spNode2 )
						spNode = spNode2;
				}
				
				return spNode;
			}
		
			
			bool     _inputMouseEnterCurrFlag  = false;
			bool     _inputMouseEnterPrevFlag  = false;
			uint32_t _inputMouseEnterPrevState = 0;
			
			size_t _inputMousePrevFinalNode = 0;
			size_t _inputMouseCurrFinalNode = 0;
			UIInputMouseKeyStateList _inputMousePrevKeyState = {UIInputMouseKeyState::Null,UIInputMouseKeyState::Null,UIInputMouseKeyState::Null,UIInputMouseKeyState::Null};
			UIInputMouseKeyStateList _inputMouseCurrKeyState = {UIInputMouseKeyState::Null,UIInputMouseKeyState::Null,UIInputMouseKeyState::Null,UIInputMouseKeyState::Null};
			
			UIInputMouseKeyStateList _inputMousePressKeyState = {UIInputMouseKeyState::Null,UIInputMouseKeyState::Null,UIInputMouseKeyState::Null,UIInputMouseKeyState::Null};
			
			void _input_MouseClearWalk() {
				_inputMouseEnterCurrFlag = false;
				
				for( auto& childNode : getChildNodeListRef() )
					childNode->_input_MouseClearWalk();
			}
			
			void _input_MouseEnterProcess(const UIInputMouseState& mouseState) {
				_inputMouseEnterCurrFlag = true;

				if ( !_inputMouseEnterPrevFlag ) {
					_inputMouseEnterPrevFlag  = true;
					_inputMouseEnterPrevState = input_OnMouseEnter(mouseState);
				}
					
				if ( _inputMouseEnterPrevState & UIInputMouseStopPropagation )
					return;
					
				auto spParentNode = getParentNodeOrNull();
				if ( spParentNode )
					spParentNode->_input_MouseEnterProcess(mouseState);
			}
			
			void _input_MouseLeaveProcessWalk(const UIInputMouseState& mouseState) {
				if ( !_inputMouseEnterCurrFlag && _inputMouseEnterPrevFlag ) {
					_inputMouseEnterCurrFlag  = false;
					_inputMouseEnterPrevFlag  = false;
					_inputMouseEnterPrevState = 0;

					input_OnMouseLeave(mouseState);					
				}
				
				for( auto& childNode : getChildNodeListRef() )
					childNode->_input_MouseLeaveProcessWalk(mouseState);
			}

			void _input_MouseDownProcess(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) {
				const auto r = input_OnMouseDown(mouseState, eBtn);
				if ( r & UIInputMouseStopPropagation )
					return;
				
				_inputMousePressKeyState.get(eBtn) = UIInputMouseKeyState::Pressed;
				
				auto spParentNode = getParentNodeOrNull();
				if ( spParentNode )
					spParentNode->_input_MouseDownProcess(mouseState, eBtn);
			}
			void _input_MouseUpProcess(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) {
				const auto r = input_OnMouseUp(mouseState, eBtn);
				if ( r & UIInputMouseStopPropagation )
					return;
				
				auto spParentNode = getParentNodeOrNull();
				if ( spParentNode )
					spParentNode->_input_MouseUpProcess(mouseState, eBtn);
			}
			
			void _input_MouseClickProcess(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) {
				const auto r = input_OnMouseClick(mouseState, eBtn);
				if ( r & UIInputMouseStopPropagation )
					return;
				
				auto spParentNode = getParentNodeOrNull();
				if ( spParentNode )
					spParentNode->_input_MouseClickProcess(mouseState, eBtn);
			}

			void _input_GlobalMouseDownProcess(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) {
				input_OnGlobalMouseDown(mouseState, eBtn);
				for( auto& childNode : getChildNodeListRef() )
					childNode->_input_GlobalMouseDownProcess(mouseState, eBtn);
			}
			void _input_GlobalMouseUpProcess(const UIInputMouseState& mouseState, const UIEnumMouseButton eBtn) {
				input_OnGlobalMouseUp(mouseState, eBtn);

				_inputMousePressKeyState.get(eBtn) = UIInputMouseKeyState::Free;

				for( auto& childNode : getChildNodeListRef() )
					childNode->_input_GlobalMouseUpProcess(mouseState, eBtn);
			}
		public:
			void input_MouseUpdate(const UIInputMouseState& mouseState) {
				const auto mouseKeyState = mouseState.pressState.getKeyStateList();

				auto spNode = input_MouseSearchHoverFinalWalk(mouseState);
				
				_inputMousePrevKeyState  = _inputMouseCurrKeyState;
				_inputMouseCurrKeyState  = mouseKeyState;
				_inputMousePrevFinalNode = _inputMouseCurrFinalNode;
				_inputMouseCurrFinalNode = (size_t)( spNode ? spNode.get() : nullptr );

				if ( spNode ) {
					_inputMouseCurrKeyState.eachDelta(
						_inputMousePrevKeyState, 
						[&](const auto eBtn) { spNode->_input_MouseDownProcess(mouseState, eBtn); },
						[&](const auto eBtn) {
							spNode->_input_MouseUpProcess(mouseState, eBtn);
							if ( spNode->_inputMousePressKeyState.get(eBtn) == UIInputMouseKeyState::Pressed )
								spNode->_input_MouseClickProcess(mouseState, eBtn);
						}
					);
				}

				_inputMouseCurrKeyState.eachDelta(
					_inputMousePrevKeyState, 
					[&](const auto eBtn) { _input_GlobalMouseDownProcess(mouseState, eBtn); },
					[&](const auto eBtn) { _input_GlobalMouseUpProcess  (mouseState, eBtn); }
				);				
				
				_input_MouseClearWalk();
				
				if ( spNode )
					spNode->_input_MouseEnterProcess(mouseState);
				
				_input_MouseLeaveProcessWalk(mouseState);
			}
			
			
			
			virtual uint32_t input_OnMouseEnter(const UIInputMouseState& mouseState) { return 0; }
			virtual void     input_OnMouseLeave(const UIInputMouseState& mouseState) { return; }
			
			virtual uint32_t input_OnMouseDown (const UIInputMouseState&  mouseState, const UIEnumMouseButton eBtn) { return 0; }
			virtual uint32_t input_OnMouseUp   (const UIInputMouseState&  mouseState, const UIEnumMouseButton eBtn) { return 0; }
			virtual uint32_t input_OnMouseClick(const UIInputMouseState&  mouseState, const UIEnumMouseButton eBtn) { return 0; }
			
			virtual void input_OnGlobalMouseDown(const UIInputMouseState&  mouseState, const UIEnumMouseButton eBtn) { }
			virtual void input_OnGlobalMouseUp  (const UIInputMouseState&  mouseState, const UIEnumMouseButton eBtn) { }

		/////////////////////////////////////////////// Main
		/////////////////////////////////////////////// Main
		/////////////////////////////////////////////// Main

		public:
			virtual bool draw                      (SP_UIRenderDriverApi& api, const UIRenderContext& rCtx) { return true; }

			virtual void _update_ChildNodeList     (UIComponentList& outList, SP_UINodeDesc& spNodeDesc) {}
			virtual void _update_SelfRenderNodeList(UIComponentList& outList                           ) {}
			virtual void _update_State() {}

		private:
			void update_ChildNodeListWalk() {
				_update_State();
				_update_ChildNodeList( getChildNodeListRef(), _spNodeDesc );

				for( auto& childNode : getChildNodeListRef() )
					childNode->update_ChildNodeListWalk();
			}
			void update_ChildRenderNodeListWalk() {
				auto& childRenderNodeListRef = getChildRenderNodeListRef();
				childRenderNodeListRef.clear();

				for( auto& childNode : getChildNodeListRef() )
					childNode->_update_SelfRenderNodeList(childRenderNodeListRef);
				
				for( auto& renderChildNode : childRenderNodeListRef )
					renderChildNode->update_ChildRenderNodeListWalk();
			}
			void update_StateWalk() {
				_update_State();

				for( auto& childNode : getChildNodeListRef() )
					childNode->update_StateWalk();
			}

		public:
			void update() {
				loop_Update();
				
				update_ChildNodeListWalk();
				update_ChildRenderNodeListWalk();
				update_PositionWalk( getInnerBBoxRef() );
				//update_StateWalk();
			}

			void drawAll(SP_UIRenderDriverApi& api, const UIRenderContext& parentRCtx = {}) {
				auto color = getStyleColor();
				const auto opacity = getStyleOpacity();
				const float finalOpacity = clamp( 0, parentRCtx.opacity * opacity * ( ((float)color.a) / ((float)0xFF) ), 1 );
				
				color.a = (uint8_t)clamp( 0, finalOpacity * ((float)0xFF), 0xFF );
				
				const auto rCtx = UIRenderContext{ 
					finalOpacity, 
					color,
				};
				
				if ( !draw(api, rCtx) )
					return;
				
				for( auto& childNode : getChildRenderNodeListRef() )
					childNode->drawAll(api, rCtx);
			}
	};



	class UIComponentContainer : public UIComponent {
		public:
			virtual void _update_ChildNodeList(UIComponentList& outList, SP_UINodeDesc& spNodeDesc) override {
				if ( outList.size() == spNodeDesc->getChildNodesRef().size() )
					return;

				outList.clear();
				for(auto spChildNodeDesc : spNodeDesc->getChildNodesRef())
					outList.push_back( createChildNode( spChildNodeDesc ) );
			}
			virtual void _update_SelfRenderNodeList(UIComponentList& outList) override {
				outList.push_back( shared_from_this() );
			}
	};

	class UIComponentContainerTransparent : public UIComponentContainer {
		public:
			virtual void _init_VarLink() override {}
			virtual void _update_SelfRenderNodeList(UIComponentList& outList) override {
				for( auto& childNode : getChildNodeListRef() )
					childNode->_update_SelfRenderNodeList(outList);
			}
	};



	class UIComponentRoot : public UIComponentContainer {
		public:
		
			virtual bool getStyleRelative() override { return true; }
			virtual UIPosValVariant getStyleWidth () override { return UIPosValVariant{ UIPosValVariant::Pixel, getOuterBBoxRef().getWidth () }; }
			virtual UIPosValVariant getStyleHeight() override { return UIPosValVariant{ UIPosValVariant::Pixel, getOuterBBoxRef().getHeight() }; }

	};


}
