; ModuleID = 'basecc'
source_filename = "basecc"

@value = global i32 5
define i32 @main() {
entry:
  %t0 = alloca i8*
  %t1 = bitcast i32* @value to i8*
  store i8* %t1, i8** %t0
  %t2 = alloca i32*
  %t3 = load i8*, i8** %t0
  %t4 = bitcast i8* %t3 to i32*
  store i32* %t4, i32** %t2
  %t5 = load i32*, i32** %t2
  %t6 = load i32, i32* %t5
  ret i32 %t6
}
