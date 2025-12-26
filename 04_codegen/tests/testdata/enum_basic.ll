; ModuleID = 'basecc'
source_filename = "basecc"

define i32 @get_red() {
entry:
  ret i32 0
}
define i32 @get_green() {
entry:
  ret i32 1
}
define i32 @get_blue() {
entry:
  ret i32 10
}
define i32 @get_yellow() {
entry:
  ret i32 11
}
@g_color = global i32 1
@g_value = global i32 10
