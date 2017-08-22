#include <octave/oct.h>
#include <octave/Cell.h>

#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"

/*
 * 21.08.2017 Andreas Weber
 * Versuch jsonlab zu ersetzen
 */

#define INITIAL_ARRAY_SIZE 2
#define DEBUG

class MyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler>
{
public: 
 
  octave_scalar_map result;
  std::string key;
  
  bool is_array;
  bool array_is_numeric;
  unsigned array_items;
  Cell array;
  void push_back (octave_value v)
  {
    unsigned c = array.columns ();
#ifdef DEBUG
    unsigned r = array.rows ();
    std::cout << "push_back c = " << c << std::endl;
    std::cout << "push_back r = " << r << std::endl;
#endif

    if (array_items == c)
      array.resize (dim_vector (1, 2 * c));

    array(0, array_items++) = v;
  }

public:

  MyHandler ()
    : is_array (0) {};
  
  bool Null()
  {
#ifdef DEBUG
    std::cout << "Null()" << std::endl;
#endif
    result.contents (key) = Matrix();
    return true;
  }
  bool Bool(bool b)
  {
#ifdef DEBUG
    std::cout << "Bool(" << std::boolalpha << b << ")" << std::endl;
#endif
    result.contents (key) = (b)? true: false;
    return true;
  }
  bool Uint(unsigned u)
  {
#ifdef DEBUG
    std::cout << "Uint(" << u << ")" << std::endl;
#endif
    if (is_array)
      push_back (u);
    else
      result.contents (key) = octave_uint32 (u);
    return true;
  }
  bool Int(signed i)
  {
#ifdef DEBUG
    std::cout << "Int(" << i << ")" << std::endl;
#endif
    if (is_array)
      push_back (i);
    else
      result.contents (key) = octave_int32 (i);
    return true;
  }
  bool Int64(int64_t i)
  {
#ifdef DEBUG
    std::cout << "Int64(" << i << ")" << std::endl;
#endif
    if (is_array)
      push_back (i);
    else
      result.contents (key) = octave_int64 (i);
    return true;
  }
  bool Uint64(uint64_t u)
  {
#ifdef DEBUG
    std::cout << "Uint64(" << u << ")" << std::endl;
#endif
    if (is_array)
      push_back (u);
    else
      result.contents (key) = octave_uint32(u);
    return true;
  }
  bool Double(double d)
  {
#ifdef DEBUG
    std::cout << "Double(" << d << ")" << std::endl;
#endif
    if (is_array)
      push_back (d);
    else
      result.contents (key) = d;
    return true;
  }
  bool String(const char* str, rapidjson::SizeType length, bool copy)
  {
#ifdef DEBUG
    std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
#endif
    array_is_numeric = false;
    if (is_array)
      push_back (str);
    else
      result.contents (key) = str;
    return true;
  }
  bool StartObject()
  {
#ifdef DEBUG
    std::cout << "StartObject()" << std::endl;
#endif
    return true;
  }
  bool Key(const char* str, rapidjson::SizeType length, bool copy)
  {
#ifdef DEBUG
    std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
#endif
    
    key = str;
    
    return true;
  }
  bool EndObject(rapidjson::SizeType memberCount)
  {
#ifdef DEBUG
    std::cout << "EndObject(" << memberCount << ")" << std::endl;
#endif
    return true;
  }
  bool StartArray()
  {
#ifdef DEBUG
    std::cout << "StartArray()" << std::endl;
#endif
    array_items = 0;
    is_array = true;
    array_is_numeric = true;
    array.resize (dim_vector (1, INITIAL_ARRAY_SIZE));
    
    return true;
  }
  bool EndArray(rapidjson::SizeType elementCount)
  {
#ifdef DEBUG
    std::cout << "EndArray(" << elementCount << ")" << std::endl;
    std::cout << "array_items = " << array_items << std::endl;
#endif

    
    assert (elementCount == array_items);
    
    is_array = false;
    
    // return Matrix if the array didn't contains string
    if (array_is_numeric)
      {
#ifdef DEBUG
        std::cout << "array_is_numeric" << std::endl;
#endif
        RowVector tmp(array_items);
        for (unsigned k = 0; k < array_items; ++k)
          tmp (k) = array(k).double_value();
        result.contents (key) = tmp;
      }
    else
      result.contents (key) = array;
    return true;
  }
};


DEFUN_DLD (load_json, args,, "load_json (json_str)")
{
  if (args.length () != 1)
    print_usage ();
    
  MyHandler handler;
  rapidjson::Reader reader;
  std::string json = args(0).string_value ();
  rapidjson::StringStream ss(json.c_str());
  int ret = reader.Parse(ss, handler);
#ifdef DEBUG
  std::cout << json << std::endl;
  std::cout << "ret = " << ret << std::endl;
#endif

  return ovl (handler.result);
}
