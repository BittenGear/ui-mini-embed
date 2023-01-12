#pragma once

namespace UIMiniEmbed {

	class UINodePropDesc {
		public:
			enum EnumType {
				ConstBool,
				ConstString,
				ConstNumber,
				ConstNumberPixel,
				ConstNumberPercent,
				ConstNumberFraction,

				VarExternal,
			};
			
		private:
			std::string                _key   = "";
			EnumType                   _type  = EnumType::ConstBool;
			std::string                _value = "";
			std::vector< std::string > _fetchPath;
		
		public:
			std::string getName     () const { return _key; }
			EnumType    getType     () const { return _type; }
			std::string getValue    () const { return _value; }
			auto        getFetchPath() const { return _fetchPath; }

			std::string dump() const {
				std::string val = "";
				switch(_type) {
					case ConstBool:
					case ConstNumber:
						val = _value;
						break;
					
					case ConstString: val = "\"" + _value + "\""; break;
					case ConstNumberPixel: val = _value + "px"; break;
					case ConstNumberPercent: val = _value + "%"; break;
					case ConstNumberFraction: val = _value + "fr"; break;
					case VarExternal: {
						val = "$" + _value;
						for(auto f : _fetchPath)
							val += "." + f;
					}
					break;
				}
				
				return _key + "=" + val;
			}
		
			static auto create(const std::string& key, const EnumType eType, const std::string value, const std::vector< std::string > fetchPath = {}) {
				UINodePropDesc nd;
				nd._key   = key;
				nd._type  = eType;
				nd._value = value;
				nd._fetchPath = fetchPath;
				return nd;
			}
	};
	using UINodePropDescList = std::vector< UINodePropDesc >;

	class UINodeDesc;
	using SP_UINodeDesc = std::shared_ptr< UINodeDesc >;

	class UINodeAliasDescMap {
		private:
			friend class UINodeDesc;
			
			std::unordered_map< std::string, std::shared_ptr< UINodeDesc > > _map;
		
		public:
			SP_UINodeDesc get(const std::string& name) {
				auto it = _map.find(name);
				if ( it == _map.end() )
					return nullptr;
				
				return it->second;
			}
			
			bool add(const std::string& name, std::shared_ptr< UINodeDesc > spNodeDesc) {
				if ( _map.find(name) != _map.end() )
					return false;
				
				_map[ name ] = spNodeDesc;
				return true;
			}

			void clear() {
				_map.clear();
			}
	};
	
	using UINodeDescList = std::vector< std::shared_ptr< UINodeDesc > >;
	struct UINodeChildrenGroup {
		UINodeDescList     nodeDescList;
		UINodeAliasDescMap nodeAliasMap;
	};
	
	class UINodeDesc {
		private:
			std::string        _componentName;
			UINodePropDescList _props;
			UINodeDescList     _childNodes;
			UINodeAliasDescMap _aliasMap;
			
			SP_UINodeDesc clone(UINodePropDescList addProps = {}) {
				auto sp = std::make_shared< UINodeDesc >();
				sp->_componentName = _componentName;
				sp->_props         = _props;
				sp->_childNodes    = _childNodes;
				sp->_aliasMap      = _aliasMap;
				
				for(auto prop : addProps)
					sp->_props.push_back(prop);
				
				return sp;
			}
		
		public:
			struct TBuildAliasError {
				std::string errorText = "";
				bool isError() { return errorText.length(); }
			};
			
			SP_UINodeDesc buildAliasSelf(TBuildAliasError& outError,  UINodeAliasDescMap& aliasMap, const std::unordered_set< std::string >& aliasSet = {}) {
				const auto componentName = getComponentName();
				
				auto aliasNode = aliasMap.get( componentName );
				if ( aliasNode ) {
					auto aliasSetCopy = aliasSet;
					if ( _childNodes.size() ) {
						outError = { "Use alias \"" + componentName + "\" component contains child nodes" };
						return nullptr;
					}
					
					if ( aliasSetCopy.find(componentName) != aliasSetCopy.end() ) {
						outError = { "Use alias \"" + componentName + "\" recursive" };
						return nullptr;
					}
					
					aliasSetCopy.insert( componentName );
					
					return aliasNode->clone(_props)->buildAliasSelf(outError,  aliasMap, aliasSetCopy);
				}
				
				UINodeDescList childNodes;
				for(auto childNode : _childNodes) {
					auto newChildNode = childNode->buildAliasSelf(outError,  aliasMap, aliasSet);
					if ( !newChildNode )
						return nullptr;
					
					childNodes.push_back( newChildNode );
				}
				
				return create(componentName, _props, { childNodes });
			}
			void clearAlias() {
				_aliasMap.clear();
				for(auto childNode : _childNodes)
					childNode->clearAlias();
			}
		
		public:
			std::string dump(const size_t dp = 0) const {
				const std::string GAP = "    ";
				std::string gap = "";
				for(size_t i = 0; i != dp; i++)
					gap += GAP;
				
				std::string out = "";
				out = gap + _componentName + " ";
				for(auto prop : _props)
					out += prop.dump() + " ";
				out += "\n";

				for(auto rec : _aliasMap._map) {
					out += gap + GAP + "@" + rec.first + "\n";
					out += rec.second->dump(dp + 1);
				}
				if ( _aliasMap._map.size() )
					out += "\n";
				
				for(auto node : _childNodes)
					out += node->dump(dp + 1);
				
				return out;
			}
		
			std::string           getComponentName() const { return _componentName; }
			UINodePropDescList    getProps        () const { return _props; }
			UINodeDescList        getChildNodes   () const { return _childNodes; }
			const UINodeDescList& getChildNodesRef() const { return _childNodes; }
		
			static SP_UINodeDesc create(const std::string& name, UINodePropDescList props, UINodeChildrenGroup childGroup) {
				auto sp = std::make_shared< UINodeDesc >();
				sp->_componentName = name;
				sp->_props = props;
				sp->_childNodes = childGroup.nodeDescList;
				sp->_aliasMap   = childGroup.nodeAliasMap;
				return sp;
			}
	};

}