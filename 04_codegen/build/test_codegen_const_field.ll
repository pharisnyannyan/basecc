; ModuleID = 'basecc'
source_filename = "basecc"

%struct.Pair = type { i32, i32 }

define i32 @main() {
entry:
  %t0 = alloca %struct.Pair
  %t1 = getelementptr inbounds %struct.Pair, %struct.Pair* %t0, i32 0, i32 0
