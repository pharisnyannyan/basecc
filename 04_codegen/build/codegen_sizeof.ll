; ModuleID = 'basecc'
source_filename = "basecc"

%struct.Pair = type { i32, i8 }

@global_value = global i32 0
@global_ptr = global i32* @global_value
define i32 @size_int() {
entry:
  %t0 = getelementptr i32, i32* null, i32 1
  %t1 = ptrtoint i32* %t0 to i32
  ret i32 %t1
}
define i32 @size_char() {
entry:
  %t0 = getelementptr i8, i8* null, i32 1
  %t1 = ptrtoint i8* %t0 to i32
  ret i32 %t1
}
define i32 @size_short() {
entry:
  %t0 = getelementptr i16, i16* null, i32 1
  %t1 = ptrtoint i16* %t0 to i32
  ret i32 %t1
}
define i32 @size_pointer() {
entry:
  %t0 = getelementptr i32*, i32** null, i32 1
  %t1 = ptrtoint i32** %t0 to i32
  ret i32 %t1
}
define i32 @size_global() {
entry:
  %t0 = getelementptr i32, i32* null, i32 1
  %t1 = ptrtoint i32* %t0 to i32
  ret i32 %t1
}
define i32 @size_local() {
entry:
  %t0 = alloca i32
  %t1 = getelementptr i32, i32* null, i32 1
  %t2 = ptrtoint i32* %t1 to i32
  ret i32 %t2
}
define i32 @size_struct_type() {
entry:
  %t0 = getelementptr %struct.Pair, %struct.Pair* null, i32 1
  %t1 = ptrtoint %struct.Pair* %t0 to i32
  ret i32 %t1
}
define i32 @size_struct_value() {
entry:
  %t0 = alloca %struct.Pair
  %t1 = getelementptr %struct.Pair, %struct.Pair* null, i32 1
  %t2 = ptrtoint %struct.Pair* %t1 to i32
  ret i32 %t2
}
define i32 @size_deref() {
entry:
  %t0 = getelementptr i32, i32* null, i32 1
  %t1 = ptrtoint i32* %t0 to i32
  ret i32 %t1
}
