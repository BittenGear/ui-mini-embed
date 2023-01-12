#pragma once

namespace UIMiniEmbed {
	
	struct _UIVarType {
		enum EnumVarType {
			Null,
				
			Boolean,
			I32,
			Float,
			String,
				
			StylePixel,
			StylePercent,
			StyleFraction,
			
			List,
			Map,
		};
	};
	class UIVarType : public _UIVarType {
		public:
		protected:
			EnumVarType _type = Null;
		
		public:
			EnumVarType getType() const { return _type; }
			
	};
	class UIVarInternal : public UIVarType {
		friend class UIVar;

		public:
			using SP_UIVarInternal = std::shared_ptr< UIVarInternal >;

		private:
			using TListVar = std::vector< SP_UIVarInternal >;
			using TMapVar  = std::unordered_map< std::string, SP_UIVarInternal >;
			
			bool        _bool   = false;
			int32_t     _i32    = 0;
			float       _float  = 0;
			std::string _string = "";
			
			TListVar    _list;
			TMapVar     _map;
			
			uint32_t    _invalidateSequence = 1;
			
			template< EnumVarType eNewType >
			bool _checkAndUpdateType() {
				if ( getType() == eNewType )
					return true;
				
				switch( getType() ) {
					case List: _list.clear(); break;
					case Map : _map .clear(); break;
				}
				
				_type = eNewType;
				
				return false;
			}
		
			template< EnumVarType eNewType >
			bool _setFloatEx(const float val) {
				if ( _checkAndUpdateType< eNewType >() )
					if ( _float == val )
						return false;

				const int32_t val_i32 = std::lround(val);
				
				_bool   = val_i32 ? true : false;
				_i32    = val_i32;
				_float  = val;
				_string = std::to_string(val);

				_invalidate();
				return true;
			}
		
			void _invalidate() {
				_invalidateSequence++;
			}


			bool compare(const UIVarInternal& other) const {
				if ( getType() != other.getType() ) 
					return false;

				switch( getType() ) {
					case Null   : return true;
					case Boolean: return _bool   == other._bool;
					case I32    : return _i32    == other._i32;
					case String : return _string == other._string;
				
					case Float:
					case StylePixel:
					case StylePercent:
					case StyleFraction:
						return _float == other._float;
					
					/// TODO, recursive inf...
					case List: {
						if ( _list.size() != other._list.size() )
							return false;
						
						for(size_t i = 0; i < _list.size(); i++)
							if ( !_list[i]->compare( other._list[i] ) )
								return false;
						
						return true;
					};
					
					case Map: {
						if ( _map.size() != other._map.size() )
							return false;
						
						for(const auto& rec : _map) {
							auto other_rec = other._map.find(rec.first);
							if ( other_rec == other._map.end() )
								return false;
							
							if ( !rec.second->compare( other_rec->second ) )
								return false;
						}
						
						return true;
					};
				}

				return false;
			}
			bool compare(const SP_UIVarInternal& spOther) const {
				if ( this == spOther.get() )
					return true;
				
				return compare(*spOther);
			}
			bool compareRef(const SP_UIVarInternal& spOther) const {
				return this == spOther.get();
			}
	
			void setValue(const SP_UIVarInternal& spOther) {
				*this = *spOther;
			}


			std::string dump(const size_t dp = 0) const {
				const std::string GAP = " ";
				std::string gap = "";
				for(size_t i = 0; i != dp; i++) gap += GAP;

				switch( getType() ) {
					case Null   : return gap + "null";
					case Boolean: return gap + _string;
					case I32    : return gap + _string + "i";
					case String : return gap + "\"" + _string + "\"";
				
					case Float: return gap + _string + "f";
					case StylePixel: return gap + _string + "px";
					case StylePercent: return gap + _string + "%";
					case StyleFraction: return gap + _string + "fr";
					
					/// TODO, recursive inf...
					case List: {
						std::string out = gap + "[\n";
						for(const auto item : _list)
							out += item->dump(dp + 1) + ",\n";
						out += gap + "]";
						return out;
					};
					
					case Map: {
						std::string out = gap + "{\n";
						for(const auto& rec : _map)
							out += gap + GAP + rec.first + ": " + rec.second->dump(dp + 1) + ",\n";
						out += gap + "}";
						return out;
					};
				}
				
				return "{Invalid}";
			}

			static SP_UIVarInternal create() {
				return std::make_shared< UIVarInternal >();
			}
	
	};
	using SP_UIVarInternal = std::shared_ptr< UIVarInternal >;
	
	struct _UIVar_VarRec;
	class UIVar : public _UIVarType {
		private:
			SP_UIVarInternal _spVarInternal      = UIVarInternal::create();
			uint32_t         _invalidateSequence = 0;

			bool _setVarInternal(SP_UIVarInternal spVar) {
				if ( !spVar ) 
					return false;
				
				if ( _spVarInternal.get() == spVar.get() )
					return false;
				
				_spVarInternal      = spVar;
				_invalidateSequence = 0;
				
				return true;
			}

			void _setScalar(const bool _bool, const int32_t _i32, const float _float, const std::string& _string) {
				_spVarInternal->_bool   = _bool;
				_spVarInternal->_i32    = _i32;
				_spVarInternal->_float  = _float;
				_spVarInternal->_string = _string;
			}
			
		public:
			EnumVarType        getType     () const { return _spVarInternal->getType(); }
			bool               isNull      () const { return getType() == Null; }
			bool               getBool     () const { return _spVarInternal->_bool;     }
			int32_t            getI32      () const { return _spVarInternal->_i32;      }
			float              getFloat    () const { return _spVarInternal->_float;    }
			std::string        getString   () const { return _spVarInternal->_string;   }
			const std::string& getStringRef() const { return _spVarInternal->_string;   }
			
			float              getPixel    () const { return getFloat();                }
			float              getPercent  () const { return getFloat();                }
			float              getFraction () const { return max(1, getFloat());        }
			
			std::string dump() const { return _spVarInternal->dump(); }
		
			bool setNull() {
				if ( _spVarInternal->_checkAndUpdateType< Null >() )
					return false;
				
				_spVarInternal->_bool   = false;
				_spVarInternal->_i32    = 0;
				_spVarInternal->_float  = 0;
				_spVarInternal->_string = "";
				
				_spVarInternal->_invalidate();
				return true;
			}
			bool setBool(const bool val) {
				if ( _spVarInternal->_checkAndUpdateType< Boolean >() )
					if ( _spVarInternal->_bool == val )
						return false;

				_spVarInternal->_bool   = val;
				_spVarInternal->_i32    = val ? 1 : 0;
				_spVarInternal->_float  = val ? 1 : 0;
				_spVarInternal->_string = val ? "true" : "false";
				
				_spVarInternal->_invalidate();
				return true;
			}
			
			bool setI32(const int32_t val) {
				if ( _spVarInternal->_checkAndUpdateType< I32 >() )
					if ( _spVarInternal->_i32 == val )
						return false;

				_spVarInternal->_bool   = val ? true : false;
				_spVarInternal->_i32    = val;
				_spVarInternal->_float  = val;
				_spVarInternal->_string = std::to_string(val);
				
				_spVarInternal->_invalidate();
				return true;
			}

			bool setString(const std::string& val) {
				if ( _spVarInternal->_checkAndUpdateType< String >() )
					if ( _spVarInternal->_string == val )
						return false;
				
				_spVarInternal->_bool   = val.length() ? true : false;
				_spVarInternal->_i32    = 0;
				_spVarInternal->_float  = 0;
				_spVarInternal->_string = val;
				
				_spVarInternal->_invalidate();
				return true;
			}

			bool setFloat   (const float val) { return _spVarInternal->_setFloatEx< Float         >(val); }
			bool setPixel   (const float val) { return _spVarInternal->_setFloatEx< StylePixel    >(val); }
			bool setPercent (const float val) { return _spVarInternal->_setFloatEx< StylePercent  >(val); }
			bool setFraction(const float val) { return _spVarInternal->_setFloatEx< StyleFraction >(val); }

			size_t list_GetSize() const {
				return _spVarInternal->_list.size();
			}
			bool   list_Clear() {
				if ( !_spVarInternal->_list.size() )
					return false;

				_spVarInternal->_list.clear();
				_spVarInternal->_invalidate();
				return true;
			}
			bool   list_Push(UIVar item) {
				_spVarInternal->_checkAndUpdateType< List >();
				_spVarInternal->_list.push_back(item._spVarInternal);
				_spVarInternal->_invalidate();
				return true;
			}
			UIVar  list_Get(const size_t index) {
				if ( index < _spVarInternal->_list.size() ) {
					UIVar var;
					var._setVarInternal(_spVarInternal->_list[index]);
					return var;
				}
				
				return UIVar{};
			}

			bool   map_Clear() {
				if ( !_spVarInternal->_map.size() )
					return false;
				
				_spVarInternal->_map.clear();
				_spVarInternal->_invalidate();
				return true;
			}
			UIVar  map_Get(const std::string& key) {
				if ( !_spVarInternal->_map.size() )
					return {};
				
				auto it = _spVarInternal->_map.find(key);
				if ( it == _spVarInternal->_map.end() )
					return {};
				
				UIVar var;
				var._setVarInternal(it->second);
				return var;
			}
			bool   map_Set(const std::string& key, UIVar item) {
				_spVarInternal->_checkAndUpdateType< Map >();
				_spVarInternal->_map[ key ] = item._spVarInternal;
				_spVarInternal->_invalidate();
				return true;
			}

			struct Pixel { float val = 0; };
			struct Percent { float val = 0; };
			struct Fraction { float val = 0; };
			using  VarList = std::vector< UIVar >;
			
			using  VarRec = _UIVar_VarRec;
			using  VarMap = std::vector< VarRec >;
			

			bool set(const bool         val) { return setBool(val); }
			bool set(const int32_t      val) { return setI32(val); }
			bool set(const float        val) { return setFloat(val); }
			bool set(const double       val) { return setFloat(val); }
			bool set(const std::string& val) { return setString(val); }
			bool set(const Pixel        val) { return setPixel(val.val); }
			bool set(const Percent      val) { return setPercent(val.val); }
			bool set(const Fraction     val) { return setFraction(val.val); }
			
			bool set(SP_UIVarInternal   val) { return _setVarInternal(val); }
			bool set(UIVar   val) { return _setVarInternal(val._spVarInternal); }

			void operator =(const bool         val) { set(val); }
			void operator =(const int32_t      val) { set(val); }
			void operator =(const float        val) { set(val); }
			void operator =(const double       val) { set(val); }
			void operator =(const std::string& val) { set(val); }
			
			void operator =(const Pixel        val) { set(val); }
			void operator =(const Percent      val) { set(val); }
			void operator =(const Fraction     val) { set(val); }
			
			void operator =(const VarList val) { set(val); }
			void operator =(const VarMap  val) { set(val); }
			
			
			UIVar() {}
			UIVar(const bool         val) { set(val); }
			UIVar(const int32_t      val) { set(val); }
			UIVar(const float        val) { set(val); }
			UIVar(const double       val) { set(val); }
			UIVar(const std::string& val) { set(val); }

			UIVar(const Pixel        val) { set(val); }
			UIVar(const Percent      val) { set(val); }
			UIVar(const Fraction     val) { set(val); }
			
			UIVar(const VarList      val) { set(val); }
			UIVar(const VarMap       val) { set(val); }
			
			//UIVar(SP_UIVarInternal   val) { set(val); }
			
			
			bool set(const VarList list) {
				list_Clear();
				for(auto item : list)
					list_Push(item);
				return true;
			}
			bool set(const VarMap);
			
			
			bool compare(const UIVar& other) const {
				return _spVarInternal->compare( other._spVarInternal );
			}
			bool compareRef(const UIVar& other) const {
				return _spVarInternal->compareRef( other._spVarInternal );
			}
			
			void setValue(const UIVar& other) {
				_spVarInternal->setValue( other._spVarInternal );
				_spVarInternal->_invalidate();
			}
			
			
			bool readInvalidate() {
				if ( _invalidateSequence == _spVarInternal->_invalidateSequence )
					return false;
				
				_invalidateSequence = _spVarInternal->_invalidateSequence;
				return true;
			}

			void operator =(SP_UIVarInternal spVar) {
				_setVarInternal(spVar);
			}
			void operator =(UIVar varWrapper) {
				_setVarInternal(varWrapper._spVarInternal);
			}
			
	};
	
	struct _UIVar_VarRec {
		std::string key;
		UIVar       val;
	};
	bool UIVar::set(const UIVar::VarMap list) {
		map_Clear();
		for(auto rec : list)
			map_Set(rec.key, rec.val);
		return true;
	}




	using SP_UIVarEnv = std::shared_ptr< class UIVarEnv >;
	class UIVarEnv {
		private:
			using SP_UIVarEnv = std::shared_ptr< UIVarEnv >;
			
			std::unordered_map< std::string, UIVar > _map;
			SP_UIVarEnv                      _parent = nullptr;

		public:
			UIVar getVar(const std::string& name, const bool searchParent = false) {
				auto it = _map.find(name);
				if ( it != _map.end() )
					return it->second;
				
				if ( searchParent && _parent )
					return _parent->getVar(name, true);
				
				auto var = UIVar{};
				_map[name] = var;
				return var;
			}
			
			void  setVar(const std::string& name, UIVar var) {
				_map[ name ] = var;
			}
			
			static SP_UIVarEnv create(SP_UIVarEnv parent = nullptr) {
				auto sp = std::make_shared< UIVarEnv >();
				sp->_parent = parent;
				return sp;
			}
	};

}
