; ModuleID = 'basecc'
source_filename = "basecc"

declare i32 @write(i32, i8*, i32)
declare i8* @malloc(i32)
declare i32 @free(i8*)
define i32 @extern_calls() {
entry:
  %t0 = alloca [4 x i8]
  %t1 = alloca i8*
  %t2 = alloca i32
  %t3 = alloca i32
  %t4 = getelementptr [4 x i8], [4 x i8]* %t0, i32 0, i32 0
  %t5 = call i32 @write(i32 1, i8* %t4, i32 0)
  store i32 %t5, i32* %t2
  %t6 = call i8* @malloc(i32 4)
  store i8* %t6, i8** %t1
  %t7 = load i8*, i8** %t1
  %t8 = call i32 @free(i8* %t7)
  store i32 %t8, i32* %t3
  %t9 = load i32, i32* %t2
  ret i32 %t9
}
