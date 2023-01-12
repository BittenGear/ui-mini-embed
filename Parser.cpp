#pragma once

#include "UINodeDesc.cpp"

namespace UIMiniEmbed::Parser {
	
	class ParserNode {
		public:
			using SP_ParserNode = std::shared_ptr< ParserNode >;
			using TList = std::vector< SP_ParserNode >;

			enum EnumNodeType {
				Null,
				String,
				
				ConstBool,
				ConstString,
				ConstNumber,
				ConstNumberPixel,
				ConstNumberPercent,
				ConstNumberFraction,
				
				VarExternal,
				
				OpAssign,
				OpDeclare,
				OpFetch,
				
				Rule,
				Props,
				ComponentAlias,
				
				SpaceBlock,
				SpaceBlockGroup,
				
				LineList,
				
				Comment,
				
				LineSpace,
			};
		
		private:
			EnumNodeType _type = Null;
			TList        _nodes;
			std::string  _value = "";
		
		public:
			bool isNull() const { return _type == Null; }
			bool isString() const { return _type == String; }
			
			std::string getValue() const { return _value; }
	
			static auto create(const EnumNodeType type, const std::string& value) {
				auto spNode = std::make_shared< ParserNode >();
				spNode->_type  = type;
				spNode->_value = value;
				return spNode;
			}
			
			static auto create(const EnumNodeType type, const TList& list) {
				auto spNode = std::make_shared< ParserNode >();
				spNode->_type  = type;
				spNode->_nodes = list;
				return spNode;
			}
			static auto create(const EnumNodeType type, const char value) {
				std::string s;
				s += value;
				return create(type, s);
			}
			static auto create() { return create(Null, ""); }
			
			std::string dump(const size_t dp = 0) const {
				std::string sType = "";
				switch(_type) {
					case Null: sType = "Null"; break;
					case String: sType = "String"; break;
					
					case ConstBool: sType = "ConstBool"; break;
					case ConstString: sType = "ConstString"; break;
					case ConstNumber: sType = "ConstNumber"; break;
					case ConstNumberPixel: sType = "ConstNumberPixel"; break;
					case ConstNumberPercent: sType = "ConstNumberPercent"; break;
					case ConstNumberFraction: sType = "ConstNumberFraction"; break;
					
					case OpAssign: sType = "OpAssign"; break;
					case OpDeclare: sType = "OpDeclare"; break;
					case OpFetch: sType = "OpFetch"; break;
					case VarExternal: sType = "VarExternal"; break;
					
					case Rule: sType = "Rule"; break;
					case Props: sType = "Props"; break;
					case ComponentAlias: sType = "ComponentAlias"; break;
					
					case SpaceBlock: sType = "SpaceBlock"; break;
					case SpaceBlockGroup: sType = "SpaceBlockGroup"; break;
					 
					case LineList: sType = "LineList"; break;
					case Comment: sType = "Comment"; break;

					case LineSpace: sType = "LineSpace"; break;
					 
				}
				
				std::string gap = "";
				for(size_t i = 0; i != dp; i++)
					gap += "  ";
				
				std::string text = gap + sType + ": \"" + _value + "\"\n";
				for(auto node : _nodes) {
					text += node->dump(dp + 1);
				}
				return text;
			}
			
			EnumNodeType getType() const { return _type; }
			bool isType(const std::vector< EnumNodeType >& typeList) {
				for(const auto type : typeList)
					if ( getType() == type )
						return true;
				return false;
			}

			static constexpr size_t AnyCount = 0xFFFFFF;
			struct TPathItem {
				size_t       nodeIndex = 0;
				EnumNodeType eType     = EnumNodeType::Null;
				size_t       nodeCount = AnyCount;
			};
			SP_ParserNode getNodeByPath(std::vector< TPathItem > path) {
				if ( path.size() ) {
					const auto [ ni, type, nc ] = path[0];
					path.erase(path.begin());

					if ( (0 <= ni) && (ni < getNodeCount()) ) {
						auto spNode = getNode(ni);
						if ( spNode->getType() == type ) {
							if ( nc == AnyCount || spNode->getNodeCount() == nc ) {
								if ( !path.size() )
									return spNode;
								
								return spNode->getNodeByPath(path);
							}
						}
					}
				}
				
				return create();
			}
			SP_ParserNode getByPath(EnumNodeType eType, size_t nodeCount, const std::vector< TPathItem >& path) {
				if ( eType == getType() )
					if ( nodeCount == AnyCount || nodeCount == getNodeCount() )
						return getNodeByPath(path);
				
				return create();
			}
			template< class TFun >
			bool getByPathCallNext(EnumNodeType eType, size_t nodeCount, const std::vector< TPathItem >& path, TFun fNext) {
				auto node = getByPath(eType, nodeCount, path);
				if ( !node->isNull() )
					return fNext(node);
				return false;
			}

			TList getNodes() const {
				return _nodes;
			}
			
			SP_ParserNode getNode(const size_t i) {
				if ( i < _nodes.size() )
					return _nodes[i];
				
				return ParserNode::create();
			}
			size_t getNodeCount() const { return _nodes.size(); }
			
			template< class TFun >
			bool everyNode(TFun fn) {
				for(auto node : _nodes)
					if ( !fn(node) )
						return false;
				return true;
			}
	};
	using SP_ParserNode = ParserNode::SP_ParserNode;
	
	class ParserContex {
		private:
			using SP_String = std::shared_ptr< std::string >;
			
			SP_String _spText;
			size_t    _cursor = 0;
		
		public:
			ParserContex(SP_String spText, const size_t cursor = 0) : _spText(spText), _cursor(cursor) {}
			
			char getChar() const {
				if ( _cursor < _spText->length() )
					return (*_spText)[ _cursor ];
				return '\x00';
			}
			char nextChar() {
				const auto ret = getChar();
				_cursor++;
				return ret;
			}
			
			static auto create(const std::string& text) {
				return std::make_shared< ParserContex >(std::make_shared< std::string >(text), 0);
			}
			
			auto clone() {
				return std::make_shared< ParserContex >(_spText, _cursor);
			}
			
			template< class TFun >
			auto update(TFun fun) {
				auto ctx = clone();
				auto ret = fun(ctx);
				if ( !ret->isNull() )
					_cursor = ctx->_cursor;
				return ret;
			}
	};
	using SP_ParserContext = std::shared_ptr< ParserContex >;

	struct NLineBlock {
		std::string   line = "";
		SP_ParserNode node = nullptr;
	};
	using NLineBlockList = std::vector< NLineBlock >;
	
	class NLineContext {
		private:
			NLineBlockList _nLineList;
			size_t         _cursor = 0;
		
		public:
			static auto create(NLineBlockList nLineList) {
				auto sp = std::make_shared< NLineContext >();
				sp->_nLineList = nLineList;
				return sp;
			}
			bool          hasLine() const { return _cursor < _nLineList.size(); }
			SP_ParserNode getLine() {
				if ( _cursor < _nLineList.size() ) {
					auto nLine = _nLineList[ _cursor ].node;
					if ( !nLine->isNull() )
						return nLine;
				}
				
				return ParserNode::create();
			}
			std::string   getLineString() const {
				if ( _cursor < _nLineList.size() )
					return _nLineList[ _cursor ].line;
				return "";
			}
			SP_ParserNode nextLine() {
				auto ret = getLine();
				_cursor++;
				return ret;
			}
	
			
			UINodeAliasDescMap aliasMap;
	};
	using SP_NLineContext = std::shared_ptr< NLineContext >;

	namespace ParserBuildDesc {
		
		bool aMakeComponentList(UINodeChildrenGroup& out, SP_NLineContext spLineContext, const int32_t deepLevel);
		////////////////////////////
		
		bool aMakePropAssign(UINodePropDesc& outProp, SP_ParserNode nProp) {
			return nProp->getByPathCallNext(ParserNode::OpAssign, 2, { {0, ParserNode::String, 0} }, [&](auto nPropName) {
				struct Teg { ParserNode::EnumNodeType eIn; UINodePropDesc::EnumType eOut; };
				std::vector< Teg > listEgMap = {
					{ ParserNode::ConstBool          , UINodePropDesc::ConstBool, },
					{ ParserNode::ConstString        , UINodePropDesc::ConstString, },
					{ ParserNode::ConstNumber        , UINodePropDesc::ConstNumber, },
					{ ParserNode::ConstNumberPixel   , UINodePropDesc::ConstNumberPixel, },
					{ ParserNode::ConstNumberPercent , UINodePropDesc::ConstNumberPercent, },
					{ ParserNode::ConstNumberFraction, UINodePropDesc::ConstNumberFraction, },
				};
				for(const auto [inEg, outEg] : listEgMap) {
					auto nPropVal = nProp->getByPath(ParserNode::OpAssign, 2, { {1, inEg, 1}, {0, ParserNode::String, 0} });
					if ( !nPropVal->isNull() ) {
						outProp = UINodePropDesc::create( nPropName->getValue(), outEg, nPropVal->getValue() );
						return true;
					}
				}
				
				return nProp->getByPathCallNext(ParserNode::OpAssign, 2, { {1, ParserNode::VarExternal, 2}, {0, ParserNode::String, 0} }, [&](auto nVarExternalName) {
					return nProp->getByPathCallNext(ParserNode::OpAssign, 2, { {1, ParserNode::VarExternal, 2}, {1, ParserNode::OpFetch, ParserNode::AnyCount} }, [&](auto nFetch) {
						std::vector< std::string > fetchPath;
						for(size_t i = 0; i < nFetch->getNodeCount(); i++) {
							auto fetchProp = nProp->getByPath(ParserNode::OpAssign, 2, { 
								{1, ParserNode::VarExternal, 2}, 
								{1, ParserNode::OpFetch, ParserNode::AnyCount}, 
								{i, ParserNode::String, 0} 
							});
							
							if ( fetchProp->isNull() )
								return false;
							
							fetchPath.push_back( fetchProp->getValue() );
						}
						
						outProp = UINodePropDesc::create( nPropName->getValue(), UINodePropDesc::VarExternal, nVarExternalName->getValue(), fetchPath );
						return true;
					});
				});
			});
		}
		bool aMakePropDeclare(UINodePropDesc& outProp, SP_ParserNode nProp) {
			return nProp->getByPathCallNext(ParserNode::OpDeclare, 1, { {0, ParserNode::String, 0} }, [&](auto nPropName) {
				outProp = UINodePropDesc::create( nPropName->getValue(), UINodePropDesc::ConstBool, "true" );
				return true;
			});
		}
		bool aMakePropsDescList(UINodePropDescList& outList, SP_ParserNode nPropList) {
			if ( !nPropList->isType({ ParserNode::Props }) )
				return false;
			
			outList.clear();
			return nPropList->everyNode([&](auto nProp) {
				if ( nProp->isType({ ParserNode::Comment }) )
					return true;
			
				UINodePropDesc propDesc;
				if ( aMakePropAssign(propDesc,  nProp) || aMakePropDeclare(propDesc,  nProp) ) {
					outList.push_back( propDesc );
					return true;
				}
				
				return false;
			});
		};
	
		int32_t aGetDeepLevel(SP_ParserNode nRuleLine) {
			auto nSpaceBlock = nRuleLine->getByPath(ParserNode::Rule, 3, { {0, ParserNode::SpaceBlockGroup, ParserNode::AnyCount} });
			
			if ( nSpaceBlock->isNull() )
				nSpaceBlock = nRuleLine->getByPath(ParserNode::ComponentAlias, 2, { {0, ParserNode::SpaceBlockGroup, ParserNode::AnyCount} });
			
			if ( nSpaceBlock->isNull() )
				return -1;
			
			return (int32_t) nSpaceBlock->getNodeCount();
		}
		SP_UINodeDesc aMakeComponentDesc(SP_NLineContext spLineContext) {
			SP_UINodeDesc result = nullptr;
			
			auto nRuleLine = spLineContext->getLine();
			const auto flag = nRuleLine->getByPathCallNext(ParserNode::Rule, 3, { {0, ParserNode::SpaceBlockGroup, ParserNode::AnyCount} }, [&](auto nSpaceBlock) {
				auto deepLevel = aGetDeepLevel(nRuleLine);
				
				return nRuleLine->getByPathCallNext(ParserNode::Rule, 3, { {1, ParserNode::String, 0} }, [&](auto nComponent) {
					auto componentName = nComponent->getValue();
					
					return nRuleLine->getByPathCallNext(ParserNode::Rule, 3, { {2, ParserNode::Props, ParserNode::AnyCount} }, [&](auto nPropList) {
						
						UINodePropDescList propDescList;
						if ( ParserBuildDesc::aMakePropsDescList(propDescList,  nPropList) ) {
							spLineContext->nextLine();
							
							UINodeChildrenGroup nodeGroup;
							if ( !aMakeComponentList(nodeGroup, spLineContext, deepLevel + 1) )
								return false;
							
							result = UINodeDesc::create(componentName, propDescList, nodeGroup);
							return true;
						}

						return false;
					});
				});
			});
			
			if ( flag && result )
				return result;
			
			return nullptr;
		}
		
		bool aMakeComponentList(UINodeChildrenGroup& out, SP_NLineContext spLineContext, const int32_t deepLevel) {
			if ( deepLevel < 0 )
				return false;

			while( true ) {
				if ( !spLineContext->hasLine() )
					return true;
				
				auto nRuleLine = spLineContext->getLine();
				if ( nRuleLine->isNull() )
					return false;
				
				const int32_t nextDeepLevel = aGetDeepLevel(nRuleLine);
				if ( nextDeepLevel < 0 )
					return false;

				if ( deepLevel > nextDeepLevel )
					return true;
				
				if ( deepLevel < nextDeepLevel )
					return false;
			
				auto nAlias = nRuleLine->getByPath(ParserNode::ComponentAlias, 2, { {1, ParserNode::String, 0} });
				if ( !nAlias->isNull() ) {
					spLineContext->nextLine();
					
					/// only root level
					if ( deepLevel != 0 )
						return false;
					
					SP_UINodeDesc spAliasNodeDesc = aMakeComponentDesc(spLineContext);
					if ( !spAliasNodeDesc )
						return false;
					
					if ( !out.nodeAliasMap.add(nAlias->getValue(), spAliasNodeDesc) )
						return false;
					
					continue;
				}

				SP_UINodeDesc spNodeDesc = aMakeComponentDesc(spLineContext);
				if ( !spNodeDesc )
					return false;

				out.nodeDescList.push_back(spNodeDesc);
			}
			
			return true;
		}

	};
	
	struct ParserError {
		std::string errorDesc = "";
		std::string errorLine = "";
		
		bool isError() const { return errorDesc.length() || errorLine.length(); }
	};
	struct ParserResult : public ParserError {
		SP_UINodeDesc result = nullptr;
	};
	
	
	ParserResult parse(const std::string& text) {

		const char CSpace = '\x20';
		const char CTab   = '\x09';
		const auto sWordFirst = [](auto spCtx) {
			return spCtx->update([](auto spCtxCopy) {
				if ( ('a' <= spCtxCopy->getChar() && spCtxCopy->getChar() <= 'z') || 
						('A' <= spCtxCopy->getChar() && spCtxCopy->getChar() <= 'Z') || 
							( spCtxCopy->getChar() == '_' ) )
								return ParserNode::create(ParserNode::String, spCtxCopy->nextChar());			
				return ParserNode::create();
			});
		};
		const auto sWordNext = [](auto spCtx) {
			return spCtx->update([](auto spCtxCopy) {
				if ( ('a' <= spCtxCopy->getChar() && spCtxCopy->getChar() <= 'z') || 
						('A' <= spCtxCopy->getChar() && spCtxCopy->getChar() <= 'Z') || 
							('0' <= spCtxCopy->getChar() && spCtxCopy->getChar() <= '9') || 
								( spCtxCopy->getChar() == '_' ) )
									return ParserNode::create(ParserNode::String, spCtxCopy->nextChar());
				return ParserNode::create();
			});
		};
		const auto sNumberChar = [](auto spCtx) {
			return spCtx->update([](auto spCtxCopy) {
				if ( '0' <= spCtxCopy->getChar() && spCtxCopy->getChar() <= '9' )
					return ParserNode::create(ParserNode::String, spCtxCopy->nextChar());
				return ParserNode::create();
			});
		};
		const auto sSpaceChar = [](auto spCtx) {
			return spCtx->update([](auto spCtxCopy) {
				if ( '\x20' == spCtxCopy->getChar() || spCtxCopy->getChar() == '\x09' )
					return ParserNode::create(ParserNode::String, spCtxCopy->nextChar());
				return ParserNode::create();
			});
		};
		const auto sString = [](auto spCtx) {
			return spCtx->update([](auto spCtxCopy) {
				const auto firstq = spCtxCopy->nextChar();
				if ( !( firstq == '"' || firstq == '\'' ) )
					return ParserNode::create();
				
				std::string s = "";
				while( spCtxCopy->getChar() ) {
					auto c = spCtxCopy->nextChar();
					if ( c == firstq )
						return ParserNode::create(ParserNode::String, s);
					
					if ( c == '\\' )
						c = spCtxCopy->nextChar();
					
					s += c;
				}

				return ParserNode::create();
			});
		};
		const auto sEndText =  [](auto spCtx) {
			return spCtx->update([](auto spCtxCopy) {
				if ( spCtxCopy->getChar() )
					return ParserNode::create();
				
				return ParserNode::create(ParserNode::String, "");
			});
		};

		
		const auto sSpaceBlock = [=](auto spCtx) {
			return spCtx->update([=](auto spCtxCopy) {
				if ( spCtxCopy->getChar() == CSpace ) {
					for(int i = 0; i < 4; i++)
						if ( spCtxCopy->nextChar() != CSpace )
							return ParserNode::create();
						
					return ParserNode::create(ParserNode::SpaceBlock, "");
				}

				if ( spCtxCopy->nextChar() == CTab )
					return ParserNode::create(ParserNode::SpaceBlock, "");
				
				return ParserNode::create();
			});
		};

		const auto sReadLine = [](auto spCtx) {
			return spCtx->update([=](auto spCtxCopy) {
				std::string s = "";
				while( spCtxCopy->getChar() != '\r' && spCtxCopy->getChar() != '\n' && spCtxCopy->getChar() )
					s += spCtxCopy->nextChar();
				return ParserNode::create(ParserNode::String, s);
			});
		};

		const auto sIfString = [](const std::string& pattern) {
			return [=](auto spCtx) {
				return spCtx->update([=](auto spCtxCopy) {
					for(size_t i = 0; i < pattern.length(); i++)
						if ( spCtxCopy->nextChar() != pattern[i] )
							return ParserNode::create();
					return ParserNode::create(ParserNode::String, pattern);
				});
			};
		};

		const auto fRepeat = [](auto fBld) {
			return [=](auto fn) {
				return [=](auto spCtx) {
					return spCtx->update([=](auto spCtxCopy) {
						std::vector< SP_ParserNode > list;
						while(true) {
							auto n = fn(spCtxCopy);
							if ( n->isNull() )
								break;
							list.push_back(n);
						}
						return fBld(list);
					});
				};
			};
		};
		const auto fAnd = [](auto fBld) {
			return [=](std::vector< std::function< SP_ParserNode (SP_ParserContext) > > fnList) {
				return [=](auto spCtx) {
					return spCtx->update([=](auto spCtxCopy) {
						std::vector< SP_ParserNode > list;
						for(auto fn : fnList) {
							auto n = fn(spCtxCopy);
							if ( n->isNull() )
								return ParserNode::create();
							list.push_back(n);
						}
						return fBld(list);
					});
				};
			};
		};
		const auto fOr = [](std::vector< std::function< SP_ParserNode (SP_ParserContext) > > fnList) {
			return [=](auto spCtx) {
				for(auto fn : fnList) {
					auto n = spCtx->update(fn);
					if ( !n->isNull() )
						return n;
				}
				return ParserNode::create();
			};
		};
		const auto fMaybe = [](auto fBld) {
			return [=](auto fn) {
				return [=](auto spCtx) {
					return spCtx->update([=](auto spCtxCopy) {
						std::vector< SP_ParserNode > list;
						auto n = fn(spCtxCopy);
						if ( !n->isNull() )
							list.push_back(n);
						return fBld(list);
					});
				};
			};
		};
		
		const auto fOneAndRepeat = [=](auto fBld) {
			return [=](auto fn) {
				return fAnd(fBld)({ fn, fRepeat(fBld)(fn) });
			};
		};
		
		const auto fBldString = [](auto list) {
			std::string str = "";
			for(auto item : list) {
				if ( !item->isString() )
					return ParserNode::create();
				str += item->getValue();
			}
			return ParserNode::create(ParserNode::String, str);
		};
		
		const auto fBldGroup = [](const ParserNode::EnumNodeType eType) {
			return [=](const std::vector< size_t > idx) {
				return [=](auto list) {
					for(auto item : list)
						if ( item->isNull() )
							return ParserNode::create();
					
					std::vector< SP_ParserNode > finalList;
					for(const size_t id : idx) {
						if ( !( id < list.size() ) )
							return ParserNode::create();
						
						finalList.push_back( list[id] );
					}
					return ParserNode::create(eType, finalList);
				};
			};
		};
		const auto fBldGroupList = [](const ParserNode::EnumNodeType eType) {
			return [=](auto list) { return ParserNode::create(eType, list); };
		};

		const auto fBldSelect = [=](const size_t id) {
			return [=](auto list) {
				if ( !( id < list.size() ) )
					return ParserNode::create();
						
				return list[id];
			};
		};
		
		const auto sWord = fAnd(fBldString)({ sWordFirst, fRepeat(fBldString)(sWordNext) });
		const auto sNumberCharList = fOneAndRepeat(fBldString)(sNumberChar);
		const auto sSpaceMaybe = fRepeat(fBldString)(sSpaceChar);

		const auto sNextLineChar = fOr({
			fAnd(fBldString)({ sIfString("\r"), sIfString("\n"), }),
			sIfString("\r"), 
			sIfString("\n"),
		});
		
		const auto sNumber = fAnd(fBldString)({
			fMaybe(fBldString)( fOr({ sIfString("+"), sIfString("-") }) ),
			sNumberCharList,
			fMaybe(fBldString)( fAnd(fBldString)({ sIfString("."), sNumberCharList }) ),
		});
		
		const auto eVarExternal = fAnd( fBldGroup(ParserNode::VarExternal)({1, 2}) )({
			sIfString("$"),
			sWord,
			fRepeat( fBldGroupList(ParserNode::OpFetch) )( fAnd( fBldSelect(3) )({ sSpaceMaybe, sIfString("."), sSpaceMaybe, sWord, sSpaceMaybe }) ),
		});
		
		const auto eRightAll = fOr({
			fAnd( fBldGroup(ParserNode::ConstNumberPixel   )({0}) )({ sNumber, sIfString("px") }),
			fAnd( fBldGroup(ParserNode::ConstNumberPercent )({0}) )({ sNumber, sIfString("%") }),
			fAnd( fBldGroup(ParserNode::ConstNumberFraction)({0}) )({ sNumber, sIfString("fr") }),
			fAnd( fBldGroup(ParserNode::ConstNumber        )({0}) )({ sNumber }),
			fAnd( fBldGroup(ParserNode::ConstString        )({0}) )({ sString }),
			fAnd( fBldGroup(ParserNode::ConstBool          )({0}) )({ fOr({ sIfString("true"), sIfString("false") }) }),
			eVarExternal,
		});

		const auto eOpAssign = fAnd( fBldGroup(ParserNode::OpAssign)({0, 4}) )({ 
			sWord,
			sSpaceMaybe,
			sIfString("="),
			sSpaceMaybe,
			eRightAll,
			sSpaceMaybe,
		});
		
		const auto eOpDeclare = fAnd( fBldGroup(ParserNode::OpDeclare)({0}) )({ 
			sWord,
			sSpaceMaybe,
		});
		
		const auto eComment = fAnd( fBldGroup(ParserNode::Comment)({ 2 }) )({ sSpaceMaybe, sIfString("//"), sReadLine });
		
		const auto eLineSpace = fAnd( fBldGroup(ParserNode::LineSpace)({ 0 }) )({ sSpaceMaybe });
		
		const auto eComponent = fAnd( fBldGroup(ParserNode::Rule)({0, 1, 3}) )({
			fRepeat( fBldGroupList(ParserNode::SpaceBlockGroup) )( sSpaceBlock ),
			sWord,
			sSpaceMaybe,
			fRepeat( fBldGroupList(ParserNode::Props) )( fOr({ eOpAssign, eOpDeclare, eComment }) ),
		});
		
		const auto eComponentAlias = fAnd( fBldGroup(ParserNode::ComponentAlias)({0, 2}) )({
			fRepeat( fBldGroupList(ParserNode::SpaceBlockGroup) )( sSpaceBlock ),
			sIfString("@"),
			sWord,
			fOr({ eComment, sSpaceMaybe }),
		});
	
		const auto eRuleLine = fAnd( fBldSelect(0) )({ 
			fOr({ eComponentAlias, eComponent, eComment, eLineSpace, }), 
			sEndText,
		});
		
		const auto eLineList = fRepeat( fBldGroupList(ParserNode::LineList) )( fAnd( fBldSelect(0) )({ sReadLine, sNextLineChar }) );
		
		//////////////////////////////////////////////
		//////////////////////////////////////////////
		//////////////////////////////////////////////

		ParserResult result;

		NLineBlockList lineBlockList;
		auto nLineList = eLineList( ParserContex::create(text + "\n") );
		nLineList->everyNode([&](auto nLineStr) {
			if ( !nLineStr->isType({ ParserNode::String }) )
				return false;
			
			const auto lineStr = nLineStr->getValue();

			auto nRuleLine = eRuleLine( ParserContex::create(lineStr) );
			if ( nRuleLine->isNull() ) {
				result = { "Invalid line", lineStr };
				return false;
			}
			
			if ( nRuleLine->isType({ ParserNode::Comment, ParserNode::LineSpace }) )
				return true;

			lineBlockList.push_back({ nLineStr->getValue(), nRuleLine });
			return true;
		});
		
		if ( result.isError() )
			return result;
		
		auto spLineContext = NLineContext::create(lineBlockList);

		UINodeChildrenGroup nodeGroup;
		if ( !ParserBuildDesc::aMakeComponentList(nodeGroup, spLineContext, 0) ) {
			result = { "Line error", spLineContext->getLineString() };
			return result;
		}
		
		if ( spLineContext->hasLine() ) {
			result = { "Line error", spLineContext->getLineString() };
			return result;
		}

		auto nodeRoot = UINodeDesc::create("Root", {}, nodeGroup);
		UINodeDesc::TBuildAliasError buildAliasError;
		nodeRoot = nodeRoot->buildAliasSelf(buildAliasError, nodeGroup.nodeAliasMap);
		if ( !nodeRoot || buildAliasError.isError() )
			return { buildAliasError.errorText };

		nodeRoot->clearAlias();

		result.result = nodeRoot;
		
		return result;
	}

}